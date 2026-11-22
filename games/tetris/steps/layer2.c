/* Tetris - end of Layer 2: the board
   A 10x20 grid in the middle of the screen, drawn from an array. */

#include <stdbool.h>
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/timer.h>
#include <gint/cpu.h>

#define FPS      60

#define COLS     10
#define ROWS     20
#define CELL     10

#define BOARD_X  ((DWIDTH  - COLS * CELL) / 2)
#define BOARD_Y  ((DHEIGHT - ROWS * CELL) / 2)

/* 0 = empty, anything else = a block */
static int board[ROWS][COLS];

static void draw_block(int px, int py, int color)
{
    drect(px, py, px + CELL - 2, py + CELL - 2, color);
}

static void draw_cell(int col, int row, int color)
{
    draw_block(BOARD_X + col * CELL, BOARD_Y + row * CELL, color);
}

static void draw(void)
{
    dclear(C_BLACK);

    drect_border(BOARD_X - 2, BOARD_Y - 2,
        BOARD_X + COLS * CELL + 1, BOARD_Y + ROWS * CELL + 1,
        C_RGB(2, 2, 4), 2, C_WHITE);

    for (int y = 0; y < ROWS; y++)
        for (int x = 0; x < COLS; x++)
            if (board[y][x] != 0)
                draw_cell(x, y, C_WHITE);

    dtext(16, 14, C_WHITE, "TETRIS");
    dupdate();
}

int main(void)
{
    /* Some test blocks, so we can see the drawing works */
    board[19][0] = 1;       /* bottom-left corner */
    board[19][1] = 1;
    board[18][0] = 1;
    board[19][9] = 1;       /* bottom-right corner */
    board[0][4]  = 1;       /* top row */

    volatile int tick = 0;
    int timer = timer_configure(TIMER_ANY, 1000000 / FPS, GINT_CALL_SET(&tick));
    if (timer < 0)
        return 1;
    timer_start(timer);

    while (true) {
        while (!tick)
            sleep();
        tick = 0;

        cleareventflips();
        clearevents();

        if (keypressed(KEY_EXIT))
            break;

        draw();
    }

    timer_stop(timer);
    return 0;
}
