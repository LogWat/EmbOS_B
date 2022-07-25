#include "gba.h"
#include "ball.h"
#include "game.h"
#include "box.h"
#include "block.h"
#include "draw.h"
#include "ball.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_BLACK     BGR(0, 0, 0)
#define COLOR_RED       BGR(31, 0, 0)
#define COLOR_GREEN     BGR(0, 31, 5)
#define COLOR_BLUE      BGR(0, 0, 31)
#define COLOR_LIGHT_YELLOW BGR(31, 31, 10)
#define COLOR_YELLOW    BGR(31, 31, 0)
#define COLOR_DARK_YELLOW    BGR(25, 25, 0)
#define COLOR_CYAN      BGR(0, 31, 31)
#define COLOR_PURPLE    BGR(31, 0, 29)
#define BLOCK_COLS      10
#define BLOCK_ROWS      3
#define BLOCK_WIDTH     (LCD_WIDTH / BLOCK_COLS)
#define BLOCK_HEIGHT    10
#define BLOCK_TOP      20

typedef struct {
    int x;
    int y;
    int color;
    int flag;
} RedrawBlock;

RedrawBlock redraw = {0, 0, COLOR_BLACK, 0};

static enum btype block_type[BLOCK_COLS][BLOCK_ROWS]; // ブロックタイプ
static struct box boxes[BLOCK_ROWS][BLOCK_COLS];      // ブロックの位置
static char blocks[BLOCK_ROWS][BLOCK_COLS];           // ブロックの表示フラグ
int num_blocks;
int df = 0;

static int pos_by_blocks = LCD_HEIGHT - 30;         // ラケット位置
void pos_change(int diff) { pos_by_blocks = diff; }
int get_pos_by_blocks(void) { return pos_by_blocks; }
static int twice_blocks[BLOCK_ROWS][BLOCK_COLS]; // 硬度２倍フラグ
static int twice_blocks_protect[BLOCK_ROWS][BLOCK_COLS]; // 一度のループで2度当たり判定にならないように
static int width_by_block;                       // ラケット幅(ブロックによる変更)
void width_change(int width) { width_by_block = width; }
int get_width_by_blocks(void) { return width_by_block; }
static int speed_by_block;                       // ゲームスピード(ボール&ラケット) 加算値
void speed_change(int speed) { speed_by_block = speed; }
int get_speed_by_blocks(void) { return speed_by_block; }
static int reverse_flag = 0;                     // 操作反転フラグ
void reverse_toggle(void) { reverse_flag = !reverse_flag; }
int get_reverse_flag(void) { return reverse_flag; }

// あたっても一度で消さない場合ブロックがボールによって欠けてしまうのを防ぐための関数
static void redraw_box() {
    int x = redraw.x;
    int y = redraw.y;
    int color = redraw.color;
    draw_box(&boxes[y][x], boxes[y][x].x, boxes[y][x].y, color);
    redraw.flag = 0;
}

void all_flag_reset(void) {
    int i, j;
    for (i = 0; i < BLOCK_ROWS; i++) {
        for (j = 0; j < BLOCK_COLS; j++) {
            twice_blocks[i][j] = 0;
        }
    }
    width_change(30);
    speed_change(0);
    pos_change(LCD_HEIGHT - 30);
    reverse_flag = 0;
}

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
    enum difficulty df = game_get_difficulty();
    if (i < 0 || i >= BLOCK_COLS || j < 0 || j >= BLOCK_ROWS) {
        return;
    }
    if (blocks[j][i] == '0') {
        return;
    }
    if (twice_blocks[j][i] == 1) {
        twice_blocks[j][i] = 0;
        twice_blocks_protect[j][i] = 1;
        // ブロック表示の修復を次ループで依頼
        redraw.x = i;
        redraw.y = j;
        redraw.color = COLOR_DARK_YELLOW;
        redraw.flag = 1;
        return;
    }
    if (twice_blocks_protect[j][i] == 1) {
        return;
    }

    if (block_type[j][i] == WIDTH) {
        int next_racket_width = 30;
        switch (df) {
            case EASY:
                // EASYにはWIDTHブロックはない
                break;
            case NORMAL:
                // 20 ~ 50
                next_racket_width = 30 + (getrand() % 2 ? 2 : -1) * (getrand() % 10 + 1);
                break;
            case HARD:
                // 10 ~ 50
                next_racket_width = 30 + (getrand() % 2 ? 2 : -2) * (getrand() % 10 + 1);
                break;
            case INSANE:
                // 10 ~ 29
                next_racket_width = 30 + (getrand() % 2 ? -2 : -1) * (getrand() % 10 + 1);
                break;
        }
        width_change(next_racket_width);
    } else if (block_type[j][i] == SPEED) {
        int next_speed = 2 + (df == INSANE); // INSANE時のみ更に加速
        speed_change(next_speed);
        fix ball_dx = ball_get_dx(), ball_dy = ball_get_dy();
        ball_set_dx(ball_dx + (next_speed << 6) * (ball_dx > 0 ? 1 : -1));
        ball_set_dy(ball_dy + (next_speed << 6) * (ball_dy > 0 ? 1 : -1));
    } else if (block_type[j][i] == REVERSE) {
        reverse_toggle();
    } else if (block_type[j][i] == POS) {
        pos_change(LCD_HEIGHT - 30 + (getrand() % 2 ? 2 : -2) * (getrand() % 10 + 1));
    }
    blocks[j][i] = '0';
    num_blocks--;
    draw_box(&boxes[j][i], boxes[j][i].x, boxes[j][i].y, COLOR_BLACK);
}

int twice_num, width_num, speed_num, reverse_num, pos_num;

static void block_init() {
    int i, j;
    int rand_num;
    enum difficulty df = game_get_difficulty();
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
    all_flag_reset();

    // block state define
    int twice_row, width_row, speed_row, reverse_row, pos_row;
    twice_num = 5 + 3 * (df == INSANE);
    width_num = 3;
    speed_num = 2 + (df == INSANE);
    reverse_num = 3;
    pos_num = 3;
    for (i = 0; i < BLOCK_ROWS; i++) {
        twice_row = width_row = speed_row = reverse_row = pos_row = 1;
        for (j = 0; j < BLOCK_COLS; j++) {
            if (j < 1) {
                block_type[i][j] = DEFAULT;
                continue;
            }
            rand_num = getrand();
            switch (df) {
                // EASY: 特殊ブロック 硬度2倍, ラケット幅変更
                case EASY:
                    if (rand_num > 7 + 2 * (i && (twice_num || width_num))) {
                        block_type[i][j] = DEFAULT;
                    } else if (rand_num > 3 + 2 * (i && width_num)) {
                        if (twice_num && twice_row) {
                            block_type[i][j] = TWICE;
                            twice_num--;
                            twice_row = 0;
                        } else {
                            block_type[i][j] = DEFAULT;
                        }
                    } else {
                        if (width_num && width_row) {
                            block_type[i][j] = WIDTH;
                            width_num--;
                            width_row = 0;
                        } else {
                            block_type[i][j] = DEFAULT;
                        }
                    }
                    break;
                // NORMAL: 特殊ブロック 硬度2倍, ラケット幅変更, 速度変更
                case NORMAL:
                    if (rand_num > 7 + 2 * (i && (twice_num || width_num || speed_num))) {
                        block_type[i][j] = DEFAULT;
                    } else if (rand_num > 4 + 1 * (i && (twice_num || width_num || speed_num))) {
                        if (twice_num && twice_row) {
                            block_type[i][j] = TWICE;
                            twice_num--;
                            twice_row = 0;
                        } else {
                            block_type[i][j] = DEFAULT;
                        }
                    } else {
                        if (rand_num > 1 + 1 * (i && width_num)) {
                            if (speed_num && speed_row) {
                                block_type[i][j] = SPEED;
                                speed_num--;
                                speed_row = 0;
                            } else {
                                block_type[i][j] = DEFAULT;
                            }
                        } else {
                            if (width_num && width_row) {
                                block_type[i][j] = WIDTH;
                                width_num--;
                                width_row = 0;
                            } else {
                                block_type[i][j] = DEFAULT;
                            }
                        }  
                    }
                    break;
                // HARD: 特殊ブロック 硬度2倍, ラケット幅変更, 速度変更, 操作反転
                case HARD:
                    if (rand_num > 8 + (i && (twice_num || width_num || speed_num || reverse_num))) {
                        block_type[i][j] = DEFAULT;
                    } else if (rand_num > 4 + (i && (width_num || speed_num || reverse_num))) {
                        if (twice_num && twice_row) {
                            block_type[i][j] = TWICE;
                            twice_num--;
                            twice_row = 0;
                        } else {
                            block_type[i][j] = DEFAULT;
                        }
                    } else if (rand_num > 2 + (i && (speed_num || reverse_num))) {
                        if (speed_num && speed_row) {
                            block_type[i][j] = SPEED;
                            speed_num--;
                            speed_row = 0;
                        } else {
                            block_type[i][j] = DEFAULT;
                        }
                    } else {
                        if (rand_num > 0 + (i && reverse_num)) {
                            if (width_num && width_row) {
                                block_type[i][j] = WIDTH;
                                width_num--;
                                width_row = 0;
                            } else {
                                block_type[i][j] = DEFAULT;
                            }
                        } else {
                            if (reverse_num && reverse_row) {
                                block_type[i][j] = REVERSE;
                                reverse_num--;
                                reverse_row = 0;
                            } else {
                                block_type[i][j] = DEFAULT;
                            }
                        }
                    }
                    break;
                // INSANE: 特殊ブロック 全種類
                case INSANE:
                    if (rand_num > 9 + (i && (twice_num || width_num || speed_num || reverse_num || pos_num))) {
                        block_type[i][j] = DEFAULT;
                    } else if (rand_num > 5 + (i && (width_num || speed_num || reverse_num || pos_num))) {
                        if (twice_num && twice_row) {
                            block_type[i][j] = TWICE;
                            twice_num--;
                            twice_row = 0;
                        } else {
                            block_type[i][j] = DEFAULT;
                        }
                    } else if (rand_num > 3 + (i && (speed_num || reverse_num || pos_num))) {
                        if (speed_num && speed_row) {
                            block_type[i][j] = SPEED;
                            speed_num--;
                            speed_row = 0;
                        } else {
                            block_type[i][j] = DEFAULT;
                        }
                    } else {
                        if (rand_num > 1 + (i && (reverse_num || pos_num))) {
                            if (width_num && width_row) {
                                block_type[i][j] = WIDTH;
                                width_num--;
                                width_row = 0;
                            } else {
                                block_type[i][j] = DEFAULT;
                            }
                        } else if (rand_num > 0 + (i && reverse_num)) {
                            if (reverse_num && reverse_row) {
                                block_type[i][j] = REVERSE;
                                reverse_num--;
                                reverse_row = 0;
                            } else {
                                block_type[i][j] = DEFAULT;
                            }
                        } else {
                            if (pos_num && pos_row) {
                                block_type[i][j] = POS;
                                pos_num--;
                                pos_row = 0;
                            } else {
                                block_type[i][j] = DEFAULT;
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    twice_num = 5 + 3 * (df == INSANE);
    width_num = 3;
    speed_num = 2 + (df == INSANE);
    reverse_num = 3;
    pos_num = 3;

    // block draw
    for (i = 0; i < BLOCK_ROWS; i++) {
        for (j = 0; j < BLOCK_COLS; j++) {
            if (blocks[i][j] == '1') {
                switch (block_type[i][j]) {
                case DEFAULT:
                    draw_box(&boxes[i][j], boxes[i][j].x, boxes[i][j].y, COLOR_WHITE);
                    break;
                case TWICE:
                    draw_box(&boxes[i][j], boxes[i][j].x, boxes[i][j].y, COLOR_YELLOW);
                    twice_blocks[i][j] = 1;
                    break;
                case WIDTH:
                    draw_box(&boxes[i][j], boxes[i][j].x, boxes[i][j].y, COLOR_BLUE);
                    break;
                case REVERSE:
                    draw_box(&boxes[i][j], boxes[i][j].x, boxes[i][j].y, COLOR_PURPLE);
                    break;
                case SPEED:
                    draw_box(&boxes[i][j], boxes[i][j].x, boxes[i][j].y, COLOR_GREEN);
                    break;
                case POS:
                    draw_box(&boxes[i][j], boxes[i][j].x, boxes[i][j].y, COLOR_RED);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void block_step(void)
{
    int updown, leftright;
    struct box *ball;
    int ball_dx, ball_dy;
    int i, j;
    switch (game_get_state()) {
    case START:
        if (df == 0) {
            block_init();
            df = 1;
        }
        break;
    case RUNNING:
        if (df) {
            df = 0;
        }
        if (redraw.flag) {
            redraw_box();
        }
        for (i = 0; i < BLOCK_ROWS; i++) {
            for (j = 0; j < BLOCK_COLS; j++) {
                twice_blocks_protect[i][j] = 0;
            }
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
            ball_dy = round_fix(ball_get_dy());
            if ((ball_dy < 0 && updown > 0) || (ball_dy > 0 && updown < 0)) {
                ball_set_dy(-ball_get_dy());
            }
        }
        if (leftright) {
            ball_dx = round_fix(ball_get_dx());
            if ((ball_dx < 0 && leftright > 0) || (ball_dx > 0 && leftright < 0)) {
                ball_set_dx(-ball_get_dx());
            }
        }
        if (!num_blocks) {
            screen_changed_flag_set();
            game_set_state(CLEAR);
        }
        break;
    case DEAD:
        break;
    case RESTART:
        if (df == 0) {
            block_init();
            df = 1;
        }
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
