extern int ball_get_dy(void);             // ボールのy方向の速度を返す．
extern void ball_set_dy(int new_dy);      // ボールのy方向の速度をセットする．
extern int ball_get_dx(void);             // ボールのx方向の速度を返す．
extern void ball_set_dx(int new_dx);      // ボールのx方向の速度をセットする．
extern struct box *ball_get_box(void);    // ボールの箱の位置を返す．
extern void ball_step(void);              // アニメーションの1ステップを行なう．
