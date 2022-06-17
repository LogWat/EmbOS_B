#include "gba.h"
#include "ball.h"
#include "racket.h"
#include "box.h"
#include "game.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_BLACK     BGR(0, 0, 0)

static int dx = 5, dy = 0;            /* ラケットの現在の速度 */
static struct box r = {LCD_WIDTH - 100, LCD_HEIGHT - 30, 30, 5};          /* ラケットの箱の現在の位置 */

// 疑似平方根(整数のみ)
static int sqrt(int x) {
    int i, j;
    for (i = 0, j = 1; i < x; i++) {
        if (j * j <= x)
            j++;
        else
            break;
    }
    return j - 1;
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
        int bv = ball_get_dx() * ball_get_dx() + ball_get_dy() * ball_get_dy();
        if (cross(&r, b)) {
            ball_set_dy(ball_get_dy() < 0 ? ball_get_dy() : -ball_get_dy());
            if (b->x + b->width/2 < r.x + r.width/2) {
                ball_set_dx(ball_get_dx() - (r.x + r.width/2 - b->x - b->width/2)/6);
            } else if (b->x + b->width/2 > r.x + r.width/2) {
                ball_set_dx(ball_get_dx() + (b->x + b->width/2 - r.x - r.width/2)/6);
            }
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
    }
}
