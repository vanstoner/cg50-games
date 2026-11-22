/* Tetris - end of Layer 3: the pieces
   The seven shapes as data, and one piece drawn on the board. */

#include <stdbool.h>
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/timer.h>
#include <gint/cpu.h>
#include <gint/gint.h>

#define FPS      60

#define COLS     10
#define ROWS     20
#define CELL     10

#define BOARD_X  ((DWIDTH  - COLS * CELL) / 2)
#define BOARD_Y  ((DHEIGHT - ROWS * CELL) / 2)

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point cells[4];
    int size;
    int color;
} Shape;

static const Shape SHAPES[7] = {
    /* I */ { {{0,1}, {1,1}, {2,1}, {3,1}}, 4, C_RGB( 0, 28, 31) },
    /* O */ { {{0,0}, {1,0}, {0,1}, {1,1}}, 2, C_RGB(31, 28,  0) },
    /* T */ { {{1,0}, {0,1}, {1,1}, {2,1}}, 3, C_RGB(20,  6, 28) },
    /* S */ { {{1,0}, {2,0}, {0,1}, {1,1}}, 3, C_RGB( 4, 28,  4) },
    /* Z */ { {{0,0}, {1,0}, {1,1}, {2,1}}, 3, C_RGB(31,  4,  4) },
    /* J */ { {{0,0}, {0,1}, {1,1}, {2,1}}, 3, C_RGB( 6, 12, 31) },
    /* L */ { {{2,0}, {0,1}, {1,1}, {2,1}}, 3, C_RGB(31, 16,  0) },
};

typedef struct {
    int type;
    int rot;
    int x, y;
} Piece;

/* 0 = empty, otherwise (piece type + 1), so we know what colour to draw */
static int board[ROWS][COLS];

static Piece current;

static Point piece_cell(Piece const *p, int i)
{
    Shape const *s = &SHAPES[p->type];
    int x = s->cells[i].x;
    int y = s->cells[i].y;
    return (Point){ p->x + x, p->y + y };
}

static void draw_block(int px, int py, int color)
{
    drect(px, py, px + CELL - 2, py + CELL - 2, color);
}

static void draw_cell(int col, int row, int color)
{
    draw_block(BOARD_X + col * CELL, BOARD_Y + row * CELL, color);
}

static void draw_piece(Piece const *p)
{
    int color = SHAPES[p->type].color;
    for (int i = 0; i < 4; i++) {
        Point c = piece_cell(p, i);
        draw_cell(c.x, c.y, color);
    }
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
                draw_cell(x, y, SHAPES[board[y][x] - 1].color);

    draw_piece(&current);

    dtext(16, 14, C_WHITE, "TETRIS");
    dupdate();
}

int main(void)
{
    gint_setrestart(1);

    /* Test blocks, now in colour: the number is (piece type + 1) */
    board[19][0] = 1;       /* I colour */
    board[19][1] = 2;       /* O colour */
    board[18][0] = 3;       /* T colour */
    board[19][9] = 5;       /* Z colour */

    /* A T piece near the top. Try changing type to 0..6 */
    current = (Piece){ .type = 2, .rot = 0, .x = 3, .y = 0 };

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
