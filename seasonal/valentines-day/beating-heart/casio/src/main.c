#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/timer.h>
#include <gint/cpu.h>
#include <gint/gint.h>

/* Your designs! # = colour in, . = leave empty */
const char *HEART[] = {
    ".##...##.",
    "####.####",
    "#########",
    "#########",
    ".#######.",
    "..#####..",
    "...###...",
    "....#....",
};
const char *TIP[] = {
    "#..", "##.", "###", "##.", "#..",
};
const char *TAIL[] = {
    "##..", ".##.", "..##", ".##.", "##..",
};

/* Draw a design with its top-left corner at x, y */
void draw(const char *design[], int rows, int cols,
          int x, int y, int size, int colour)
{
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            if (design[r][c] == '#') {
                int left = x + c * size;
                int top = y + r * size;
                drect(left, top, left + size - 1,
                      top + size - 1, colour);
            }
}

/* Draw the heart in the middle of the screen */
void heart(int size, int colour)
{
    draw(HEART, 8, 9, DWIDTH / 2 - 9 * size / 2,
         DHEIGHT / 2 - 4 * size, size, colour);
}

int main(void)
{
    gint_setrestart(1);
    int mid = DWIDTH / 2, y = DHEIGHT / 2;
    int gold = C_RGB(28, 18, 2);
    int colours[3] = {C_RED, C_RGB(31, 14, 22), C_WHITE};
    int arrow = -10;   /* where the arrow's tip is */
    int hit = -1;      /* when it hit (-1 = not yet) */
    int t = 0;         /* counts up 30 times a second */

    volatile int tick = 0;
    int timer = timer_configure(TIMER_ANY, 33333,
                                GINT_CALL_SET(&tick));
    timer_start(timer);

    while (1) {
        while (!tick) sleep();
        tick = 0;
        dclear(C_WHITE);

        /* Back of the arrow, behind the hearts */
        drect(arrow - 290, y - 1, arrow, y + 2, gold);
        draw(TAIL, 5, 4, arrow - 300, y - 9, 4, gold);

        /* Hearts inside hearts, biggest first */
        int wave = 0;
        if (hit >= 0) wave = (t - hit) / 3;
        for (int k = 0; k < 7; k++)
            heart(24 - 3 * k, colours[(k + wave) % 3]);

        /* The little heart in the middle beats */
        if (hit >= 0 && t % 20 < 4) heart(4, C_RED);
        else heart(3, C_RED);

        /* Front of the arrow, over the hearts */
        if (arrow > mid)
            drect(mid, y - 1, arrow, y + 2, gold);
        draw(TIP, 5, 3, arrow, y - 9, 4, gold);
        dupdate();

        if (arrow < mid + 150) arrow = arrow + 8;
        if (hit < 0 && arrow > mid) hit = t;
        t = t + 1;

        clearevents();
        if (keydown(KEY_EXIT)) break;
        if (keydown(KEY_EXE)) { arrow = -10; hit = -1; }
    }
    return 0;
}
