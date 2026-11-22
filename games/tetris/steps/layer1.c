/* Tetris - end of Layer 1: the game loop
   A screen that redraws 60 times a second and quits on EXIT. */

#include <stdbool.h>
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/timer.h>
#include <gint/cpu.h>
#include <gint/gint.h>

#define FPS 60

static int frames = 0;

static void draw(void)
{
    dclear(C_BLACK);
    dtext(16, 14, C_WHITE, "TETRIS");
    dprint(16, 40, C_LIGHT, "Frames: %d", frames);
    dprint(16, 56, C_LIGHT, "Seconds: %d", frames / FPS);
    dtext(16, 200, C_DARK, "EXIT to quit");
    dupdate();
}

int main(void)
{
    gint_setrestart(1);

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

        frames++;
        draw();
    }

    timer_stop(timer);
    return 0;
}
