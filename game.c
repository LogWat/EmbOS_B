#include "gba.h"
#include "draw.h"
#include "game.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_GRAY      BGR(15, 15, 15)
#define COLOR_BLACK     0

static enum state prev_state = HOME;            // 前回のstate(描写用)
static enum state current_state = HOME;        // 現在の状態
static enum difficulty prev_game_difficulty = EASY;  // 前回のgame_difficulty(描写用)
static enum difficulty game_difficulty = EASY;       // ゲームの難易度
static int prev_auto_play = 0;                  // 前回のauto_play(描写用)
static int auto_play = 0;                       // 自動プレイフラグ
static int optidx = 0;                          // オプション番号 (選択肢のどれを選んでいるか)
static int prev_optidx = 0;                     // 前回のオプション番号 (選択肢のどれを選んでいたか)

static int prev_key;

static int rand_seed = 0;

static void options_slct(
    int num_of_options,
    int key
) {
    if ((prev_key & KEY_DOWN) && !(key & KEY_DOWN)) {
        optidx = (++optidx > num_of_options - 1) ? 0 : optidx;
    }
    if ((prev_key & KEY_UP) && !(key & KEY_UP)) {
        optidx = (--optidx < 0) ? num_of_options - 1 : optidx;
    }
}

enum state game_get_state(void) { return current_state; }
enum state game_get_prev_state(void) { return prev_state; }
void game_set_state(enum state new_state) {
    current_state = new_state;
}
void game_set_prev_state(enum state new_state) {
    prev_state = new_state;
}
enum difficulty game_get_difficulty(void) { return game_difficulty; }
enum difficulty game_get_prev_difficulty(void) { return prev_game_difficulty; }
void game_set_difficulty(enum difficulty new_difficulty) {
    game_difficulty = new_difficulty;
}
void game_set_prev_difficulty(enum difficulty new_difficulty) {
    prev_game_difficulty = new_difficulty;
}

void auto_play_toggle(void) {
    auto_play = !auto_play;
}
void prev_auto_play_toggle(void) {
    prev_auto_play = !prev_auto_play;
}
int get_autoplay(void) {
    return auto_play;
}
int get_prev_autoplay(void) {
    return prev_auto_play;
}

int get_optidx(void) { return optidx; }
int get_prev_optidx(void) { return prev_optidx; }
void set_prev_optidx(int new_optidx) { prev_optidx = new_optidx; }

// main関数によって常時カウントアップされている
void rand_countup(void) {
    rand_seed = (++rand_seed > 0xFFFF) ? 0 : rand_seed;
}

// 疑似乱数を取得する (0～10)
int getrand(void) {
    int r = rand_seed;
    r = (r * 3 + 7);
    rand_countup();
    return r % 11;
}

int getrandseed(void) {
    return rand_seed;
}


void game_step(void)
{
    int key = gba_register(KEY_STATUS);
    switch (game_get_state()) {
    case START:
        /* ゲーム準備状態 */
        /* 次のティックはRUNNING状態にする．*/
        game_set_state(RUNNING);
        break;
    case RUNNING:
        // cheat code
        if (!(key & KEY_SELECT)) {
            game_set_state(CLEAR);
            screen_changed_flag_set();
        }
        break;
    case DEAD:
        options_slct(2, key);
        if ((prev_key & KEY_A) && !(key & KEY_A)) {
            switch (optidx) {
            case 0:
                game_set_state(HOME);
                screen_changed_flag_set();
                break;
            case 1:
                game_set_state(RESTART);
                screen_changed_flag_set();
                break;
            }
        }
        break;
    case RESTART:
        /* 次のティックはRUNNING状態にする．*/
        game_set_state(RUNNING);
        break;
    case CLEAR:
        if (!(key & KEY_START)) {
            game_set_state(HOME);
            screen_changed_flag_set();
        }
        break;
    case HOME:
        options_slct(3, key);
        if ((prev_key & KEY_A) && !(key & KEY_A)) {
            switch (optidx) {
            case 0:
                auto_play_toggle();
                break;
            case 1:
                switch (game_get_difficulty()) {
                case EASY:
                    game_set_difficulty(NORMAL);
                    break;
                case NORMAL:
                    game_set_difficulty(HARD);
                    break;
                case HARD:
                    game_set_difficulty(INSANE);
                    break;
                case INSANE:
                    game_set_difficulty(EASY);
                    break;
                default:
                    break;
                }
                break;
            case 2:
                game_set_state(START); // ゲームを開始する
                screen_changed_flag_set();
                optidx = 0;
                break;
            }
        }
        break;
    case STOP:
        break;
    case SETTING:
        break;
    case SCORE:
        break;
    }
    prev_key = key;
}
