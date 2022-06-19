#include "gba.h"
#include "draw.h"
#include "game.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_GRAY      BGR(15, 15, 15)
#define COLOR_BLACK     0

static enum state current_state = START;  // 現在の状態
static enum state game_difficulty = EASY; // ゲームの難易度
static int auto_play = 0;                // 自動プレイフラグ
static int optidx = 0;                   // オプション番号 (選択肢のどれを選んでいるか)

static void options_slct(
    int num_of_options,
    int key
) {
    if (!(key & KEY_DOWN)) {
        screen_changed_flag_set();
        optidx = (optidx + 1) % num_of_options;
    }
    if (!(key & KEY_UP)) {
        screen_changed_flag_set();
        optidx = (optidx + num_of_options - 1) % num_of_options;
    }
}

enum state game_get_state(void) { return current_state; }
void game_set_state(enum state new_state) {
    current_state = new_state;
}
enum state game_get_difficulty(void) { return game_difficulty; }
void game_set_difficulty(enum state new_difficulty) {
    game_difficulty = new_difficulty;
}

void auto_play_toggle(void) {
    auto_play = !auto_play;
}
int get_autoplay(void) {
    return auto_play;
}

int get_optidx(void) { return optidx; }

void game_step(void)
{
    int key = gba_register(KEY_STATUS);
    switch (game_get_state()) {
    case START:
        if (!(key & KEY_START)) {
            game_set_state(HOME);
            screen_changed_flag_set();
        }
        break;
    case RUNNING:
        // cheat code
        if (!(key & KEY_SELECT)) {
            game_set_state(STOP);
        }
        break;
    case DEAD:
        if (!(key & KEY_START)) {
            game_set_state(RESTART);
        }
        break;
    case RESTART:
        /* 次のティックはRUNNING状態にする．*/
        game_set_state(RUNNING);
        break;
    case CLEAR:
        if (!(key & KEY_START)) {
            game_set_state(RESTART);
        }
        break;
    case HOME:
        options_slct(4, key);
        if (!(key & KEY_A)) {
            switch (optidx) {
            case 0:
                game_set_state(SETTING); // 設定画面へ
                optidx = 0;
                break;
            case 1:
                auto_play_toggle();
                break;
            case 2:
                switch (game_get_difficulty()) {
                case EASY:
                    game_set_difficulty(NORMAL);
                    break;
                case NORMAL:
                    game_set_difficulty(HARD);
                    break;
                case HARD:
                    game_set_difficulty(EASY);
                    break;
                }
                break;
            case 3:
                game_set_state(RUNNING); // ゲームを開始する
                optidx = 0;
                break;
            }
            screen_changed_flag_set();
        }
        break;
    case STOP:
        break;
    case SETTING:
        break;
    case SCORE:
        break;
    }

}
