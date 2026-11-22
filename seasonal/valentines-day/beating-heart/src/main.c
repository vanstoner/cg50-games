#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/gint.h>

/* Your design! # = colour in, . = leave empty */
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

int main(void)
{
    gint_setrestart(1);
    int size = 16;
    int colour = C_RED;

    while (1) {
        dclear(C_WHITE);
        int left = (DWIDTH - 9 * size) / 2;
        int top = (DHEIGHT - 8 * size) / 2;
        for (int row = 0; row < 8; row++)
            for (int col = 0; col < 9; col++)
                if (HEART[row][col] == '#') {
                    int x = left + col * size;
                    int y = top + row * size;
                    drect(x, y, x + size - 2,
                          y + size - 2, colour);
                }
        dupdate();

        int key = getkey().key;
        if (key == KEY_EXIT) break;
        if (key == KEY_UP && size < 24)  size = size + 2;
        if (key == KEY_DOWN && size > 4) size = size - 2;
        if (key == KEY_F1) colour = C_RED;
        if (key == KEY_F2) colour = C_RGB(31, 10, 20);
    }
    return 0;
}
