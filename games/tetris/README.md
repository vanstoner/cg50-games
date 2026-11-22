# Build Tetris for the CG50, layer by layer

This folder has a complete Tetris game for the Casio fx-CG50 ([src/main.c](src/main.c)). This guide shows you how to build it yourself **from an empty file**, one layer at a time.

Each layer adds one idea. At the end of every layer you have a program that compiles and runs, so you always know where you are. If you get stuck, the [steps/](steps/) folder has a working copy of the program at the end of each layer to compare against.

When you finish you won't just have Tetris. You'll know the handful of ideas that almost every 2D game is built from, and the last section shows how to use them to make Snake, Breakout, Minesweeper and more.

| Layer | You'll add | You'll learn |
|---|---|---|
| [0](#layer-0-your-toolkit) | Nothing yet! | The screen, colours, and the bits of C you'll need |
| [1](#layer-1-the-game-loop) | A screen that updates 60 times a second | The **game loop**, timers |
| [2](#layer-2-the-board) | The 10×20 playing field | Keeping the game world in **data**, then drawing it |
| [3](#layer-3-the-pieces) | The seven Tetris pieces | **Structs**, describing shapes as data |
| [4](#layer-4-keyboard-input) | Moving a piece with the arrows | `keypressed` vs `keydown`, **key repeat** |
| [5](#layer-5-collision-and-gravity) | Walls, floors, and falling | **Collision** ("try it, then keep it"), timing in frames |
| [6](#layer-6-rotation) | Rotating pieces | A little **maths**: turning points around a box |
| [7](#layer-7-locking-and-clearing-lines) | Pieces that stack, rows that vanish | Changing the world, **random numbers** |
| [8](#layer-8-scoring-levels-and-fair-pieces) | Score, levels, NEXT box | Difficulty curves, **shuffling** |
| [9](#layer-9-game-states-and-polish) | Title, pause, game over, ghost piece | **State machines**, polish |
| [10](#layer-10-make-your-own-game) | Your own game! | Reusing the recipe |

---

## Before you start

### Make your own copy

Don't type over the finished game. Make your own project next to it:

```bash
cp -r games/tetris games/my-tetris
```

Then open `games/my-tetris/src/main.c`, delete everything in it, and start Layer 1 there.

### Building and testing

From the top folder of the repo:

```bash
./run-emulator.sh games/my-tetris
```

This compiles your code and opens the emulator at **http://localhost:6080**. If your code has a mistake, the compiler's error message appears in the terminal instead. (See [Troubleshooting](#troubleshooting) for what the common ones mean.)

In the emulator, your computer's **arrow keys** work. **Shift** is the calculator's ALPHA key, **X** or **Space** is SHIFT, and you click the on-screen buttons for everything else (EXE, EXIT, F1...).

### Peeking at a checkpoint

To run the program as it should be at the end of a layer:

```bash
cp games/tetris/steps/layer3.c games/my-tetris/src/main.c
./run-emulator.sh games/my-tetris
```

(That replaces your `main.c`, so save your own version somewhere first if you want to keep it.)

---

## Layer 0: Your toolkit

Read this once, then come back to it whenever you need to.

### The screen

The CG50 screen is **396 pixels wide** and **224 pixels tall**. gint calls these `DWIDTH` and `DHEIGHT`.

Positions are measured from the **top-left corner**. `x` goes right and `y` goes **down** (not up like in maths lessons):

```
(0,0) ──────────── x ───────────▶ (395,0)
  │
  │        (100, 50) is 100 right, 50 down
  y
  │
  ▼
(0,223)                          (395,223)
```

### Colours

Colours are made with `C_RGB(red, green, blue)`. Each part goes from **0 to 31** (not 0–255 like on a computer):

```c
C_RGB(31,  0,  0)   // bright red
C_RGB( 0, 31,  0)   // bright green
C_RGB(31, 31,  0)   // yellow
C_RGB( 4,  4,  8)   // very dark blue-grey
```

There are also some ready-made ones: `C_BLACK`, `C_WHITE`, `C_RED`, `C_GREEN`, `C_BLUE`, `C_LIGHT` (light grey), `C_DARK` (dark grey), and `C_NONE` (meaning "don't draw anything").

### Drawing with gint

| Function | What it does |
|---|---|
| `dclear(color)` | Fill the whole screen with one colour |
| `drect(x1, y1, x2, y2, color)` | Filled rectangle from corner (x1,y1) to corner (x2,y2), **including** both corners |
| `drect_border(x1, y1, x2, y2, fill, width, border)` | Rectangle with a border. Use `C_NONE` as `fill` for an outline only |
| `dline(x1, y1, x2, y2, color)` | A line |
| `dpixel(x, y, color)` | One pixel |
| `dtext(x, y, color, "text")` | Text, with its top-left corner at (x, y) |
| `dprint(x, y, color, "Score: %d", score)` | Text with numbers in it (like `printf`) |
| `dupdate()` | **Show** everything you've drawn |

That last one matters. Drawing functions don't change the screen straight away. They draw into a hidden copy of the screen in memory, and `dupdate()` copies the whole picture to the real screen in one go. That's why games don't flicker: you never see a half-drawn picture.

So every frame looks like this:

```c
dclear(C_BLACK);     // 1. wipe the hidden picture
/* ...draw everything... */
dupdate();           // 2. show it
```

### The C you'll need

You don't need to know much C to start. Here's what this guide uses, with examples:

```c
int score = 0;              // a variable holding a whole number
bool running = true;        // true or false (needs #include <stdbool.h>)

#define COLS 10             // a name for a fixed number. The compiler
                            // swaps COLS for 10 everywhere.

int board[20][10];          // a grid of numbers: 20 rows of 10
board[3][7] = 1;            // row 3, column 7 (counting from 0!)

static void draw(void)      // a function that takes nothing and returns nothing
{
    /* ... */
}

static int add(int a, int b)  // a function that takes two ints and returns one
{
    return a + b;
}

for (int i = 0; i < 4; i++) {   // repeat with i = 0, 1, 2, 3
    /* ... */
}
```

Two things trip everyone up in C:

- **Everything counts from 0.** A board with 10 columns has columns 0 to 9. Column 10 doesn't exist, and using it causes strange bugs.
- **Order matters.** A function has to be written *above* the code that uses it. If the compiler says something is "undeclared" or "implicit", that's usually why.

`static` in front of a function or variable just means "only this file uses it". You can ignore it. We'll explain structs and pointers when we get to them.

---

## Layer 1: The game loop

**Goal:** a screen that redraws itself 60 times a second and quits when you press EXIT.

### The idea

Almost every game in the world is one loop that goes round and round:

```
     ┌──────────────────────────┐
     ▼                          │
  wait for next frame           │
     │                          │
  read the keyboard             │
     │                          │
  update the game               │   60 times
  (move things, check rules)    │   a second
     │                          │
  draw everything               │
     │                          │
     └──────────────────────────┘
```

Each trip round the loop is called a **frame**. If we do 60 frames a second, movement looks smooth.

### Keeping a steady speed

How do we make the loop run exactly 60 times a second? The calculator has hardware **timers** that can poke our program at regular intervals. We ask one to set a variable called `tick` to 1 every 1/60th of a second:

```c
volatile int tick = 0;
int timer = timer_configure(TIMER_ANY, 1000000 / FPS, GINT_CALL_SET(&tick));
```

- `1000000 / FPS` is the delay in **microseconds** (millionths of a second). 1,000,000 ÷ 60 ≈ 16,667.
- `GINT_CALL_SET(&tick)` means "when the timer fires, set `tick` to 1".
- `volatile` warns the compiler that `tick` can change by itself (the timer changes it, not our code). Without it, the compiler might "optimise" our waiting loop into one that never notices.

Then at the start of every frame we wait for the tick:

```c
while (!tick)
    sleep();    // nap until something happens (saves battery)
tick = 0;       // reset, ready for the next one
```

The game now runs at the same speed whether a frame takes a long or short time to draw.

### Reading the keyboard

```c
cleareventflips();
clearevents();

if (keypressed(KEY_EXIT))
    break;      // leave the loop, so the program ends
```

These two lines read all the key presses that have happened since last frame. After them, you can ask:

- `keypressed(KEY_EXIT)`: was EXIT **just** pressed this frame? (True for one frame only.)
- `keydown(KEY_EXIT)`: is EXIT being **held down** right now? (True every frame while held.)

We'll use both in Layer 4.

### The code

Here's the whole program for Layer 1. Type it into your `main.c`:

```c
#include <stdbool.h>
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/timer.h>
#include <gint/cpu.h>

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
```

`main()` is where every C program starts. When it `return`s, the program ends and the calculator goes back to its menu.

### ✅ Checkpoint

Run it. You should see the frame counter going up by about 60 every second, and EXIT should quit. Compare with [steps/layer1.c](steps/layer1.c).

### 🧪 Try this

- Change `FPS` to 10. What happens to the counter? Change it back.
- Draw a rectangle that moves: `drect(frames % 396, 100, frames % 396 + 20, 120, C_RED);`. (`%` gives the remainder after dividing, so `frames % 396` counts 0 to 395 and starts again.)

---

## Layer 2: The board

**Goal:** draw the 10×20 Tetris playing field in the middle of the screen.

### The idea: data first, drawing second

This is the most important idea in this guide:

> **The game lives in variables. Drawing just shows what's in them.**

We don't draw a block and hope we remember it's there. We store the whole board as a grid of numbers. Then every frame we draw whatever the grid says.

```c
static int board[ROWS][COLS];   // 0 = empty, anything else = a block
```

```
          column (x)
         0 1 2 3 4 5 6 7 8 9
       0 . . . . . . . . . .
       1 . . . . . . . . . .
  row  : . . . . . . . . . .
  (y) 18 # . . . . . . . . .
      19 # # . . . . . . . #
```

⚠️ Notice it's `board[row][column]`, which is `board[y][x]`: **y comes first**. Everyone gets this backwards at least once.

### From cells to pixels

Each cell is 10×10 pixels, so the board is 100×200 pixels. To put it in the middle of the 396×224 screen:

```c
#define COLS     10
#define ROWS     20
#define CELL     10

#define BOARD_X  ((DWIDTH  - COLS * CELL) / 2)   // (396 - 100) / 2 = 148
#define BOARD_Y  ((DHEIGHT - ROWS * CELL) / 2)   // (224 - 200) / 2 = 12
```

Then cell (col, row) has its top-left pixel at:

```
pixel x = BOARD_X + col * CELL
pixel y = BOARD_Y + row * CELL
```

We'll write two small functions: one that draws a block at a **pixel** position, and one that draws a block at a **cell** position:

```c
static void draw_block(int px, int py, int color)
{
    drect(px, py, px + CELL - 2, py + CELL - 2, color);
}

static void draw_cell(int col, int row, int color)
{
    draw_block(BOARD_X + col * CELL, BOARD_Y + row * CELL, color);
}
```

Why `CELL - 2` and not `CELL - 1`? A cell covers pixels 0 to 9. Stopping at 8 leaves a 1-pixel gap on the right and bottom, so you can see the grid between blocks.

### Drawing the board

In `draw()`, after `dclear`, draw a frame around the board, then every block that's filled in:

```c
drect_border(BOARD_X - 2, BOARD_Y - 2,
    BOARD_X + COLS * CELL + 1, BOARD_Y + ROWS * CELL + 1,
    C_RGB(2, 2, 4), 2, C_WHITE);

for (int y = 0; y < ROWS; y++)
    for (int x = 0; x < COLS; x++)
        if (board[y][x] != 0)
            draw_cell(x, y, C_WHITE);
```

Two loops, one inside the other, visit every cell: all the columns of row 0, then all the columns of row 1, and so on.

### Test it

The board starts empty, so put some test blocks in at the start of `main()`:

```c
board[19][0] = 1;       // bottom-left corner
board[19][1] = 1;
board[18][0] = 1;
board[19][9] = 1;       // bottom-right corner
board[0][4]  = 1;       // top row
```

### ✅ Checkpoint

You should see a dark board with a white frame and five white blocks: three in the bottom-left corner, one bottom-right, and one at the top. Compare with [steps/layer2.c](steps/layer2.c).

### 🧪 Try this

- Draw your initials in blocks.
- What happens if you set `board[20][0] = 1`? (Row 20 doesn't exist! This kind of mistake can crash the program or cause weird bugs, which is exactly why we'll be careful about it in Layer 5.)

---

## Layer 3: The pieces

**Goal:** describe the seven Tetris pieces and draw one on the board.

### Structs: grouping things together

A **struct** bundles several variables into one thing. For example, a point has an x and a y:

```c
typedef struct {
    int x, y;
} Point;

Point p = { 3, 5 };     // p.x is 3, p.y is 5
```

### Describing a shape

Every Tetris piece is **4 blocks** inside a small square box. Here they are, with the box drawn round each one:

```
  I (4x4)     O (2x2)    T (3x3)    S (3x3)    Z (3x3)    J (3x3)    L (3x3)
  . . . .     # #        . # .      . # #      # # .      # . .      . . #
  # # # #     # #        # # #      # # .      . # #      # # #      # # #
  . . . .                . . .      . . .      . . .      . . .      . . .
  . . . .
```

We store each shape as the positions of its 4 blocks inside its box, plus the box size and a colour:

```c
typedef struct {
    Point cells[4];     // where the 4 blocks are inside the box
    int size;           // the box is size x size cells
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
```

Check one against the picture. The T has blocks at (1,0), (0,1), (1,1) and (2,1): the middle of the top row, then the whole second row. ✔

`const` means these never change while the game runs.

Why bother with the box? You'll find out in Layer 6: it's what makes rotation work.

### A piece on the board

A shape is just a description. A **piece** is an actual shape on the board, so it also needs a position and a rotation:

```c
typedef struct {
    int type;   // 0..6, which entry of SHAPES
    int rot;    // 0..3, number of quarter turns (used in Layer 6)
    int x, y;   // board position of the top-left of its box
} Piece;

static Piece current;   // the piece that's falling
```

### Where are its blocks?

To draw or move a piece, we need to know which board cells its 4 blocks are on. Block `i` of a piece is at its shape's cell `i`, moved by the piece's position:

```c
static Point piece_cell(Piece const *p, int i)
{
    Shape const *s = &SHAPES[p->type];
    int x = s->cells[i].x;
    int y = s->cells[i].y;
    return (Point){ p->x + x, p->y + y };
}
```

That `*` and `->` are **pointers**. `Piece const *p` means "where to find a Piece" rather than a copy of it, and `p->x` means "the x of the Piece that p points to". For now, think of `p->x` as meaning the same as `p.x`. `&SHAPES[...]` means "where to find" that shape.

Drawing a piece is then just drawing its 4 blocks:

```c
static void draw_piece(Piece const *p)
{
    int color = SHAPES[p->type].color;
    for (int i = 0; i < 4; i++) {
        Point c = piece_cell(p, i);
        draw_cell(c.x, c.y, color);
    }
}
```

### Colours on the board

When a piece lands (Layer 7) we'll copy it into `board`. To remember its colour, we store **the piece type + 1** in each cell. We add 1 because 0 already means "empty", and the I piece is type 0.

So in `draw()`, change the board loop to look up the colour:

```c
if (board[y][x] != 0)
    draw_cell(x, y, SHAPES[board[y][x] - 1].color);
```

and after it, draw the falling piece with `draw_piece(&current);`.

At the start of `main()`, make some coloured test blocks and a piece:

```c
board[19][0] = 1;       // I colour
board[19][1] = 2;       // O colour
board[18][0] = 3;       // T colour
board[19][9] = 5;       // Z colour

current = (Piece){ .type = 2, .rot = 0, .x = 3, .y = 0 };
```

### ✅ Checkpoint

A purple T near the top of the board, and coloured test blocks at the bottom. Compare with [steps/layer3.c](steps/layer3.c).

### 🧪 Try this

- Change `.type = 2` to each number from 0 to 6 and check every shape looks right.
- Invent an eighth piece. (You'll need to change `SHAPES[7]` to `SHAPES[8]`.)

---

## Layer 4: Keyboard input

**Goal:** move the piece around with the arrow keys.

### Pressed vs held

Remember the two ways to check a key:

```
  key:          up   up  DOWN DOWN DOWN DOWN  up   up
  frame:         1    2    3    4    5    6    7    8
  keydown():     .    .    ✔    ✔    ✔    ✔    .    .
  keypressed():  .    .    ✔    .    .    .    .    .
```

- **`keypressed`** is for things that should happen **once** per press: rotating, pausing, starting a game.
- **`keydown`** is for things that happen **all the time** you hold the key: soft drop, or a spaceship's engine.

### Key repeat

For moving left and right, neither is quite right. `keypressed` means tapping 9 times to cross the board. `keydown` moves one cell **every frame**, which is 60 cells a second, far too fast to control.

What we want is what your computer does when you hold a key: move once straight away, wait a moment, then repeat steadily:

```
  held for frame:  1   2 ... 9   10  11  12  13  14  15  16 ...
  move?            ✔   .     .   ✔   .   .   ✔   .   .   ✔
                   │             │           │           │
                   first move    after       then every
                   immediately   a pause     3 frames
```

We count how many frames the key has been held, and decide from that:

```c
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
```

`int *held_frames` is a pointer again. Each key needs its own counter, so we pass in *where* that key's counter lives and the function updates it.

### Updating the game

Add a function that runs once per frame and reacts to the keys:

```c
static void update_playing(void)
{
    static int left_held, right_held, down_held;

    if (key_repeat(KEY_LEFT, &left_held))
        current.x--;
    if (key_repeat(KEY_RIGHT, &right_held))
        current.x++;
    if (key_repeat(KEY_DOWN, &down_held))
        current.y++;
    if (keypressed(KEY_UP))
        current.y = 0;          // back to the top, handy for testing
}
```

`static` *inside* a function means the variable keeps its value between calls, instead of resetting each time. That's exactly what the counters need.

`current.x--` is short for `current.x = current.x - 1`.

Call it in the main loop, just before `draw()`:

```c
update_playing();
draw();
```

### ✅ Checkpoint

The arrows move the T around, holding them repeats, and UP sends it back to the top. Compare with [steps/layer4.c](steps/layer4.c).

### 🐛 Spot the problem

Move the piece left a few times. It goes straight through the wall and off the board. It also goes straight through the test blocks. Layer 5 fixes that.

### 🧪 Try this

- Change `REPEAT_DELAY` and `REPEAT_RATE`. What feels best?

---

## Layer 5: Collision and gravity

**Goal:** the piece can't go through walls or blocks, and falls by itself.

### Does it fit?

A piece **fits** if all 4 of its blocks are on the board *and* on empty cells:

```c
static bool fits(Piece const *p)
{
    for (int i = 0; i < 4; i++) {
        Point c = piece_cell(p, i);
        if (c.x < 0 || c.x >= COLS || c.y < 0 || c.y >= ROWS)
            return false;               // off the board
        if (board[c.y][c.x] != 0)
            return false;               // hits a settled block
    }
    return true;
}
```

`||` means "or". The order matters here: we check the block is on the board **before** looking at `board[c.y][c.x]`, so we never read a row or column that doesn't exist.

### Try it, then keep it

Here's a trick you'll use in nearly every game. To move something, **make a copy, move the copy, and only keep it if it's allowed**:

```c
static bool try_move(int dx, int dy)
{
    Piece moved = current;      // a copy
    moved.x += dx;
    moved.y += dy;
    if (!fits(&moved))
        return false;           // not allowed: forget the copy
    current = moved;            // allowed: keep it
    return true;
}
```

`dx` and `dy` mean "change in x" and "change in y". So `try_move(-1, 0)` is left, `try_move(1, 0)` is right, and `try_move(0, 1)` is down.

### Gravity

To make the piece fall, we count frames. Every `FALL_DELAY` frames, it moves down one row:

```c
#define FALL_DELAY 48       // 48 frames = 0.8 seconds

static int fall_timer;
```

```c
if (++fall_timer >= FALL_DELAY) {
    fall_timer = 0;
    try_move(0, 1);
}
```

`++fall_timer` adds 1 and then uses the new value.

> 💡 **Timing in frames.** Because the loop runs exactly 60 times a second, counting frames is the same as measuring time: 60 frames = 1 second. Want something to happen every half second? Count to 30.

**Soft drop:** while DOWN is held, use a much shorter delay.

Here's the new `update_playing()`:

```c
static void update_playing(void)
{
    static int left_held, right_held;

    if (key_repeat(KEY_LEFT, &left_held))
        try_move(-1, 0);
    if (key_repeat(KEY_RIGHT, &right_held))
        try_move(1, 0);

    int delay = FALL_DELAY;
    bool soft_drop = keydown(KEY_DOWN);
    if (soft_drop && delay > 2)
        delay = 2;

    if (++fall_timer >= delay) {
        fall_timer = 0;
        if (!try_move(0, 1)) {
            /* Landed. For now just start again at the top;
               Layer 7 will make it stay where it landed. */
            current.y = 0;
        }
    }
}
```

If `try_move(0, 1)` fails, the piece couldn't fall any further: it has **landed**. We'll deal with that properly in Layer 7.

Add `board[12][5] = 4;` to your test blocks too, so there's something floating in the middle to bump into.

### ✅ Checkpoint

The T falls by itself, can't leave the board, can't pass through blocks, and goes back to the top when it lands. Holding DOWN makes it fall fast. Compare with [steps/layer5.c](steps/layer5.c).

---

## Layer 6: Rotation

**Goal:** rotate pieces, even when they're against a wall.

### Turning a point

Here's the clever bit. Turning a block a quarter turn **clockwise** inside a box of size `n` moves it from `(x, y)` to `(n-1-y, x)`.

Let's check with the T (box size 3, so `n-1` is 2):

```
  before          rule (x,y) → (2-y, x)          after
  . # .           (1,0) → (2-0, 1) = (2,1)        . # .
  # # #           (0,1) → (2-1, 0) = (1,0)        . # #
  . . .           (1,1) → (2-1, 1) = (1,1)        . # .
                  (2,1) → (2-1, 2) = (1,2)
```

The T was pointing up, and now it points right. That's a clockwise turn. ✔

This is why each shape has a box. The rule turns blocks around the **middle of the box**, so the piece spins in place instead of flying off somewhere. It's also why the O has a 2×2 box: turning it gives exactly the same square, which is how an O should behave.

To turn the piece `rot` times, apply the rule `rot` times. Add this loop to `piece_cell()`, between reading `x`/`y` and the `return`:

```c
for (int r = 0; r < p->rot; r++) {
    int old_x = x;
    x = s->size - 1 - y;
    y = old_x;
}
```

We need `old_x` because the new `y` uses the old `x`, and we've just overwritten `x` on the line before.

### Rotating the current piece

Rotating is just like moving: try it on a copy and only keep it if it fits.

```c
turned.rot = (turned.rot + direction + 4) % 4;
```

`direction` is +1 for clockwise or -1 for anticlockwise. The `% 4` wraps 4 back round to 0, because four quarter turns is a full circle. The `+ 4` stops it going negative when turning anticlockwise from 0: (0 - 1 + 4) % 4 = 3. ✔

### Wall kicks

Try rotating a vertical I piece that's right against the wall, and it won't fit, because half of it would stick out. That feels broken to a player. The fix is a **wall kick**: if the rotated piece doesn't fit, try nudging it sideways (or up) a little before giving up.

```c
static void try_rotate(int direction)   // +1 = clockwise, -1 = anticlockwise
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
```

It tries the spot it's in (`{0, 0}`), then one left, one right, one up, two left and two right, and keeps the first that fits.

`sizeof kicks / sizeof kicks[0]` is a C trick for "how many things are in this list". So you can add more kicks without changing the loop.

Hook it up to the keys in `update_playing()`:

```c
if (keypressed(KEY_UP) || keypressed(KEY_SHIFT))
    try_rotate(+1);
if (keypressed(KEY_ALPHA))
    try_rotate(-1);
```

To make testing easier, change the "landed" part of `update_playing()` so each landing gives you the next shape:

```c
current.type = (current.type + 1) % 7;
current.rot = 0;
current.y = 0;
```

### ✅ Checkpoint

UP or SHIFT turns pieces clockwise, ALPHA turns them back, and every shape rotates in place. Push an I against a wall and rotate it: it should hop away from the wall rather than refusing. Compare with [steps/layer6.c](steps/layer6.c).

### 🧪 Try this

- Remove all the kicks except `{0, 0}`, then try rotating against walls. Feel the difference?
- Real Tetris uses a fancier system called **SRS** with different kicks for every piece and rotation. Look it up if you like a challenge!

---

## Layer 7: Locking and clearing lines

**Goal:** pieces stay where they land, full rows disappear, and a new piece arrives each time. After this layer, it's a real game!

### Locking a piece

When a piece lands, we **copy it into the board**. From then on it's just part of the board data, and the falling piece is a brand new one.

```c
static void lock_piece(void)
{
    for (int i = 0; i < 4; i++) {
        Point c = piece_cell(&current, i);
        board[c.y][c.x] = current.type + 1;     // +1, remember?
    }

    clear_lines();

    if (!spawn_piece()) {
        /* No room: for now, wipe the board and carry on.
           Layer 9 adds a proper GAME OVER screen. */
        clear_board();
        spawn_piece();
    }
}
```

In `update_playing()`, replace the temporary "landed" code with:

```c
if (!try_move(0, 1))
    lock_piece();
```

### A new piece

```c
static bool spawn_piece(void)
{
    current.type = rand() % 7;
    current.rot = 0;
    current.x = (COLS - SHAPES[current.type].size) / 2;    // centred
    current.y = 0;
    fall_timer = 0;
    return fits(&current);
}
```

`rand()` gives a random number, and `% 7` turns it into 0–6. If the new piece doesn't fit, the stack has reached the top: **game over**. `spawn_piece` returns `false` so `lock_piece` knows.

> 🎲 **Random numbers.** Computers can't really be random. `rand()` follows a long sequence that *looks* random, starting from a "seed". Same seed means same sequence, so you'd get the same pieces every game! We seed it with `srand(rtc_ticks())` at the start of `main()`. `rtc_ticks()` is a clock that's always counting, so each run starts somewhere different.
>
> You'll need `#include <stdlib.h>` (for `rand`) and `#include <gint/rtc.h>` (for `rtc_ticks`) at the top.

```c
static void clear_board(void)
{
    for (int y = 0; y < ROWS; y++)
        for (int x = 0; x < COLS; x++)
            board[y][x] = 0;
}
```

### Clearing lines

Go through the rows from the bottom up. When a row is full, move every row above it down by one:

```
  before             after clearing row 18
  row 16  . . # .    row 16  . . . .     ← new empty row comes in at the top
  row 17  # . # #    row 17  . . # .
  row 18  # # # #    row 18  # . # #     ← everything above moved down
  row 19  # # . #    row 19  # # . #
```

```c
static int clear_lines(void)
{
    int cleared = 0;

    for (int y = ROWS - 1; y >= 0; y--) {
        bool full = true;
        for (int x = 0; x < COLS; x++)
            if (board[y][x] == 0)
                full = false;
        if (!full)
            continue;       // skip to the next row

        /* Shift every row above this one down by one */
        for (int yy = y; yy > 0; yy--)
            for (int x = 0; x < COLS; x++)
                board[yy][x] = board[yy - 1][x];
        for (int x = 0; x < COLS; x++)
            board[0][x] = 0;

        cleared++;
        y++;    // this row now holds what was above it: check it again
    }
    return cleared;
}
```

That sneaky `y++` at the end: after shifting, the row we just checked now holds what *was* the row above, which might be full too. Adding 1 cancels out the loop's `y--`, so we check the same row again. Without it, clearing two lines at once would miss the second one. (Try deleting it and see!)

Finally, delete the test blocks from `main()` and start the game with:

```c
srand(rtc_ticks());
spawn_piece();
```

### ✅ Checkpoint

Random pieces fall, stack up, and full rows vanish. When the stack reaches the top, the board wipes and you carry on. Compare with [steps/layer7.c](steps/layer7.c).

---

## Layer 8: Scoring, levels and fair pieces

**Goal:** a score, a level that makes the game speed up, a NEXT box, and fairer pieces.

### Score, lines and level

```c
static int score, lines, level;
```

Clearing more lines at once is worth much more, and higher levels multiply it. This is the classic Tetris scoring:

| Lines at once | Points |
|---|---|
| 1 | 40 × (level + 1) |
| 2 | 100 × (level + 1) |
| 3 | 300 × (level + 1) |
| 4 ("Tetris!") | 1200 × (level + 1) |

A lookup table makes this one line. Put it in `lock_piece()`, replacing the plain `clear_lines();` call:

```c
static const int points[5] = { 0, 40, 100, 300, 1200 };
int cleared = clear_lines();
score += points[cleared] * (level + 1);
lines += cleared;
level = lines / 10;         // a new level every 10 lines
```

(Integer division in C throws away the remainder, so 27 / 10 is 2.)

When the game restarts, set all three back to 0 as well.

Give a small bonus for soft dropping too, 1 point per row. In `update_playing()`:

```c
if (try_move(0, 1)) {
    if (soft_drop)
        score += 1;
}
else {
    lock_piece();
}
```

### Getting faster

Replace `FALL_DELAY` with a function that looks up how many frames to wait at each level:

```c
static int frames_per_row(void)
{
    static const int speed[] = {
        48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2,
    };
    int n = sizeof speed / sizeof speed[0];
    return speed[level < n ? level : n - 1];
}
```

Level 0 waits 48 frames (0.8 s) per row, and level 9 only 6 frames (0.1 s). After the end of the table it stays at the last value.

`a ? b : c` means "if a then b, otherwise c". Here it stops us reading past the end of the table, which would be one of those row-20 bugs again.

In `update_playing()`, change `int delay = FALL_DELAY;` to `int delay = frames_per_row();`.

> 💡 **Tuning with tables.** Putting numbers like this in a table makes the game easy to tweak. Want it harder? Change the numbers. You don't have to touch the logic.

### Fair pieces: the 7-bag

With `rand() % 7`, you can get four S pieces in a row, or wait 30 pieces for an I. Real Tetris uses a **bag**: put one of each of the 7 pieces in a bag, shake it, and hand them out in that order. When the bag's empty, refill it. You'll never wait more than 12 pieces for any shape.

```c
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
```

The shuffle works like shuffling cards by hand: pick a random card from the ones not yet placed and swap it to the end. Repeat with one fewer card each time. Every order is equally likely.

### The NEXT piece

Players like to see what's coming. Keep the next piece's type in a variable:

```c
static int next_type;
```

and change `spawn_piece()` so the current piece comes from `next_type`, and a new one is taken from the bag:

```c
current.type = next_type;
/* ... */
next_type = take_from_bag();
```

At the start, fill `next_type` before the first spawn:

```c
srand(rtc_ticks());
next_type = take_from_bag();
spawn_piece();
```

### Drawing the panels

Add positions for the left and right panels:

```c
#define LEFT_X   16
#define RIGHT_X  (BOARD_X + COLS * CELL + 16)
```

The stats go down the left side. `dprint` works like `printf`, so `%d` is replaced by the number:

```c
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
```

The NEXT box goes on the right. We draw the shape straight from `SHAPES`, without rotation, centred in a 4×4 box:

```c
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
```

`offset` centres the smaller boxes: a 3×3 shape gets moved in by half a cell, and the 2×2 O by a whole cell.

Call both from `draw()` (replacing the old `dtext(16, 14, ... "TETRIS")`):

```c
draw_stats();
draw_next();
```

### ✅ Checkpoint

The score goes up when you clear lines, the level goes up every 10 lines and the game speeds up, and NEXT shows the coming piece. Compare with [steps/layer8.c](steps/layer8.c).

---

## Layer 9: Game states and polish

**Goal:** a title screen, pause, a proper game over, hard drop, and a ghost piece showing where you'll land. This turns it from "working" into "fun".

### State machines

Right now the game is always "playing". A real game has several **screens**, and what the keys do depends on which one you're on. We keep track with a **state** variable:

```c
typedef enum {
    STATE_TITLE,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER,
} State;

static State state = STATE_TITLE;
```

An `enum` is a list of names for numbers (`STATE_TITLE` is 0, `STATE_PLAYING` is 1 and so on). Names are much easier to read than numbers.

Here's how the states connect:

```
                   EXE                      F1
  ┌─────────┐ ──────────▶ ┌──────────┐ ──────────▶ ┌────────┐
  │  TITLE  │             │ PLAYING  │             │ PAUSED │
  └─────────┘             └──────────┘ ◀────────── └────────┘
                             ▲    │          F1
                         EXE │    │ stack reaches the top
                             │    ▼
                          ┌───────────┐
                          │ GAME OVER │
                          └───────────┘
```

This is a **state machine**. Drawing one of these before you code is one of the best habits in game making.

In the main loop, replace `update_playing();` with a `switch` that does something different in each state:

```c
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
```

And at the top of `update_playing()`, add the way into the pause screen:

```c
if (keypressed(KEY_F1)) {
    state = STATE_PAUSED;
    return;
}
```

### Starting and ending a game

Everything that resets for a new game goes in one function:

```c
static void new_game(void)
{
    clear_board();
    score = 0;
    lines = 0;
    level = 0;
    bag_left = 0;

    srand(rtc_ticks());
    next_type = take_from_bag();
    spawn_piece();
    state = STATE_PLAYING;
}
```

Seeding here instead of at power-on is a nice touch. `rtc_ticks()` now depends on exactly when the player pressed EXE, which is never the same twice.

Remove the `srand / take_from_bag / spawn_piece` lines from `main()`, since the title screen does that now.

Keep a best score with `static int best;`. In `lock_piece()`, replace the "wipe the board and carry on" code with a real game over:

```c
if (!spawn_piece()) {
    state = STATE_GAME_OVER;
    if (score > best)
        best = score;
}
```

Show it under the other stats in `draw_stats()`:

```c
dtext(LEFT_X, 152, C_LIGHT, "BEST");
dprint(LEFT_X, 164, C_WHITE, "%d", best);
```

### Messages

A box across the middle of the board for the title, pause and game over screens. `dtext_opt` can centre text on a point:

```c
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
```

At the end of `draw()`, just before `dupdate()`:

```c
if (state == STATE_TITLE)
    draw_message("TETRIS", "Press EXE");
else if (state == STATE_PAUSED)
    draw_message("PAUSED", "F1: resume");
else if (state == STATE_GAME_OVER)
    draw_message("GAME OVER", "EXE: again");
```

Only draw the falling piece while there is one:

```c
if (state == STATE_PLAYING || state == STATE_PAUSED) {
    /* draw the piece here */
}
```

And in `draw_next()`, skip the piece on the title screen (there isn't one yet) by adding this just after the box is drawn:

```c
if (state == STATE_TITLE)
    return;
```

### Hard drop

EXE slams the piece straight down. Keep moving it down until it can't go further, then lock it. That's 2 points per row, for being brave:

```c
static void hard_drop(void)
{
    int rows = 0;
    while (try_move(0, 1))
        rows++;
    score += 2 * rows;
    lock_piece();
}
```

In `update_playing()`, after the rotation keys:

```c
if (keypressed(KEY_EXE)) {
    hard_drop();
    return;
}
```

### The ghost piece

An outline showing where the piece will land makes the game much easier to play. It uses the same "keep going down until it doesn't fit" idea, but on a copy, so the real piece doesn't move:

```c
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
```

Give `draw_piece` a second option to draw outlines only:

```c
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
```

Draw the ghost **first**, so the real piece is drawn on top when they overlap:

```c
Piece ghost = landing_spot();
draw_piece(&ghost, true);
draw_piece(&current, false);
```

### Showing the controls

A list on the right, under the NEXT box. Call `draw_controls()` from `draw()`:

```c
static void draw_controls(void)
{
    static char const *help[] = {
        "<- ->  move",
        "UP     rotate",
        "ALPHA  turn back",
        "DOWN   soft drop",
        "EXE    hard drop",
        "F1     pause",
        "EXIT   quit",
    };
    for (unsigned i = 0; i < sizeof help / sizeof help[0]; i++)
        dtext(RIGHT_X, 90 + 14 * i, C_DARK, help[i]);
}
```

### ✅ Checkpoint: you've built Tetris! 🎉

Title screen, play, pause, game over, and play again, with a ghost, hard drop and a best score. Compare with the finished game in [src/main.c](src/main.c). Its comments say which layer each part came from.

---

## Layer 10: Make your own game

Look back at what you built. Under all the Tetris details, it's made of the same few pieces that nearly every 2D game uses:

| # | Ingredient | In Tetris |
|---|---|---|
| 1 | **A game loop** at a steady speed | `main()` with the 60 FPS timer |
| 2 | **The world as data** | `board[][]`, `current` |
| 3 | **Drawing from the data**, every frame | `draw()` |
| 4 | **Input changes the data** | `update_playing()` |
| 5 | **Rules: try a change, keep it if allowed** | `fits()`, `try_move()` |
| 6 | **Timing by counting frames** | `fall_timer`, `key_repeat()` |
| 7 | **Things described as tables** | `SHAPES`, `points`, `speed` |
| 8 | **States** for different screens | `STATE_TITLE`, `STATE_PLAYING`... |

To make a new game, work out what each of those means for your game, then build it **in layers**, just like this guide: get something on screen, then make it move, then add rules, then scoring, then polish.

### A starting template

This is the skeleton every game in this repo can start from. It's Layers 1 and 9 with all the Tetris taken out:

```c
#include <stdbool.h>
#include <stdlib.h>
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/timer.h>
#include <gint/cpu.h>
#include <gint/rtc.h>

#define FPS 60

typedef enum { STATE_TITLE, STATE_PLAYING, STATE_GAME_OVER } State;
static State state = STATE_TITLE;

/* --- Your game's data goes here --- */
static int score;

static void new_game(void)
{
    srand(rtc_ticks());
    score = 0;
    /* set up your world */
    state = STATE_PLAYING;
}

static void update_playing(void)
{
    /* read keys, move things, check rules */
}

static void draw(void)
{
    dclear(C_BLACK);
    /* draw your world from its data */
    dprint(4, 4, C_WHITE, "Score: %d", score);

    if (state == STATE_TITLE)
        dtext_opt(DWIDTH / 2, DHEIGHT / 2, C_WHITE, C_NONE,
            DTEXT_CENTER, DTEXT_MIDDLE, "Press EXE", -1);
    else if (state == STATE_GAME_OVER)
        dtext_opt(DWIDTH / 2, DHEIGHT / 2, C_WHITE, C_NONE,
            DTEXT_CENTER, DTEXT_MIDDLE, "GAME OVER - EXE", -1);
    dupdate();
}

int main(void)
{
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

        if (state == STATE_PLAYING)
            update_playing();
        else if (keypressed(KEY_EXE))
            new_game();

        draw();
    }

    timer_stop(timer);
    return 0;
}
```

### Game ideas, and how to build them

Each of these is a good next project. Here's what you can reuse from Tetris and what's new.

**🐍 Snake** *(easiest next step)*
- **World:** a grid like Tetris's `board`, plus a list of the snake's body positions (`Point body[200]` and a `length`), a direction, and a food position.
- **Reuse:** grid drawing (Layer 2), timing by frames (Layer 5): the snake moves one cell every N frames, just like gravity.
- **New:** moving the body. Each step, every segment moves to where the one in front was, and the head moves in the current direction. Eating food makes `length` one bigger.
- **Rules:** hit a wall or yourself and it's game over. That's `fits()` again.

**🏓 Pong** and **🧱 Breakout**
- **World:** not a grid! The ball has a pixel position `x, y` and a speed `dx, dy`. Paddles and bricks are rectangles.
- **Reuse:** game loop, states, scoring, and Breakout's bricks can be a grid exactly like `board`.
- **New:** **velocity**. Every frame do `x += dx; y += dy;`. Hitting a wall flips the speed: `dx = -dx`.
- **New:** rectangle collision. Two boxes overlap if `ax1 <= bx2 && bx1 <= ax2 && ay1 <= by2 && by1 <= ay2`.
- **Two players** for Pong: one uses the arrows, the other uses two other keys.

**💣 Minesweeper**
- **World:** a grid where each cell knows: is it a mine? is it uncovered? is it flagged? how many mines are next to it? A struct per cell works well.
- **Reuse:** grid drawing, a cursor moved with `key_repeat` (Layer 4), random numbers to place mines, states.
- **New:** **flood fill**. Uncovering a cell with no mines nearby automatically uncovers its neighbours, and theirs, and so on. A function that calls itself (recursion) does this neatly.
- No timer pressure, so it's a good one for thinking about rules.

**🔢 2048**
- **World:** a 4×4 grid of numbers.
- **Reuse:** grid drawing (bigger cells, with `dprint` to put the number in each), `keypressed` for the four directions, random numbers for new tiles.
- **New:** sliding and merging. Write it for one direction first (say left, one row at a time), test it well, then think about how to reuse it for the others.

**👾 Space Invaders**
- **World:** the player's x position, a grid of aliens (alive or not) with one shared offset that moves the whole grid, and a list of bullets.
- **Reuse:** everything! Grid of aliens like `board`, `key_repeat` for moving, frame-counting timers for the aliens stepping sideways, states, levels getting faster like `frames_per_row()`.
- **New:** lots of moving things at once. Keep bullets in an array, with a flag saying which ones are in use.

**🧩 A Tetris cousin: Columns, Puyo Puyo, Dr. Mario**
- Copy this Tetris and change the rules: three-block columns that cycle colours instead of rotating, and matching 3 of a colour instead of full rows. Most of the code stays the same. That's the payoff of building in layers!

### Upgrades for your Tetris

Want to keep going with Tetris first? Try these, roughly easiest first:

1. **Colour themes**: change the `SHAPES` colours and the background.
2. **Prettier blocks**: draw a lighter line along the top and left of each block in `draw_block()` so they look 3D.
3. **Line clear flash**: when rows are full, make them flash for 20 frames before removing them. (Hint: add a new state, `STATE_CLEARING`.)
4. **Hold piece**: press a key to swap the current piece into a HOLD box, once per piece.
5. **Lock delay**: give the player half a second to slide a piece after it lands, instead of locking instantly.
6. **Start level**: let the player pick a level on the title screen with the arrow keys.
7. **SRS**: replace the simple wall kicks with the official rotation system.

---

## Troubleshooting

**"error: expected ';' before ..."**
You've missed a `;` at the end of the line *before* the one it mentions.

**"implicit declaration of function ..." or "... undeclared"**
Either it's misspelt, or you're using it *above* where it's written. Move the function higher up the file. Or you've forgotten an `#include` at the top (`rand` needs `<stdlib.h>`, `rtc_ticks` needs `<gint/rtc.h>`).

**"error: expected declaration or statement at end of input"**
A `{` without a matching `}`. Check your functions and loops are closed.

**Nothing appears on screen**
Did you call `dupdate()` at the end of `draw()`?

**The game is way too fast or too slow**
Check the timer delay is `1000000 / FPS` and that you reset `tick = 0` each frame.

**Things appear in the wrong place, or the game acts weird or crashes**
Look for a `board[x][y]` that should be `board[y][x]`, or a loop that goes one too far (`<=` where it should be `<`). Reading row 20 of a 20-row board is the classic one.

**A key does nothing**
Check you're using `keypressed` for one-off actions and `keydown` for held ones, and that it's after `clearevents()` in the loop. In the emulator, remember only the arrows, Shift and X/Space work from your keyboard. Click the on-screen buttons for the rest.

**It works in the emulator but not on the calculator (or the other way round)**
The emulator is very good but not perfect. Trust the real calculator.

---

## Where to learn more

- The comments in [src/main.c](src/main.c), and the gint header files, which list every function with an explanation. Inside the build image they're under `/root/.local/share/fxsdk/sysroot/sh3eb-elf/include/gint/`.
- [Planet Casio](https://www.planet-casio.com/): the community that makes gint. Mostly French, but very friendly.
- Read other people's game code. Then change something and see what happens. That's how every programmer learned.
