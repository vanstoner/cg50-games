/*
 * CG50 Tetris
 *
 * A complete Tetris for the Casio fx-CG50, written with gint.
 *
 * README.md in this folder builds this file up step by step in 9 layers.
 * The "Layer n" notes below show which step each part comes from, and
 * steps/ has a copy of the program as it looks at the end of each layer.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/timer.h>
#include <gint/cpu.h>
#include <gint/rtc.h>
#include <gint/gint.h>

/* ======================================================================
 * Constants
 * ====================================================================== */

#define FPS      60     /* Layer 1: frames per second */

#define COLS     10     /* Layer 2: board width, in cells  */
#define ROWS     20     /*          board height, in cells */
#define CELL     10     /*          size of one cell, in pixels */

/* Put the 100x200 board in the middle of the 396x224 screen */
#define BOARD_X  ((DWIDTH  - COLS * CELL) / 2)
#define BOARD_Y  ((DHEIGHT - ROWS * CELL) / 2)

/* Layer 8: side panels */
#define LEFT_X   16
#define RIGHT_X  (BOARD_X + COLS * CELL + 16)

/* ======================================================================
 * Layer 3: The seven pieces
 *
 * Each piece is 4 blocks inside a small square box (2x2, 3x3 or 4x4).
 * We only store the starting position; rotations are calculated.
 * ====================================================================== */

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point cells[4];     /* where the 4 blocks are inside the box */
    int size;           /* the box is size x size cells */
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

/* A piece that is on the board: which shape, how rotated, and where */
typedef struct {
    int type;   /* 0..6, index into SHAPES */
    int rot;    /* 0..3, number of clockwise quarter turns */
    int x, y;   /* board position of the top-left of its box */
} Piece;

/* ======================================================================
 * Game state: everything that describes the game at this moment
 * ====================================================================== */

/* Layer 9: which screen we're on */
typedef enum {
    STATE_TITLE,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER,
} State;

static State state = STATE_TITLE;

/* Layer 2: the board. 0 = empty, otherwise (piece type + 1) */
static int board[ROWS][COLS];

static Piece current;       /* Layer 3: the falling piece */
static int fall_timer;      /* Layer 5: frames since it last moved down */
static int next_type;       /* Layer 8: the piece shown in the NEXT box */

static int score, lines, level;     /* Layer 8 */
static int best;                    /* Layer 9 */

/* ======================================================================
 * Layer 3: Where are a piece's blocks?
 * ====================================================================== */

/* Board position of block i of piece p.
   Layer 6 adds rotation: turning a point (x, y) a quarter turn clockwise
   inside a box of size n moves it to (n-1-y, x). We do that once per
   quarter turn. */
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

/* ======================================================================
 * Layer 5: Collision - does a piece fit where it is?
 * ====================================================================== */

static bool fits(Piece const *p)
{
    for (int i = 0; i < 4; i++) {
        Point c = piece_cell(p, i);
        if (c.x < 0 || c.x >= COLS || c.y < 0 || c.y >= ROWS)
            return false;               /* off the board */
        if (board[c.y][c.x] != 0)
            return false;               /* hits a settled block */
    }
    return true;
}

/* Move the current piece if it fits there. Returns true if it moved. */
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

/* ======================================================================
 * Layer 6: Rotation with wall kicks
 *
 * If the rotated piece doesn't fit (e.g. it's against a wall), try
 * nudging it a little to the side or up before giving up.
 * ====================================================================== */

static void try_rotate(int direction)   /* +1 = clockwise, -1 = anticlockwise */
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
    /* Nothing fitted: the piece doesn't rotate */
}

/* ======================================================================
 * Layer 8: Picking pieces fairly (the "7-bag")
 *
 * Put one of each piece in a bag, shuffle it, and hand them out in that
 * order. When the bag is empty, refill it. You never wait ages for an I.
 * ====================================================================== */

static int bag[7];
static int bag_left = 0;

static int take_from_bag(void)
{
    if (bag_left == 0) {
        for (int i = 0; i < 7; i++)
            bag[i] = i;
        /* Shuffle (Fisher-Yates) */
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

/* ======================================================================
 * Layer 7: New pieces, locking, and clearing lines
 * ====================================================================== */

static void clear_board(void)
{
    for (int y = 0; y < ROWS; y++)
        for (int x = 0; x < COLS; x++)
            board[y][x] = 0;
}

/* Bring the NEXT piece onto the board. Returns false if there's no room,
   which means the game is over. */
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

/* Remove full rows and drop everything above them. Returns how many. */
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

        /* Shift every row above this one down by one */
        for (int yy = y; yy > 0; yy--)
            for (int x = 0; x < COLS; x++)
                board[yy][x] = board[yy - 1][x];
        for (int x = 0; x < COLS; x++)
            board[0][x] = 0;

        cleared++;
        y++;    /* this row now holds what was above it: check it again */
    }
    return cleared;
}

/* Layer 8: how many frames the piece waits before falling one row */
static int frames_per_row(void)
{
    static const int speed[] = {
        48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2,
    };
    int n = sizeof speed / sizeof speed[0];
    return speed[level < n ? level : n - 1];
}

/* Copy the current piece into the board, score, and bring in the next one */
static void lock_piece(void)
{
    for (int i = 0; i < 4; i++) {
        Point c = piece_cell(&current, i);
        board[c.y][c.x] = current.type + 1;
    }

    /* Layer 8: scoring */
    static const int points[5] = { 0, 40, 100, 300, 1200 };
    int cleared = clear_lines();
    score += points[cleared] * (level + 1);
    lines += cleared;
    level = lines / 10;

    /* Layer 9: no room for the next piece means game over */
    if (!spawn_piece()) {
        state = STATE_GAME_OVER;
        if (score > best)
            best = score;
    }
}

/* ======================================================================
 * Layer 9: Hard drop, the ghost piece, and starting a game
 * ====================================================================== */

/* Drop straight to the bottom and lock immediately */
static void hard_drop(void)
{
    int rows = 0;
    while (try_move(0, 1))
        rows++;
    score += 2 * rows;
    lock_piece();
}

/* Where would the current piece land? (for the ghost outline) */
static Piece landing_spot(void)
{
    Piece p = current;
    while (true) {
        Piece lower = p;
        lower.y++;
        if (!fits(&lower))
            return p;
        p = lower;
    }
}

static void new_game(void)
{
    clear_board();
    score = 0;
    lines = 0;
    level = 0;
    bag_left = 0;

    /* rtc_ticks() depends on exactly when EXE was pressed, so every
       game gets a different order of pieces */
    srand(rtc_ticks());
    next_type = take_from_bag();
    spawn_piece();
    state = STATE_PLAYING;
}

/* ======================================================================
 * Layer 4: Keyboard input
 * ====================================================================== */

/* Auto-repeat for held keys: move once straight away, then after a short
   pause keep moving every few frames (like holding a key on a computer). */
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

/* One frame of the game while it's being played (Layers 4 to 9) */
static void update_playing(void)
{
    static int left_held, right_held;

    if (keypressed(KEY_F1)) {               /* Layer 9 */
        state = STATE_PAUSED;
        return;
    }

    if (key_repeat(KEY_LEFT, &left_held))   /* Layers 4 and 5 */
        try_move(-1, 0);
    if (key_repeat(KEY_RIGHT, &right_held))
        try_move(1, 0);

    if (keypressed(KEY_UP) || keypressed(KEY_SHIFT))    /* Layer 6 */
        try_rotate(+1);
    if (keypressed(KEY_ALPHA))
        try_rotate(-1);

    if (keypressed(KEY_EXE)) {              /* Layer 9 */
        hard_drop();
        return;
    }

    /* Layer 5: gravity. Holding DOWN makes the piece fall much faster. */
    int delay = frames_per_row();
    bool soft_drop = keydown(KEY_DOWN);
    if (soft_drop && delay > 2)
        delay = 2;

    if (++fall_timer >= delay) {
        fall_timer = 0;
        if (try_move(0, 1)) {
            if (soft_drop)
                score += 1;                 /* Layer 8 */
        }
        else {
            lock_piece();   /* Layer 7: couldn't fall any further: landed */
        }
    }
}

/* ======================================================================
 * Drawing
 * ====================================================================== */

/* Layer 2: one block at pixel position (px, py). The 1-pixel gap makes
   a grid. */
static void draw_block(int px, int py, int color)
{
    drect(px, py, px + CELL - 2, py + CELL - 2, color);
}

/* Layer 2: one block at board cell (col, row) */
static void draw_cell(int col, int row, int color)
{
    draw_block(BOARD_X + col * CELL, BOARD_Y + row * CELL, color);
}

/* Layer 3: a whole piece. Layer 9 adds the outline-only "ghost" style. */
static void draw_piece(Piece const *p, bool ghost)
{
    int color = SHAPES[p->type].color;
    for (int i = 0; i < 4; i++) {
        Point c = piece_cell(p, i);
        if (ghost) {
            int px = BOARD_X + c.x * CELL;
            int py = BOARD_Y + c.y * CELL;
            drect_border(px, py, px + CELL - 2, py + CELL - 2,
                C_NONE, 1, color);
        }
        else {
            draw_cell(c.x, c.y, color);
        }
    }
}

/* Layer 8: score, lines and level down the left side */
static void draw_stats(void)
{
    dtext(LEFT_X, 14, C_WHITE, "TETRIS");

    dtext(LEFT_X, 44, C_LIGHT, "SCORE");
    dprint(LEFT_X, 56, C_WHITE, "%d", score);

    dtext(LEFT_X, 80, C_LIGHT, "LINES");
    dprint(LEFT_X, 92, C_WHITE, "%d", lines);

    dtext(LEFT_X, 116, C_LIGHT, "LEVEL");
    dprint(LEFT_X, 128, C_WHITE, "%d", level);

    dtext(LEFT_X, 152, C_LIGHT, "BEST");            /* Layer 9 */
    dprint(LEFT_X, 164, C_WHITE, "%d", best);
}

/* Layer 8: the NEXT box */
static void draw_next(void)
{
    dtext(RIGHT_X, 14, C_LIGHT, "NEXT");

    /* A 4x4-cell box, with the piece centred in it */
    int box_x = RIGHT_X;
    int box_y = 28;
    drect_border(box_x - 2, box_y - 2, box_x + 4 * CELL + 1,
        box_y + 4 * CELL + 1, C_NONE, 1, C_DARK);

    if (state == STATE_TITLE)
        return;

    Shape const *s = &SHAPES[next_type];
    int offset = (4 - s->size) * CELL / 2;
    for (int i = 0; i < 4; i++) {
        draw_block(box_x + offset + s->cells[i].x * CELL,
                   box_y + offset + s->cells[i].y * CELL, s->color);
    }
}

/* Layer 9: which key does what */
static void draw_controls(void)
{
    static char const *help[] = {
        "<- ->  move",
        "UP     rotate",
        "ALPHA  turn back",
        "DOWN   soft drop",
        "EXE    hard drop",
        "F1     pause",
        "MENU   main menu",
        "EXIT   quit",
    };
    for (unsigned i = 0; i < sizeof help / sizeof help[0]; i++)
        dtext(RIGHT_X, 90 + 14 * i, C_DARK, help[i]);
}

/* Layer 9: a message box across the middle of the board */
static void draw_message(char const *line1, char const *line2)
{
    int cx = BOARD_X + COLS * CELL / 2;
    int cy = BOARD_Y + ROWS * CELL / 2;

    drect_border(BOARD_X - 6, cy - 22, BOARD_X + COLS * CELL + 5, cy + 22,
        C_BLACK, 2, C_WHITE);
    dtext_opt(cx, cy - 9, C_WHITE, C_NONE, DTEXT_CENTER, DTEXT_MIDDLE,
        line1, -1);
    dtext_opt(cx, cy + 9, C_LIGHT, C_NONE, DTEXT_CENTER, DTEXT_MIDDLE,
        line2, -1);
}

static void draw(void)
{
    dclear(C_BLACK);

    /* Layer 2: board background and frame */
    drect_border(BOARD_X - 2, BOARD_Y - 2,
        BOARD_X + COLS * CELL + 1, BOARD_Y + ROWS * CELL + 1,
        C_RGB(2, 2, 4), 2, C_WHITE);

    /* Layer 2: blocks that have already landed */
    for (int y = 0; y < ROWS; y++)
        for (int x = 0; x < COLS; x++)
            if (board[y][x] != 0)
                draw_cell(x, y, SHAPES[board[y][x] - 1].color);

    /* Layer 3: the falling piece. Layer 9: its ghost */
    if (state == STATE_PLAYING || state == STATE_PAUSED) {
        Piece ghost = landing_spot();
        draw_piece(&ghost, true);
        draw_piece(&current, false);
    }

    draw_stats();
    draw_next();
    draw_controls();

    if (state == STATE_TITLE)
        draw_message("TETRIS", "Press EXE");
    else if (state == STATE_PAUSED)
        draw_message("PAUSED", "F1: resume");
    else if (state == STATE_GAME_OVER)
        draw_message("GAME OVER", "EXE: again");

    dupdate();
}

/* ======================================================================
 * Layer 1: The game loop
 * ====================================================================== */

int main(void)
{
    /* When main() returns, the calculator would normally refuse to open
       Tetris again until you'd been into another app. This makes gint go
       back to the menu in a way that lets you restart straight away. */
    gint_setrestart(1);

    /* A hardware timer sets `tick` to 1 sixty times a second. Each time
       round the loop we wait for it, so the game runs at the same speed
       however long drawing takes. */
    volatile int tick = 0;
    int timer = timer_configure(TIMER_ANY, 1000000 / FPS, GINT_CALL_SET(&tick));
    if (timer < 0)
        return 1;
    timer_start(timer);

    while (true) {
        /* Wait for the next frame (sleep() saves battery while waiting) */
        while (!tick)
            sleep();
        tick = 0;

        /* Read the keyboard. keypressed() will then tell us which keys
           went down since last frame; keydown() which are held now. */
        cleareventflips();
        clearevents();

        if (keypressed(KEY_EXIT))
            break;

        /* MENU pops out to the calculator's main menu. If you pick Tetris
           again, gint_osmenu() returns and the game carries on (paused). */
        if (keypressed(KEY_MENU)) {
            if (state == STATE_PLAYING)
                state = STATE_PAUSED;
            gint_osmenu();
            clearevents();
        }

        /* Layer 9: what happens depends on which screen we're on */
        switch (state) {
        case STATE_TITLE:
        case STATE_GAME_OVER:
            if (keypressed(KEY_EXE))
                new_game();
            break;
        case STATE_PAUSED:
            if (keypressed(KEY_F1))
                state = STATE_PLAYING;
            break;
        case STATE_PLAYING:
            update_playing();
            break;
        }

        draw();
    }

    timer_stop(timer);
    return 0;
}
