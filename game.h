enum state {START, RUNNING, DEAD, RESTART, CLEAR, HOME, STOP, SETTING, SCORE};
enum song_difficulty {EASY, NORMAL, HARD};
extern int get_optidx();
extern void game_step(void);             // 1ティックの動作を行なう．
extern enum state game_get_state(void);  // 今の状態を問い合わせる．
extern void game_set_state(enum state);  // 状態を変更する．
extern void game_set_difficulty(enum state); //難易度を変更する．
extern enum state game_get_difficulty(void); //難易度を問い合わせる．
extern void auto_play_toggle(void); // 自動プレイを切り替える．
extern int get_autoplay(void); // 自動プレイフラグを問い合わせる．
