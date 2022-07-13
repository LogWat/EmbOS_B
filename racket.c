#include "gba.h"
#include "ball.h"
#include "racket.h"
#include "box.h"
#include "game.h"

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

void racket_step(void) {
    int key = gba_register(KEY_STATUS);

    int x = r.x, y = r.y;

    switch (game_get_state()) {
    case START:
        x = LCD_WIDTH - 100; y = LCD_HEIGHT - 30;
        dx = 5; dy = 0;
        break;
    case RUNNING:
        x = r.x;
        y = r.y;
        if (r.x > dx && !(key & KEY_LEFT)) {
            x -= dx;
        } else if (r.x < LCD_WIDTH - r.width - dx && !(key & KEY_RIGHT)) {
            x += dx;
        }
        struct box *b = ball_get_box();
        if (cross(&r, b)) {
            bounce_angle();
            ball_set_dy(ball_get_dy() < 0 ? ball_get_dy() : -ball_get_dy());
        }
        move_box(&r, x, y, COLOR_WHITE);
        break;
    case DEAD:
        break;
    case RESTART:
        move_box(&r, x, y, COLOR_BLACK);
        x = LCD_WIDTH - 100; y = LCD_HEIGHT - 30;
        dx = 5; dy = 0;
        break;
    case CLEAR:
        draw_box(&r, x, y, COLOR_BLACK);
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
