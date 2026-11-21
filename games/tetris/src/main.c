#include <stdbool.h>
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/clock.h>

#define COLS      10
#define ROWS      20
#define CELL_SIZE 10

/* Center the 100x200 playfield on the 396x224 CG50 display */
#define BOARD_X   ((DWIDTH  - (COLS * CELL_SIZE)) / 2)
#define BOARD_Y   ((DHEIGHT - (ROWS * CELL_SIZE)) / 2)

static void draw_cell(int col, int row, int color)
{
    int x = BOARD_X + col * CELL_SIZE;
    int y = BOARD_Y + row * CELL_SIZE;
    drect_border(x, y, x + CELL_SIZE - 1, y + CELL_SIZE - 1, color, 1, C_BLACK);
}

int main(void)
{
    int px = COLS / 2;
    int py = 0;
    int drop_ticks = 0;
    bool running = true;

    while (running) {
        /* 1. Non-blocking input polling */
        key_event_t ev;
        while ((ev = pollevent()).type != KEYEV_NONE) {
            if (ev.type != KEYEV_DOWN) continue;

            if (ev.key == KEY_EXIT)  running = false;
            if (ev.key == KEY_LEFT  && px > 0)        px--;
            if (ev.key == KEY_RIGHT && px < COLS - 1) px++;
            if (ev.key == KEY_DOWN  && py < ROWS - 1) py++;
        }

        /* 2. Gravity (~500ms per row at ~60 FPS) */
        if (++drop_ticks >= 30) {
            drop_ticks = 0;
            py = (py + 1) % ROWS;
        }

        /* 3. Render frame */
        dclear(C_BLACK);

        /* Playfield border */
        drect_border(
            BOARD_X - 2, BOARD_Y - 2,
            BOARD_X + (COLS * CELL_SIZE) + 1,
            BOARD_Y + (ROWS * CELL_SIZE) + 1,
            C_NONE, 2, C_WHITE
        );

        /* Active block */
        draw_cell(px, py, C_RGB(0, 28, 31));

        /* HUD text */
        dtext(16, 16, C_WHITE, "CG50 TETRIS");
        dtext(16, 36, C_RGB(20, 20, 20), "EXIT to quit");

        dupdate();
        sleep_ms(16);
    }

    return 0;
}
