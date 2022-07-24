#include "gba.h"
#include "ball.h"
#include "racket.h"
#include "box.h"
#include "game.h"
#include "block.h"
#include "draw.h"

#define INTERVAL 164 * 2

void delay(hword val) {
    val += gba_register(TMR_COUNT0);
    while (val != gba_register(TMR_COUNT0));
}

void wait_until_vblank(void) {
    while ((gba_register(LCD_STATUS) & 1) == 0);
}

void wait_until_vblank_end(void) {
    while ((gba_register(LCD_STATUS) & 1));
}

int main(void)
{
    // 画面を初期化
    gba_register(LCD_CTRL) = LCD_BG2EN | LCD_MODE3;
    // タイマーを初期化
    gba_register(TMR_COUNT0) = 111;
    gba_register(TMR_CTRL0) = TMR_ENABLE + TMR_1024CLOCK;

    while (1) {
        wait_until_vblank();
        game_step();
        draw_step(); // 画面更新担当だから ball, racket, block よりも先に呼び出し
        ball_step();
        racket_step();
        block_step();
        wait_until_vblank_end();
    }
}
