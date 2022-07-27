extern int strlen(const char *);
extern void draw_char(hword *ptr, hword color, int code);
extern void draw_string(hword *ptr, hword color, char *str, int x, int y);
extern void draw_number(hword *ptr, hword color, int num, int x, int y);
extern void draw_number_as_bin(hword *ptr, hword color, int num, int x, int y);
extern void reset_screen(void);
extern void draw_step();
extern void screen_changed_flag_set();   // 画面に変更があったことを示すフラグをセット
extern void screen_changed_flag_reset(); // 上記フラグをリセット
