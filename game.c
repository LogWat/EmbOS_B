#include "gba.h"
#include "draw.h"
#include "game.h"

#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_GRAY      BGR(15, 15, 15)
#define COLOR_BLACK     0

static enum state current_state = START;  // 現在の状態

static int option_index = 0;

static int options_slct(
    int num_of_options,
    int key,
    char *options[],
) {
    if (!(key & KEY_DOWN)) {
        option_index = (option_index - 1) % num_of_options;
    }
    if (!(key & KEY_UP)) {
        option_index = (option_index + 1) % num_of_options;
    }

    // draw options
    opt_slct(num_of_options, options, option_index);

    return option_index;
}

enum state game_get_state(void) { return current_state; }
void game_set_state(enum state new_state) {
    current_state = new_state;
}

void game_step(void)
{
    int key = gba_register(KEY_STATUS);
    switch (game_get_state()) {
    case START:
        if (!(key & KEY_START)) {
            game_set_state(HOME);
        }
        break;
    case RUNNING:
        // cheat code
        if (!(key & KEY_SELECT)) {
            game_set_state(STOP);
        }
        break;
    case STOP:
        break;
    case DEAD:
        if (!(key & KEY_START)) {
            game_set_state(RESTART);
        }
        break;
    case RESTART:
        /* 次のティックはRUNNING状態にする．*/
        game_set_state(RUNNING);
        break;
    case CLEAR:
        if (!(key & KEY_START)) {
            game_set_state(RESTART);
        }
        break;
    case HOME:
        break;
    case STOP:
        break;
    case MODSELCT:
        break;
    case SETTING:
        break;
    case SDSELECT:
        break;
    case SCORE:
        break;
    }

}
