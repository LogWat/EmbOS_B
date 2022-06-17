#include "gba.h"
#include "ball.h"
#include "game.h"
#include "box.h"
#include "block.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_BLACK     BGR(0, 0, 0)
#define BLOCK_COLS      10
#define BLOCK_ROWS      2
#define BLOCK_WIDTH     (LCD_WIDTH / BLOCK_COLS)
#define BLOCK_HEIGHT    10
#define BLOCK_TOP      0

static struct box boxes[BLOCK_ROWS][BLOCK_COLS];
static char blocks[BLOCK_ROWS][BLOCK_COLS];
int num_blocks;

static int hit(int x, int y) {
    int i = x / BLOCK_WIDTH;
    int j = (y - BLOCK_TOP) / BLOCK_HEIGHT;
    if (i < 0 || i >= BLOCK_COLS || j < 0 || j >= BLOCK_ROWS) {
        return 0;
    }
    return blocks[j][i] == '1' ? 1 : 0;
}

static void delete(int x, int y) {
    int i = x / BLOCK_WIDTH;
    int j = (y - BLOCK_TOP) / BLOCK_HEIGHT;
    if (i < 0 || i >= BLOCK_COLS || j < 0 || j >= BLOCK_ROWS) {
        return;
    }
    if (blocks[j][i] == '0') {
        return;
    }
    blocks[j][i] = '0';
    num_blocks--;
    draw_box(&boxes[j][i], boxes[j][i].x, boxes[j][i].y, COLOR_BLACK);
}

void block_step(void)
{
    int i, j;
    int updown, leftright;
    struct box *ball;
    int ball_dx, ball_dy;
    int df = 0;
    switch (game_get_state()) {
    case START:
        for (i = 0; i < BLOCK_ROWS; i++) {
            for (j = 0; j < BLOCK_COLS; j++) {
                blocks[i][j] = '1';
                boxes[i][j].x = j * BLOCK_WIDTH;
                boxes[i][j].y = i * BLOCK_HEIGHT + BLOCK_TOP;
                boxes[i][j].width = BLOCK_WIDTH;
                boxes[i][j].height = BLOCK_HEIGHT;
            }
        }
        num_blocks = BLOCK_ROWS * BLOCK_COLS;
        df = 0;
        break;
    case RUNNING:
        if (df == 0) {
            for (i = 0; i < BLOCK_ROWS; i++) {
                for (j = 0; j < BLOCK_COLS; j++) {
                    if (blocks[i][j] == '1') {
                        draw_box(&boxes[i][j], boxes[i][j].x, boxes[i][j].y, COLOR_WHITE);
                    }
                }
            }
            df = 1;
        }
        updown = leftright = 0;
        ball = ball_get_box();
        if (hit(ball->x, ball->y)) {
            updown++;
            leftright++;
        }
        if (hit(ball->x, ball->y + ball->height)) {
            updown--;
            leftright++;
        }
        if (hit(ball->x + ball->width, ball->y)) {
            updown++;
            leftright--;
        }
        if (hit(ball->x + ball->width, ball->y + ball->height)) {
            updown--;
            leftright--;
        }
        delete(ball->x, ball->y);
        delete(ball->x + ball->width, ball->y);
        delete(ball->x, ball->y + ball->height);
        delete(ball->x + ball->width, ball->y + ball->height);

        if (updown) {
            ball_dy = ball_get_dy();
            if ((ball_dy < 0 && updown > 0) || (ball_dy > 0 && updown < 0)) {
                ball_set_dy(ball_dy * -1);
            }
        }
        if (leftright) {
            ball_dx = ball_get_dx();
            if ((ball_dx < 0 && leftright > 0) || (ball_dx > 0 && leftright < 0)) {
                ball_set_dx(ball_dx * -1);
            }
        }
        if (!num_blocks) {
            game_set_state(CLEAR);
        }
        break;
    case DEAD:
        break;
    case RESTART:
        for (i = 0; i < BLOCK_ROWS; i++) {
            for (j = 0; j < BLOCK_COLS; j++) {
                blocks[i][j] = '1';
                boxes[i][j].x = j * BLOCK_WIDTH;
                boxes[i][j].y = i * BLOCK_HEIGHT + BLOCK_TOP;
                boxes[i][j].width = BLOCK_WIDTH;
                boxes[i][j].height = BLOCK_HEIGHT;
            }
        }
        num_blocks = BLOCK_ROWS * BLOCK_COLS;
        df = 0;
    case CLEAR:
        if (df == 1) {
            for (i = 0; i < BLOCK_ROWS; i++) {
                for (j = 0; j < BLOCK_COLS; j++) {
                    draw_box(&boxes[i][j], boxes[i][j].x, boxes[i][j].y, COLOR_BLACK);
                }
            }
            df = 0;
        }
        break;
    }
}
