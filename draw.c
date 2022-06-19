#include "gba.h"
#include "8x8.til"
#include "game.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_GRAY      BGR(15, 15, 15)
#define COLOR_BLACK     0
#define FONT_SIZE       8
#define SMALL_FONT_SIZE 4

static int draw_flag = 0;
static int end_flag = 0;

int strlen(const char *str) {
    int i = 0;
    while (str[i] != '\0') {
        i++;
    }
    return i;
}

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

void opt_slct(
    int num_of_options,
    char *options[],
    int selected_index,
) {
    hword *fb = (hword *)VRAM;
    int i;
    for (i = 0; i < num_of_options; i++) {
        if (i == selected_index) {
            draw_string(fb, COLOR_WHITE, options[i], LCD_WIDTH / 2 - strlen(options[i]) * FONT_SIZE / 2, LCD_HEIGHT / 2 + i * FONT_SIZE);
        } else {
            draw_string(fb, COLOR_GRAY, options[i], LCD_WIDTH / 2 - strlen(options[i]) * FONT_SIZE / 2, LCD_HEIGHT / 2 + i * FONT_SIZE);
        }
    }
}

void reset_field(void)
{
    hword *fb = (hword *)VRAM;
    int i, j;
    for (i = 0; i < LCD_HEIGHT; i++) {
        for (j = 0; j < LCD_WIDTH; j++) {
            *(fb + i * LCD_WIDTH + j) = COLOR_BLACK;
        }
    }
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
    case HOME:
        break;
    case STOP:
        break;
    case MODSELCT:
        break;
    case SETTING:
        break;
    case SDSELECT:
        break;
    case SCORE:
        break;
    }
}
