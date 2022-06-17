#include "gba.h"
#include "box.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_BLACK     0

void draw_box(struct box *b, int x, int y, hword color) {
    hword *base, *d;
    int w, h;

    base = (hword*)VRAM + (x + y * LCD_WIDTH);

    for (h = b->height; h > 0; h--) {
        d = base;
        for (w = b->width; w > 0; w--)
            *(d++) = color;
        base += LCD_WIDTH;
    }

    b->x = x;
    b->y = y;
}

void move_box(struct box *b, int x, int y, hword color) {
    draw_box(b, b->x, b->y, COLOR_BLACK);
    draw_box(b, x, y, color);
}

int cross(struct box *b1, struct box *b2) {
    return (b1->x <= b2->x + b2->width && b1->x + b1->width >= b2->x &&
            b1->y <= b2->y + b2->height && b1->y + b1->height >= b2->y);
}

// crossを前提とする 第１引数にracketを想定
int side(struct box *b1, struct box *b2) {
    return ((b1->x > b2->x && b1->y + b1->height < b2->y + b2->height) ||
            (b1->x + b1->width < b2->x + b2->width && b1->y + b1->height < b2->y + b2->height));
}
