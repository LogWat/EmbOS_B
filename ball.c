#include "gba.h"
#include "box.h"
#include "game.h"
#include "ball.h"
#include "draw.h"
#include "block.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_BLACK     BGR(0, 0, 0)

static fix dx = (2 << 8), dy = (2 << 8);        /* ボールの現在の速度 */
static struct box b = {0, 65, 10, 10};          /* ボールの箱の現在の位置 */

fix ball_get_dy(void) { return dy; }
void ball_set_dy(fix new_dy) { dy = new_dy; }
fix ball_get_dx(void) { return dx; }
void ball_set_dx(fix new_dx) { dx = new_dx; }
struct box *ball_get_box(void) { return &b; }

int round_fix(fix f) {
    return (f + 0x80) >> 8;
}

void ball_step(void)
{
    int x = b.x, y = b.y;
    int dx_int, dy_int;

    switch (game_get_state()) {
    case START:
        move_box(&b, x, y, COLOR_BLACK);
        x = 0; y = 65;
        dx = (2 << 8); dy = (2 << 8);
        move_box(&b, x, y, COLOR_WHITE);
        break;
    case RUNNING:
        dx_int = round_fix(dx), dy_int = round_fix(dy);
        x = b.x + dx_int; y = b.y + dy_int;
        if (x < 0) {
            x = 0;
            dx = -dx;
        } else if (x > LCD_WIDTH - b.width) {
            x = LCD_WIDTH - b.width;
            dx = -dx;
        }
        if (y < 0) {
            y = 0;
            dy = -dy;
        } else if (y > LCD_HEIGHT - b.height) {
            y = LCD_HEIGHT - b.height;
            dy = -dy;
            screen_changed_flag_set();
            game_set_state(DEAD);
        }
        move_box(&b, x, y, COLOR_WHITE);
        break;
    case DEAD:
        break;
    case RESTART:
        move_box(&b, x, y, COLOR_BLACK);
        x = 0; y = 65;
        dx = (2 << 8); dy = (2 << 8);
        move_box(&b, x, y, COLOR_WHITE);
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
