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

// ※hit 時に呼ばれる前提
// ボールの反射角度を調節
static void bounce_angle() {
    struct box *b = ball_get_box();
    int distance = (r.x + r.width / 2) - (b->x + b->width / 2); // ラケットとボールの距離(中心基準)
    fix prev_dx = ball_get_dx(), prev_dy = ball_get_dy(); // ボールの調整前の速度(x, y成分)
    int prev_dx_int = round_fix(prev_dx), prev_dy_int = round_fix(prev_dy); // ボールの調整前の速度(整数)
    int prev_v = prev_dx_int * prev_dx_int + prev_dy_int * prev_dy_int; // ボールの調整前の速度(平方)
    fix new_dx = prev_dx + (distance << 5), new_dy = 1; // ボールの調整後の速度(x, y成分)
    int new_dx_int = round_fix(new_dx), new_dy_int = round_fix(new_dy); // ボールのx方向の速度(整数)
    int v = new_dx_int * new_dx_int + new_dy_int * new_dy_int; // ボールのx方向の速度(平方)
    
    // y成分が最低値(1)でnew_v が prev_v より大きい場合は、x成分を変えるしかない
    int c = 0;
    if (v > prev_v) {
        while (v > prev_v && c < 255) {
            c++;
            new_dx += (new_dx < 0) ? 1 : -1;
            new_dx_int = round_fix(new_dx);
            v = new_dx_int * new_dx_int + new_dy_int * new_dy_int;
        }
    } else {
        // y成分が最低値(1)でnew_v が prev_v より小さい場合は、y成分を大きくできる
        while (v < prev_v && c < 255) {
            c++;
            new_dy += 1;
            new_dy_int = round_fix(new_dy);
            v = new_dx_int * new_dx_int + new_dy_int * new_dy_int;
        }
    }

    // y方向をもとのy方向に合わせる
    new_dy *= (prev_dy < 0 ? -1 : 1);

    ball_set_dx(new_dx);
    ball_set_dy(prev_dy);
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
    int auto_flag = 0;
    struct box *b;
    int ball_dx, d_y, ball_dy;

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
            b = ball_get_box();
            ball_dx = round_fix(ball_get_dx()), ball_dy = round_fix(ball_get_dy());
            d_y = (ball_dy > 0) ? 5 : (r.y - b->y) / 4;
            if (b->x + b->width / 2 + ball_dx * d_y > r.x + r.width - dx && r.x < LCD_WIDTH - r.width - dx) {
                x += dx;
            } else if (b->x + b->width + ball_dx * d_y < r.x + dx && r.x > dx) {
                x -= dx;
            }
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
