#include "gba.h"
#include "box.h"
#include "ball.h"
#include "game.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_BLACK     BGR(0, 0, 0)

static int dx = 4, dy = 4;            /* ボールの現在の速度 */
static struct box b = {0, 35, 10, 10};          /* ボールの箱の現在の位置 */

int ball_get_dy(void) { return dy; }
void ball_set_dy(int new_dy) { dy = new_dy; }
int ball_get_dx(void) { return dx; }
void ball_set_dx(int new_dx) { dx = new_dx; }
struct box *ball_get_box(void) { return &b; }

void ball_step(void)
{
    int x = b.x, y = b.y;

    switch (game_get_state()) {
    case START:
        break;
    case RUNNING:
        x = b.x + dx;
        y = b.y + dy;
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
            game_set_state(DEAD);
        }
        move_box(&b, x, y, COLOR_WHITE);
        break;
    case DEAD:
        break;
    case RESTART:
        move_box(&b, x, y, COLOR_BLACK);
        x = 0; y = 35;
        dx = 4; dy = 4;
        move_box(&b, x, y, COLOR_WHITE);
        break;
    case CLEAR:
        draw_box(&b, x, y, COLOR_BLACK);
        break;    
    }
}
