enum state {START, RUNNING, DEAD, RESTART, CLEAR, HOME, STOP, MODSELECT, SETTING, SDSELECT, SCORE};

extern void game_step(void);             // 1ティックの動作を行なう．
extern enum state game_get_state(void);  // 今の状態を問い合わせる．
extern void game_set_state(enum state);  // 状態を変更する．
