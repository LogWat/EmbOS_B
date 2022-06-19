extern int strlen(const char *);
extern void draw_char(hword *ptr, hword color, int code);
extern void draw_string(hword *ptr, hword color, char *str, int x, int y);
extern void draw_number(hword *ptr, hword color, int num, int x, int y);
extern void opt_slct(int num_of_options, char *options[], int selected_index);
extern void reset_field(void);
extern void draw_step();
