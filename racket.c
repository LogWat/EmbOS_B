#include "gba.h"
#include "ball.h"
#include "racket.h"
#include "box.h"
#include "game.h"
#include "block.h"
#include "draw.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_BLACK     BGR(0, 0, 0)
#define FONT_SIZE       8

static int dx = 5, dy = 0;            /* ラケットの現在の速度 */
static struct box r = {LCD_WIDTH - 100, LCD_HEIGHT - 30, 30, 5};          /* ラケットの箱の現在の位置 */

static void racket_init(int *x, int *y) {
    *x = LCD_WIDTH - 100; *y = LCD_HEIGHT - 30;
    dx = 5 + 3 * (game_get_difficulty() == INSANE); dy = 0;
    r.x = *x; r.y = *y;
    r.width = 30; r.height = 5;
}

// ボールの落下地点を計算する夢の関数
static int get_fallpoint() {
    struct box *b = ball_get_box();
    int x = b->x, y = b->y;
    int dx = round_fix(ball_get_dx()), dy = round_fix(ball_get_dy());
    int updown = 0, leftright = 0;
    while (y < r.y) {
        x += dx; y += dy;
        updown = leftright = 0;
        if (hit(x, y)) {
            updown++; leftright++;
        }
        if (hit(x, y + b->height)) {
            updown--; leftright++;
        }
        if (hit(x + b->width, y)) {
            updown++; leftright--;
        }
        if (hit(x + b->width, y + b->height)) {
            updown--; leftright--;
        }
        // ブロックによるボールの進行方向の変化を予測する
        if (updown && ((dy > 0 && updown < 0) || (dy < 0 && updown > 0))) {
            dy = -dy;
        }
        if (leftright && ((dx > 0 && leftright < 0) || (dx < 0 && leftright > 0))) {
            dx = -dx;
        }
        // 壁によるボールの進行方向の変化を予測する
        if (x < 0) {
            x = 0;
            dx = -dx;
        } else if (x > LCD_WIDTH - b->width) {
            x = LCD_WIDTH - b->width;
            dx = -dx;
        }
        // 天井によるボールの進行方向の変化を予測する
        if (y < 0) {
            y = 0;
            dy = -dy;
        }
    }
    return x;
}

void racket_step(void) {
    int key = gba_register(KEY_STATUS);

    int x = r.x, y = r.y;
    int next_racket_sizex, next_racket_posy;
    int rev_flag = 0;
    int auto_flag = 0;
    struct box *b;
    int td; // ボールがラケットの位置まで落下する時間

    switch (game_get_state()) {
    case START:
        racket_init(&x, &y);
        break;
    case RUNNING:
        x = r.x;
        y = r.y;
        dx = 5 + get_speed_by_blocks() / 2;
        rev_flag = get_reverse_flag(); // 操作反転状態かどうか
        auto_flag = get_autoplay(); // 自動プレイ状態かどうか
        if (r.x > dx && (!rev_flag ? !(key & KEY_LEFT) : !(key & KEY_RIGHT)) && !auto_flag) {
            x -= dx;
        } else if (r.x < LCD_WIDTH - r.width - dx && (!rev_flag ? !(key & KEY_RIGHT) : !(key & KEY_LEFT)) && !auto_flag) {
            x += dx;
        }
        if (auto_flag) {
            td = get_fallpoint();
            if (td > r.x + r.width - 1 && r.x < LCD_WIDTH - r.width - dx) {
                x += dx;
            } else if (td < r.x + 1 && r.x > dx) {
                x -= dx;
            }
            
        }

        b = ball_get_box();
        if (cross(&r, b)) {
            ball_set_dy(ball_get_dy() < 0 ? ball_get_dy() : -ball_get_dy());
        }
        if ((next_racket_sizex = get_width_by_blocks()) != r.width) {
            move_box(&r, r.x, r.y, COLOR_BLACK); // 前の大きさのラケットを消す
            r.width = next_racket_sizex;
        }
        if ((next_racket_posy = get_pos_by_blocks()) != r.y) {
            move_box(&r, r.x, r.y, COLOR_BLACK); // 前の位置のラケットを消す
            r.y = next_racket_posy;
            y = r.y;
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
