#include "gba.h"
#include "8x8.til"
#include "game.h"
#include "draw.h"
#include "box.h"
#include "block.h"
#include "ball.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_GRAY      BGR(15, 15, 15)
#define COLOR_RED       BGR(31, 0, 0)
#define COLOR_BLUE      BGR(0, 0, 31)
#define COLOR_GREEN     BGR(0, 31, 0)
#define COLOR_PURPLE    BGR(31, 0, 31)
#define COLOR_BLACK     0
#define FONT_SIZE       8

/* screen_changed flag の使い方 */
/* game.cでスクリーンになにか変更があった場合には1をセットする． */
/* draw.cではこのフラグが1だった場合には描写を行い，0にもどす．0だった場合には何もしない */
/* そうすることで，何度も重ねて同じ描写を行わないようにする． */
static int screen_changed = 1; // 最初は描写を行う(START画面)

static int prev_rev_flag = 0; // 前ループでのrev_flag状態

void screen_changed_flag_set(void) {
    screen_changed = 1;
}
void screen_changed_flag_reset(void) {
    screen_changed = 0;
}

int strlen(const char *str) {
    int i = 0;
    while (str[i] != '\0') {
        i++;
    }
    return i;
}

void draw_char(hword *ptr, hword color, int code)
{
    hword   *p;
    int     i, j;
    unsigned char   *font = char8x8[code];
    for (i = 0; i < FONT_SIZE; i++) {
        p = ptr + LCD_WIDTH * i;
        for (j = FONT_SIZE - 1; j >= 0; j--, p++) {
            if (font[i] & (1 << j))
                *p = color;
        }
    }
}

void draw_string(hword *ptr, hword color, char *str, int x, int y)
{
    for (ptr += x + y * LCD_WIDTH; *str; str++, ptr += FONT_SIZE) {
        draw_char(ptr, color, *str);
    }
}

void draw_number(hword *ptr, hword color, int num, int x, int y)
{
    int i;
    char str[10];
    for (i = 0; i < 10; i++, num /= 10) {
        str[9 - i] = num % 10 + '0';
    }
    draw_string(ptr, color, str, x, y);
}

static void opt_slct(int num_of_options, int base_y, int interval) {
    if (num_of_options == 0) {
        return;
    }
    hword *fb = (hword *)VRAM;
    int i;
    int h = base_y;
    for (i = 0; i < num_of_options; i++) {
        if (i == get_optidx()) {
            draw_string(fb, COLOR_WHITE, "->", 20, h + interval * (i+1));
        } else if (i == get_prev_optidx()) {
            draw_string(fb, COLOR_BLACK, "->", 20, h + interval * (i+1));
        }
    }
}

static void draw_difficulty(void) {
    hword *fb = (hword *)VRAM;
    if (game_get_difficulty() != game_get_prev_difficulty()) {
        // 前回の難易度を消去
        switch(game_get_prev_difficulty()) {
            case EASY:
                draw_string(fb, COLOR_BLACK, "EASY", LCD_WIDTH / 2 + FONT_SIZE * 5, LCD_HEIGHT / 4 +  (FONT_SIZE+4) * 2);
                break;
            case NORMAL:
                draw_string(fb, COLOR_BLACK, "NORMAL", LCD_WIDTH / 2 + FONT_SIZE * 5, LCD_HEIGHT / 4 +  (FONT_SIZE+4) * 2);
                break;
            case HARD:
                draw_string(fb, COLOR_BLACK, "HARD", LCD_WIDTH / 2 + FONT_SIZE * 5, LCD_HEIGHT / 4 +  (FONT_SIZE+4) * 2);
                break;
            case INSANE:
                draw_string(fb, COLOR_BLACK, "INSANE", LCD_WIDTH / 2 + FONT_SIZE * 5, LCD_HEIGHT / 4 +  (FONT_SIZE+4) * 2);
                break;
        }
    }
    // 現在の難易度を描写
    switch(game_get_difficulty()) {
        case EASY:
            draw_string(fb, COLOR_GREEN, "EASY", LCD_WIDTH / 2 + FONT_SIZE * 5, LCD_HEIGHT / 4 +  (FONT_SIZE+4) * 2);
            break;
        case NORMAL:
            draw_string(fb, COLOR_BLUE, "NORMAL", LCD_WIDTH / 2 + FONT_SIZE * 5, LCD_HEIGHT / 4 +  (FONT_SIZE+4) * 2);
            break;
        case HARD:
            draw_string(fb, COLOR_RED, "HARD", LCD_WIDTH / 2 + FONT_SIZE * 5, LCD_HEIGHT / 4 +  (FONT_SIZE+4) * 2);
            break;
        case INSANE:
            draw_string(fb, COLOR_PURPLE, "INSANE", LCD_WIDTH / 2 + FONT_SIZE * 5, LCD_HEIGHT / 4 +  (FONT_SIZE+4) * 2);
            break;
    }
}

static void draw_autoplay_status(void) {
    hword *fb = (hword *)VRAM;
    if (get_autoplay() != get_prev_autoplay()) {
        // 前回の自動プレイ状態を消去
        if (get_prev_autoplay()) {
            draw_string(fb, COLOR_BLACK, "ON", LCD_WIDTH / 2 + FONT_SIZE * 5, LCD_HEIGHT / 4 + FONT_SIZE + 4);
        } else {
            draw_string(fb, COLOR_BLACK, "OFF", LCD_WIDTH / 2 + FONT_SIZE * 5, LCD_HEIGHT / 4 + FONT_SIZE + 4);
        }
    }
    // 現在の自動プレイ状態を描写
    draw_string(fb, COLOR_WHITE, "Auto Play: ", LCD_WIDTH / 2 - FONT_SIZE * 10, LCD_HEIGHT / 4 + FONT_SIZE + 4);
    if (!get_autoplay()) {
        draw_string(fb, COLOR_BLUE, "OFF", LCD_WIDTH / 2 + FONT_SIZE * 5, LCD_HEIGHT / 4 + FONT_SIZE + 4);
    } else {
        draw_string(fb, COLOR_GREEN, "ON", LCD_WIDTH / 2 + FONT_SIZE * 5, LCD_HEIGHT / 4 + FONT_SIZE + 4);
    }
}

void reset_screen(void)
{
    hword *fb = (hword *)VRAM;
    int i, j;
    for (i = 0; i < LCD_HEIGHT; i++) {
        for (j = 0; j < LCD_WIDTH; j++) {
            *(fb + i * LCD_WIDTH + j) = COLOR_BLACK;
        }
    }
}

void draw_step() {
    hword *fb = (hword *)VRAM;
    struct box *ball;

    if (screen_changed) reset_screen();

    switch (game_get_state()) {
    case START:
        if (screen_changed) {
            reset_screen();
            prev_rev_flag = 0;
        }
        break;
    case RUNNING:
        if (prev_rev_flag != get_reverse_flag() && prev_rev_flag == 0) {
            draw_string(fb, COLOR_PURPLE, "<->", 5, 0);
            prev_rev_flag = get_reverse_flag();
        } else if (prev_rev_flag != get_reverse_flag() && prev_rev_flag == 1) {
            draw_string(fb, COLOR_BLACK, "<->", 5, 0);
            prev_rev_flag = get_reverse_flag();
        }
        // ボールが表示域内にある場合は再描写
        ball = ball_get_box();
        if (get_reverse_flag() == 1) {
            if (ball->x <= 5 + FONT_SIZE * 3 && ball->y <= FONT_SIZE + 1) {
                draw_string(fb, COLOR_PURPLE, "<->", 5, 0);
            }
        }
        break;
    case DEAD:
        if (screen_changed) {
            draw_string(fb, COLOR_RED, "Game Over", LCD_WIDTH / 2 - FONT_SIZE * 5, LCD_HEIGHT / 2);
            draw_string(fb, COLOR_WHITE, "Back To HOME", LCD_WIDTH / 2 - FONT_SIZE * 6, LCD_HEIGHT / 2 + (FONT_SIZE + 4));
            draw_string(fb, COLOR_WHITE, "Continue", LCD_WIDTH / 2 - FONT_SIZE * 4, LCD_HEIGHT / 2 + (FONT_SIZE + 4) * 2);
            opt_slct(2, LCD_HEIGHT / 2, (FONT_SIZE + 4));
        }
        if (get_optidx() != get_prev_optidx()) {
            opt_slct(2, LCD_HEIGHT / 2, (FONT_SIZE + 4));
            set_prev_optidx(get_optidx());
        }
        break;
    case CLEAR:
        if (screen_changed) {
            draw_string(fb, COLOR_GREEN, "Game Clear!!", LCD_WIDTH / 2 - FONT_SIZE * 5, LCD_HEIGHT / 2);
            draw_string(fb, COLOR_WHITE, "Press START to back to HOME", LCD_WIDTH / 2 - FONT_SIZE * 14, LCD_HEIGHT / 2 + FONT_SIZE + 4);
        }
        break;
    case RESTART:
        if (screen_changed) {
            reset_screen();
            prev_rev_flag = 0;
        }
        break;
    case HOME:
        if (screen_changed) {
            draw_autoplay_status();
            draw_string(fb, COLOR_WHITE, "Difficulty: ", LCD_WIDTH / 2 - FONT_SIZE * 10, LCD_HEIGHT / 4 + (FONT_SIZE+4) * 2);
            draw_difficulty();
            draw_string(fb, COLOR_WHITE, "Play!", LCD_WIDTH / 2 - FONT_SIZE * 2, LCD_HEIGHT / 4 +  (FONT_SIZE+4) * 3);
            opt_slct(3, LCD_HEIGHT / 4, FONT_SIZE + 4);
        }
        if (get_optidx() != get_prev_optidx()) {
            opt_slct(3, LCD_HEIGHT / 4, FONT_SIZE + 4);
            set_prev_optidx(get_optidx());
        }
        if (game_get_difficulty() != game_get_prev_difficulty()) {
            draw_difficulty(); 
            game_set_prev_difficulty(game_get_difficulty());
        }
        if (get_autoplay() != get_prev_autoplay()) {
            draw_autoplay_status();
            prev_auto_play_toggle();
        }
        break;
    case STOP:
        break;
    case SETTING:
        break;
    case SCORE:
        break;
    }
    if (screen_changed) screen_changed_flag_reset();
}
