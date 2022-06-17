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

int main(void)
{
    // 画面を初期化
    gba_register(LCD_CTRL) = LCD_BG2EN | LCD_MODE3;
    // タイマーを初期化
    gba_register(TMR_COUNT0) = 0;
    gba_register(TMR_CTRL0) = TMR_ENABLE + TMR_1024CLOCK;

    while (1) {
        draw_step();
        ball_step();
        racket_step();
        game_step();
        block_step();
        delay(INTERVAL);
    }
}
