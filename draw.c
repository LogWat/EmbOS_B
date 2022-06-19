#include "gba.h"
#include "8x8.til"
#include "game.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_BLACK     0
#define FONT_SIZE       8

static int draw_flag = 0;
static int end_flag = 0;

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
    int i;
    for (i = 0, ptr += x + y * LCD_WIDTH; *str; str++, ptr += FONT_SIZE) {
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

void draw_step() {
    hword *fb = (hword *)VRAM;
    switch (game_get_state()) {
    case START:
        if (!draw_flag) {
            draw_string(fb, COLOR_WHITE, "Press START to start", LCD_WIDTH / 2 - FONT_SIZE * 10, LCD_HEIGHT / 2);
            draw_flag = 1;
        }
        break;
    case RUNNING:
        if (draw_flag) {
            draw_string(fb, COLOR_BLACK, "Press START to start", LCD_WIDTH / 2 - FONT_SIZE * 10, LCD_HEIGHT / 2);
            draw_flag = 0;
        }
        break;
    case DEAD:
        if (!end_flag) {
            draw_string(fb, COLOR_WHITE, "Game Over", LCD_WIDTH / 2 - FONT_SIZE * 5, LCD_HEIGHT / 2);
            draw_string(fb, COLOR_WHITE, "Press START to restart", LCD_WIDTH / 2 - FONT_SIZE * 11, LCD_HEIGHT / 2 + FONT_SIZE);
            end_flag = 1;
        }
        break;
    case CLEAR:
        if (!end_flag) {
            draw_string(fb, COLOR_WHITE, "Game Clear", LCD_WIDTH / 2 - FONT_SIZE * 5, LCD_HEIGHT / 2);
            draw_string(fb, COLOR_WHITE, "Press START to restart", LCD_WIDTH / 2 - FONT_SIZE * 11, LCD_HEIGHT / 2 + FONT_SIZE);
            end_flag = 2;
        }
        break;
    case RESTART:
        if (end_flag == 1) {
            draw_string(fb, COLOR_BLACK, "Game Over", LCD_WIDTH / 2 - FONT_SIZE * 5, LCD_HEIGHT / 2);
            draw_string(fb, COLOR_BLACK, "Press START to restart", LCD_WIDTH / 2 - FONT_SIZE * 11, LCD_HEIGHT / 2 + FONT_SIZE);
        } else if (end_flag == 2) {
            draw_string(fb, COLOR_BLACK, "Game Clear", LCD_WIDTH / 2 - FONT_SIZE * 5, LCD_HEIGHT / 2);
            draw_string(fb, COLOR_BLACK, "Press START to restart", LCD_WIDTH / 2 - FONT_SIZE * 11, LCD_HEIGHT / 2 + FONT_SIZE);
        }
        end_flag = 0;
        break;
    }
}
