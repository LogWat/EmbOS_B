#include "gba.h"
#include "ball.h"
#include "racket.h"
#include "box.h"
#include "game.h"
#include "block.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_BLACK     BGR(0, 0, 0)

static int dx = 5, dy = 0;            /* ラケットの現在の速度 */
static struct box r = {LCD_WIDTH - 100, LCD_HEIGHT - 30, 30, 5};          /* ラケットの箱の現在の位置 */

static void bounce_angle() {
    struct box *b = ball_get_box();
    fix ball_dx = ball_get_dx(), ball_dy = ball_get_dy(); // fix: 整数部24bit, 小数部8bit
    int dx = round_fix(ball_dx), dy = round_fix(ball_dy);
    int v = (dx * dx + dy * dy);
    fix new_ball_dx, new_ball_dy = 0;
    if (b->x + b->width/2 > r.x + r.width/2) {
        new_ball_dx = ball_dx + (((b->x + b->width / 2) - (r.x + r.width / 2)) << 2);
    } else {
        new_ball_dx = ball_dx - (((r.x + r.width / 2) - (b->x + b->width / 2)) << 2);
    }
    new_ball_dy = ball_dy;
    ball_set_dx(new_ball_dx);
    ball_set_dy(new_ball_dy);
}

static void racket_init(int *x, int *y) {
    *x = LCD_WIDTH - 100; *y = LCD_HEIGHT - 30;
    dx = 5; dy = 0;
    r.x = *x; r.y = *y;
    r.width = 30; r.height = 5;
}

void racket_step(void) {
    int key = gba_register(KEY_STATUS);

    int x = r.x, y = r.y;
    int next_racket_sizex;
    int rev_flag = 0;

    switch (game_get_state()) {
    case START:
        racket_init(&x, &y);
        break;
    case RUNNING:
        x = r.x;
        y = r.y;
        dx = 5 + get_speed_by_blocks() / 2;
        rev_flag = get_reverse_flag(); // 操作反転状態かどうか
        if (r.x > dx && (!rev_flag ? !(key & KEY_LEFT) : !(key & KEY_RIGHT))) {
            x -= dx;
        } else if (r.x < LCD_WIDTH - r.width - dx && (!rev_flag ? !(key & KEY_RIGHT) : !(key & KEY_LEFT))) {
            x += dx;
        }
        struct box *b = ball_get_box();
        if (cross(&r, b)) {
            // bounce_angle();
            ball_set_dy(ball_get_dy() < 0 ? ball_get_dy() : -ball_get_dy());
        }
        if ((next_racket_sizex = get_width_by_blocks()) != r.width) {
            move_box(&r, r.x, r.y, COLOR_BLACK); // 前の大きさのラケットを消す
            r.width = next_racket_sizex;
        }
        move_box(&r, x, y, COLOR_WHITE);
        break;
    case DEAD:
        break;
    case RESTART:
        move_box(&r, x, y, COLOR_BLACK);
        racket_init(&x, &y);
        break;
    case CLEAR:
        break;
    case HOME:
        break;
    case STOP:
        break;
    case SETTING:
        break;
    case SCORE:
        break;
    }
}
