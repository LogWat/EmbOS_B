enum state {START, RUNNING, DEAD, RESTART, CLEAR, HOME, STOP, SETTING, SCORE};
enum song_difficulty {EASY, NORMAL, HARD};
extern void game_step(void);             // 1ティックの動作を行なう．
extern enum state game_get_state(void);  // 今の状態を問い合わせる．
extern enum state game_get_prev_state(void); // 前の状態を問い合わせる．
extern void game_set_state(enum state);  // 状態を変更する．
extern void game_set_prev_state(enum state); // 前の状態を変更する．
extern void game_set_difficulty(enum song_difficulty); //難易度を変更する．
extern void game_set_prev_difficulty(enum song_difficulty); //前の難易度を変更する．
extern enum song_difficulty game_get_difficulty(void); //難易度を問い合わせる．
extern enum song_difficulty game_get_prev_difficulty(void); //前の難易度を問い合わせる．
extern void auto_play_toggle(void); // 自動プレイを切り替える．
extern void prev_auto_play_toggle(void); // 前の自動プレイを切り替える．
extern int get_autoplay(void); // 自動プレイフラグを問い合わせる．
extern int get_prev_autoplay(void); // 前の自動プレイフラグを問い合わせる．
extern int get_optidx(void); // オプションインデックスを問い合わせる．
extern int get_prev_optidx(void); // 前回のオプションインデックスを問い合わせる．
extern void set_prev_optidx(int); // 前回のオプションインデックスを変更する．
