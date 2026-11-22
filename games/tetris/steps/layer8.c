/* Tetris - end of Layer 8: scoring, levels, and fair pieces
   Score, lines and level on screen, a NEXT box, and the game speeds up. */

#include <stdbool.h>
#include <stdlib.h>
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/timer.h>
#include <gint/cpu.h>
#include <gint/gint.h>
#include <gint/rtc.h>

#define FPS      60

#define COLS     10
#define ROWS     20
#define CELL     10

#define BOARD_X  ((DWIDTH  - COLS * CELL) / 2)
#define BOARD_Y  ((DHEIGHT - ROWS * CELL) / 2)

#define LEFT_X   16
#define RIGHT_X  (BOARD_X + COLS * CELL + 16)

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

static int board[ROWS][COLS];

static Piece current;
static int fall_timer;
static int next_type;

static int score, lines, level;

static Point piece_cell(Piece const *p, int i)
{
    Shape const *s = &SHAPES[p->type];
    int x = s->cells[i].x;
    int y = s->cells[i].y;

    for (int r = 0; r < p->rot; r++) {
        int old_x = x;
        x = s->size - 1 - y;
        y = old_x;
    }
    return (Point){ p->x + x, p->y + y };
}

static bool fits(Piece const *p)
{
    for (int i = 0; i < 4; i++) {
        Point c = piece_cell(p, i);
        if (c.x < 0 || c.x >= COLS || c.y < 0 || c.y >= ROWS)
            return false;
        if (board[c.y][c.x] != 0)
            return false;
    }
    return true;
}

static bool try_move(int dx, int dy)
{
    Piece moved = current;
    moved.x += dx;
    moved.y += dy;
    if (!fits(&moved))
        return false;
    current = moved;
    return true;
}

static void try_rotate(int direction)
{
    static const Point kicks[] = {
        {0, 0}, {-1, 0}, {1, 0}, {0, -1}, {-2, 0}, {2, 0},
    };

    Piece turned = current;
    turned.rot = (turned.rot + direction + 4) % 4;

    for (unsigned i = 0; i < sizeof kicks / sizeof kicks[0]; i++) {
        Piece kicked = turned;
        kicked.x += kicks[i].x;
        kicked.y += kicks[i].y;
        if (fits(&kicked)) {
            current = kicked;
            return;
        }
    }
}

static int bag[7];
static int bag_left = 0;

static int take_from_bag(void)
{
    if (bag_left == 0) {
        for (int i = 0; i < 7; i++)
            bag[i] = i;
        for (int i = 6; i > 0; i--) {
            int j = rand() % (i + 1);
            int tmp = bag[i];
            bag[i] = bag[j];
            bag[j] = tmp;
        }
        bag_left = 7;
    }
    return bag[--bag_left];
}

static void clear_board(void)
{
    for (int y = 0; y < ROWS; y++)
        for (int x = 0; x < COLS; x++)
            board[y][x] = 0;
}

static bool spawn_piece(void)
{
    current.type = next_type;
    current.rot = 0;
    current.x = (COLS - SHAPES[current.type].size) / 2;
    current.y = 0;
    next_type = take_from_bag();
    fall_timer = 0;
    return fits(&current);
}

static int clear_lines(void)
{
    int cleared = 0;

    for (int y = ROWS - 1; y >= 0; y--) {
        bool full = true;
        for (int x = 0; x < COLS; x++)
            if (board[y][x] == 0)
                full = false;
        if (!full)
            continue;

        for (int yy = y; yy > 0; yy--)
            for (int x = 0; x < COLS; x++)
                board[yy][x] = board[yy - 1][x];
        for (int x = 0; x < COLS; x++)
            board[0][x] = 0;

        cleared++;
        y++;
    }
    return cleared;
}

static int frames_per_row(void)
{
    static const int speed[] = {
        48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2,
    };
    int n = sizeof speed / sizeof speed[0];
    return speed[level < n ? level : n - 1];
}

static void lock_piece(void)
{
    for (int i = 0; i < 4; i++) {
        Point c = piece_cell(&current, i);
        board[c.y][c.x] = current.type + 1;
    }

    static const int points[5] = { 0, 40, 100, 300, 1200 };
    int cleared = clear_lines();
    score += points[cleared] * (level + 1);
    lines += cleared;
    level = lines / 10;

    if (!spawn_piece()) {
        /* Still no game over screen (Layer 9): start afresh */
        clear_board();
        score = 0;
        lines = 0;
        level = 0;
        spawn_piece();
    }
}

#define REPEAT_DELAY 10
#define REPEAT_RATE   3

static bool key_repeat(int key, int *held_frames)
{
    if (!keydown(key)) {
        *held_frames = 0;
        return false;
    }
    (*held_frames)++;
    if (*held_frames == 1)
        return true;
    if (*held_frames >= REPEAT_DELAY
        && (*held_frames - REPEAT_DELAY) % REPEAT_RATE == 0)
        return true;
    return false;
}

static void update_playing(void)
{
    static int left_held, right_held;

    if (key_repeat(KEY_LEFT, &left_held))
        try_move(-1, 0);
    if (key_repeat(KEY_RIGHT, &right_held))
        try_move(1, 0);

    if (keypressed(KEY_UP) || keypressed(KEY_SHIFT))
        try_rotate(+1);
    if (keypressed(KEY_ALPHA))
        try_rotate(-1);

    int delay = frames_per_row();
    bool soft_drop = keydown(KEY_DOWN);
    if (soft_drop && delay > 2)
        delay = 2;

    if (++fall_timer >= delay) {
        fall_timer = 0;
        if (try_move(0, 1)) {
            if (soft_drop)
                score += 1;
        }
        else {
            lock_piece();
        }
    }
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

static void draw_stats(void)
{
    dtext(LEFT_X, 14, C_WHITE, "TETRIS");

    dtext(LEFT_X, 44, C_LIGHT, "SCORE");
    dprint(LEFT_X, 56, C_WHITE, "%d", score);

    dtext(LEFT_X, 80, C_LIGHT, "LINES");
    dprint(LEFT_X, 92, C_WHITE, "%d", lines);

    dtext(LEFT_X, 116, C_LIGHT, "LEVEL");
    dprint(LEFT_X, 128, C_WHITE, "%d", level);
}

static void draw_next(void)
{
    dtext(RIGHT_X, 14, C_LIGHT, "NEXT");

    int box_x = RIGHT_X;
    int box_y = 28;
    drect_border(box_x - 2, box_y - 2, box_x + 4 * CELL + 1,
        box_y + 4 * CELL + 1, C_NONE, 1, C_DARK);

    Shape const *s = &SHAPES[next_type];
    int offset = (4 - s->size) * CELL / 2;
    for (int i = 0; i < 4; i++) {
        draw_block(box_x + offset + s->cells[i].x * CELL,
                   box_y + offset + s->cells[i].y * CELL, s->color);
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

    draw_stats();
    draw_next();

    dupdate();
}

int main(void)
{
    gint_setrestart(1);

    srand(rtc_ticks());
    next_type = take_from_bag();
    spawn_piece();

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

        update_playing();
        draw();
    }

    timer_stop(timer);
    return 0;
}
