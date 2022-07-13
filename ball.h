typedef int fix;                          // 固定小数点数 (整数部は24ビット、小数部は8ビット)
extern fix ball_get_dy(void);             // ボールのy方向の速度を返す．
extern void ball_set_dy(fix new_dy);      // ボールのy方向の速度をセットする．
extern fix ball_get_dx(void);             // ボールのx方向の速度を返す．
extern void ball_set_dx(fix new_dx);      // ボールのx方向の速度をセットする．
extern struct box *ball_get_box(void);    // ボールの箱の位置を返す．
extern void ball_step(void);              // アニメーションの1ステップを行なう．
extern int round_fix(fix f);              // 固定小数点数を整数に変換する．
