struct box {
    int x, y;
    int width, height;
};
extern void draw_box(struct box *b, int x, int y, hword color);
extern void move_box(struct box *b, int x, int y, hword color);
extern int cross(struct box *b1, struct box *b2);
extern int side(struct box *b1, struct box *b2);
