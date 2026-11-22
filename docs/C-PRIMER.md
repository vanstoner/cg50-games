# C for Scratch and Python coders

All the games in this repo are written in **C**. It's one of the oldest programming languages still in everyday use (from 1972!), and it's still what people use when a program has to be small and fast and talk directly to hardware, like on a calculator.

If you've used **Scratch** or **Python**, you already know the big ideas: variables, loops, ifs, lists, functions. C has all of them. It just writes them differently, and it's much fussier. This guide shows each idea side by side in Scratch, Python and C, and explains the parts of C that have no Scratch or Python equivalent.

You don't need to read it all at once. Skim it, start the [Tetris guide](../games/tetris/README.md), and come back when you meet something strange.

**Contents**

1. [How C is different](#1-how-c-is-different)
2. [How a C program is built](#2-how-a-c-program-is-built-and-what-cmakeliststxt-is)
3. [The shape of a C file](#3-the-shape-of-a-c-file)
4. [Variables and types](#4-variables-and-types)
5. [Maths](#5-maths)
6. [If and else](#6-if-and-else)
7. [Loops](#7-loops)
8. [Functions](#8-functions)
9. [Arrays (C's lists)](#9-arrays-cs-lists)
10. [Text](#10-text)
11. [Structs: grouping things together](#11-structs-grouping-things-together)
12. [enum and switch](#12-enum-and-switch)
13. [#define and #include](#13-define-and-include)
14. [static and const](#14-static-and-const)
15. [Pointers: & * and ->](#15-pointers---and--)
16. [Things that bite](#16-things-that-bite)
17. [Cheat sheet](#17-cheat-sheet)

---

## 1. How C is different

| | Scratch | Python | C |
|---|---|---|---|
| **How it runs** | Click the green flag | `python game.py` runs it straight away | **Compiled** first into a file the calculator runs |
| **Mistakes** | Blocks only fit where they're allowed | Found when that line runs | Most are found by the compiler, **before** it runs |
| **Blocks of code** | Blocks inside blocks | Indentation | `{ curly braces }` |
| **End of a line** | — | Just a new line | A semicolon `;` |
| **Variable types** | Any variable holds anything | Any variable holds anything | You say what type each variable holds, and it can't change |
| **Lists** | Grow and shrink | Grow and shrink | Fixed size, decided in advance |
| **Safety** | Can't really break anything | Tells you about mistakes ("IndexError") | Trusts you completely. Mistakes can cause weird bugs or crashes. |

That last row is the big one. Python checks everything for you as it runs. C doesn't, because checking takes time and C is built for speed. In return, the **compiler** checks a lot before your program ever runs, and its error messages are your friend.

---

## 2. How a C program is built (and what CMakeLists.txt is)

In Python you write `game.py` and run it. In C there are a few more steps, because the calculator can't read C. It only understands **machine code**, the raw numbers its processor runs.

```
  src/main.c            your C code (text you can read)
      │
      │  compiler (sh-elf-gcc): turns C into machine code
      ▼
  build-cg/tetris       machine code
      │
      │  fxgxa: wraps it with a name and icon
      ▼
  Tetris.g3a            the add-in file you copy to the calculator
```

You don't run these steps by hand. `fxsdk build-cg` does them all, and `./run-emulator.sh` runs `fxsdk build-cg` for you.

### The files in a game folder

```
games/tetris/
  CMakeLists.txt     the recipe: how to build this game
  src/main.c         the game's code
  assets-cg/         pictures (the menu icon)
  build-cg/          made by the build; safe to delete, never edit
  Tetris.g3a         the finished add-in
```

### CMakeLists.txt, line by line

**CMake** is a tool that reads a recipe file, called `CMakeLists.txt`, and works out how to build your program. Here's Tetris's:

```cmake
cmake_minimum_required(VERSION 3.15)      # which version of CMake this needs
project(Tetris C)                         # the project's name, written in C

include(GenerateG3A)                      # load fxSDK's "make a .g3a" tool
find_package(Gint 2.9 REQUIRED)           # we use gint, version 2.9 or newer

add_executable(tetris src/main.c)         # build a program called "tetris" from main.c
target_compile_options(tetris PRIVATE -Wall -Wextra -Os)   # compiler settings (below)
target_link_libraries(tetris Gint::Gint)  # and include gint in it

if("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)   # when building for the CG50...
  generate_g3a(                                 # ...wrap it into a .g3a:
    TARGET tetris                               #   from the "tetris" program
    OUTPUT "Tetris.g3a"                         #   the file to make
    NAME "Tetris"                               #   the name shown in MENU
    ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png   # the icons
  )
endif()
```

The compiler settings mean:
- `-Wall -Wextra`: warn me about **all** the suspicious things you can spot, and then some **extra**. Warnings aren't errors, but they're usually bugs. Fix them!
- `-Os`: make the program small (**O**ptimise for **s**ize).

**When you'd change it:**
- Your game's name goes in `project()`, `NAME` and `OUTPUT`.
- If you split your code into more files, list them all: `add_executable(snake src/main.c src/snake.c src/food.c)`.

---

## 3. The shape of a C file

Every C program in this repo has the same overall shape:

```c
/* 1. Includes: bring in other people's code (like Python's import) */
#include <stdbool.h>
#include <gint/display.h>

/* 2. Constants */
#define COLS 10

/* 3. Variables that the whole program shares */
static int score = 0;

/* 4. Functions */
static void draw(void)
{
    dclear(C_BLACK);
    dupdate();
}

/* 5. main(): where the program starts */
int main(void)
{
    draw();
    return 0;
}
```

In Python, a file runs from top to bottom. In C, **nothing runs except `main()`**. Everything else just gets defined, waiting to be called. `main()` is like Scratch's "when green flag clicked" hat block: it's where things start, and when it finishes, the program ends.

`/* like this */` and `// like this` are **comments** (Python uses `#`). The computer ignores them.

---

## 4. Variables and types

In Scratch and Python, any variable can hold anything. In C, you say what **type** of thing a variable holds when you create it (that's called **declaring** it), and it stays that type forever:

| Scratch | Python | C |
|---|---|---|
| make a variable `score`, set to 0 | `score = 0` | `int score = 0;` |
| set `alive` to true | `alive = True` | `bool alive = true;` |
| set `speed` to 1.5 | `speed = 1.5` | `float speed = 1.5;` |
| set `name` to Rob | `name = "Rob"` | `char const *name = "Rob";` |

The types you'll use most:

| Type | Holds | Examples |
|---|---|---|
| `int` | whole numbers, positive or negative | `0`, `42`, `-7` |
| `bool` | true or false (needs `#include <stdbool.h>`) | `true`, `false` |
| `float` | numbers with a decimal point | `1.5`, `-0.25` |
| `char` | a single character | `'A'` (single quotes!) |
| `char const *` | a piece of text (see [Text](#10-text)) | `"Hello"` (double quotes) |

After it's declared, you use a variable without the type:

```c
int score = 0;      // declare it (once)
score = 10;         // change it
score = score + 5;  // now 15
```

**You can't skip declaring.** In Python, `scroe = 5` (typo) quietly creates a new variable. In C it's an error: `'scroe' undeclared`. That's C catching your typo for you.

> 💡 **Why bother with types?** The compiler uses them to make your program fast and small, and to catch mistakes like trying to do maths on text.

---

## 5. Maths

Mostly the same as Python, with a few surprises:

| | Python | C |
|---|---|---|
| Add, subtract, multiply | `+ - *` | `+ - *` |
| Divide | `7 / 2` → `3.5` | `7 / 2` → **`3`** (whole numbers stay whole) |
| Whole-number divide | `7 // 2` → `3` | `7 / 2` → `3` |
| Remainder | `7 % 2` → `1` | `7 % 2` → `1` |
| Add 1 | `x += 1` | `x += 1;` or `x++;` |
| Take 1 away | `x -= 1` | `x -= 1;` or `x--;` |
| Power | `2 ** 3` | No operator. Use `2 * 2 * 2` |

⚠️ **Dividing two `int`s throws the remainder away.** `7 / 2` is 3, and `1 / 2` is 0! The Tetris guide uses this on purpose: `level = lines / 10;` gives the level as a whole number.

`x++` (Scratch: "change x by 1") is so common in C that it has its own symbol. You'll see it everywhere, especially in loops. `++x` also adds 1. The difference only matters inside bigger expressions, like `if (++fall_timer >= 48)`, which adds 1 first and then compares.

---

## 6. If and else

| Scratch | Python | C |
|---|---|---|
| if ⟨ score > 10 ⟩ then | `if score > 10:` | `if (score > 10) {` |
| else | `else:` | `} else {` |
| ⟨ ⟩ and ⟨ ⟩ | `and` | `&&` |
| ⟨ ⟩ or ⟨ ⟩ | `or` | `\|\|` |
| not ⟨ ⟩ | `not` | `!` |
| = (equals) | `==` | `==` |
| ≠ | `!=` | `!=` |

```python
# Python
if lives == 0:
    game_over = True
elif lives < 3 and not shield:
    warning = True
else:
    warning = False
```

```c
// C
if (lives == 0) {
    game_over = true;
} else if (lives < 3 && !shield) {
    warning = true;
} else {
    warning = false;
}
```

The differences:
- The condition always goes in **round brackets** `( )`.
- The code to run goes in **curly braces** `{ }`. Indentation is just to make it readable; C ignores it.
- It's `else if`, not `elif`.

If there's only **one** line inside, you can leave the braces off. The Tetris code does this a lot:

```c
if (keypressed(KEY_EXIT))
    break;
```

⚠️ **`=` is not `==`.** `if (lives = 0)` *sets* lives to 0 (and counts as false). This is legal C, so it won't always be caught! Luckily `-Wall` usually warns you.

---

## 7. Loops

### Repeat forever

| Scratch | Python | C |
|---|---|---|
| forever | `while True:` | `while (true) {` ... `}` |

### Repeat until / while

| Scratch | Python | C |
|---|---|---|
| repeat until ⟨ done ⟩ | `while not done:` | `while (!done) {` ... `}` |

### Repeat N times: the `for` loop

This is the one that looks strangest at first:

| Scratch | Python | C |
|---|---|---|
| repeat (10) | `for i in range(10):` | `for (int i = 0; i < 10; i++) {` |

The C version has three parts inside the brackets, separated by semicolons:

```c
for (int i = 0;   i < 10;   i++) {
//   ─────────    ──────    ───
//   start:       keep      after each
//   make i = 0   going     time round:
//                while     add 1 to i
//                i < 10
    dprint(10, 10 + i * 12, C_WHITE, "Line %d", i);
}
```

So `i` goes 0, 1, 2 ... 9, just like Python's `range(10)`. Other examples:

| Python | C |
|---|---|
| `for i in range(10):` | `for (int i = 0; i < 10; i++)` |
| `for i in range(2, 8):` | `for (int i = 2; i < 8; i++)` |
| `for i in range(10, 0, -1):` (count down) | `for (int i = 10; i > 0; i--)` |
| `for i in range(0, 100, 5):` (step 5) | `for (int i = 0; i < 100; i += 5)` |

C has no `for item in my_list:`. You loop over the positions instead (see [Arrays](#9-arrays-cs-lists)).

### Leaving a loop early

`break` jumps out of the loop (same as Python). `continue` skips to the next time round (same as Python). Scratch has "stop this script", which is a bit like `break`.

---

## 8. Functions

In Scratch you make your own block. In Python, `def`. In C, you write the **type of answer** it gives back, then its name, then the type of each input:

```python
# Python
def add(a, b):
    return a + b

def say_hi():
    print("hi")
```

```c
// C
int add(int a, int b)       // takes two ints, gives back an int
{
    return a + b;
}

void say_hi(void)           // takes nothing, gives back nothing
{
    /* ... */
}
```

- **`void`** means "nothing". `void say_hi(void)` takes no inputs and returns no answer.
- **Order matters.** A function must be written *above* any code that calls it. (Python is relaxed about this, C is not.) If the compiler says `implicit declaration of function`, the function is below where you used it, or misspelt.
- **One answer only.** A C function returns at most one value. To hand back several, return a struct (see [Structs](#11-structs-grouping-things-together)), or use pointers (see [Pointers](#15-pointers---and--)).

---

## 9. Arrays (C's lists)

C's version of a list is called an **array**. It's much simpler than a Python list or a Scratch list, and that's both its strength and its danger.

| Scratch | Python | C |
|---|---|---|
| make a list `scores` | `scores = [0, 0, 0, 0, 0]` | `int scores[5] = {0};` |
| item (1) of `scores` | `scores[0]` | `scores[0]` |
| replace item (1) of `scores` with 50 | `scores[0] = 50` | `scores[0] = 50;` |
| length of `scores` | `len(scores)` | No such thing! You remember it's 5. |
| add 7 to `scores` | `scores.append(7)` | Can't. The size is fixed. |

The rules:

- **The size is fixed when you create it.** `int scores[5];` makes exactly 5 ints, forever. If you don't know how many you'll need (like bullets in Space Invaders), make it big enough for the most you'll ever have, and keep a separate count of how many you're using.
- **Every item is the same type.** An array of `int` holds only ints.
- **Counting starts at 0.** `scores[5]` has items `scores[0]` to `scores[4]`. (Scratch counts from 1, Python and C from 0.)
- **Nothing stops you going past the end.** Python says `IndexError` if you use `scores[5]`. C doesn't check: it quietly reads or writes whatever memory comes next, which might be another variable! The result is bugs that seem to make no sense. **Always make sure your positions are in range.**

Looping over an array:

```python
# Python
total = 0
for s in scores:
    total += s
```

```c
// C
int total = 0;
for (int i = 0; i < 5; i++)
    total += scores[i];
```

Rather than typing `5` in several places, give it a name with `#define` (see [#define](#13-define-and-include)), or ask how big the array is with `sizeof scores / sizeof scores[0]`. That trick is in the Tetris code: `sizeof` gives a size in bytes, so dividing the size of the whole array by the size of one item gives the number of items.

### Grids: 2D arrays

A grid is an array of rows:

```c
int board[20][10];      // 20 rows, each 10 wide
board[3][7] = 1;        // row 3, column 7
```

It's like a Python list of lists (`board[3][7]`), but fixed size. Since it's `board[row][column]`, that's `board[y][x]`. **y comes first**, which catches everyone out at least once.

### Filling an array when you create it

```c
int speeds[5] = { 48, 43, 38, 33, 28 };    // like Python's [48, 43, 38, 33, 28]
int empty[100] = {0};                       // all 100 set to 0
```

---

## 10. Text

C has no real text type. Text is an array of `char` (characters) with a hidden 0 at the end to mark where it stops. The good news: for games you rarely need to do anything with text except **show** it, and that's easy:

```c
dtext(10, 10, C_WHITE, "Hello!");
```

To show numbers inside text, use `dprint`. It works like Python's `%` formatting, or a fill-in-the-blanks:

```python
# Python
f"Score: {score}  Lives: {lives}"
```

```c
// C (gint)
dprint(10, 10, C_WHITE, "Score: %d  Lives: %d", score, lives);
```

Each `%d` is replaced by the next number in the list. (`%d` means "a whole number in decimal".)

Other differences:
- `"double quotes"` are text; `'A'` (single quotes) is one character. They aren't interchangeable like in Python.
- You can't join text with `+`, and you can't compare it with `==`. In games, you'll almost never need to.
- To keep a piece of text in a variable, use `char const *name = "Rob";`. The `*` is explained in [Pointers](#15-pointers---and--).

---

## 11. Structs: grouping things together

In Scratch, each sprite has its own x, y, costume and so on. In Python you might use a dictionary or a class. In C, you make a **struct**: a new type that bundles several variables together.

```python
# Python
player = {"x": 10, "y": 20, "lives": 3}
player["x"] += 1
```

```c
// C
typedef struct {
    int x, y;
    int lives;
} Player;                           // we've invented a new type called Player

Player player = { 10, 20, 3 };      // x=10, y=20, lives=3
player.x += 1;                      // use a dot to get at the parts
```

- `typedef struct { ... } Player;` describes the type. It doesn't make one yet: it's a blueprint, like a Python class.
- `Player player = ...;` makes an actual one.
- `player.x` is "the x part of player".

You can name the parts when you fill one in, which is clearer:

```c
Player player = { .x = 10, .y = 20, .lives = 3 };
```

And you can make arrays of structs, which is perfect for lots of similar things:

```c
Player enemies[8];          // 8 enemies, each with its own x, y and lives
enemies[2].x = 50;
```

Unlike a Python dictionary, you can't add new parts later. A struct has exactly the parts you declared.

---

## 12. enum and switch

An **enum** gives names to a set of choices. It's much easier to read `STATE_PAUSED` than to remember that "state 2 means paused":

```c
typedef enum {
    STATE_TITLE,        // = 0
    STATE_PLAYING,      // = 1
    STATE_PAUSED,       // = 2
    STATE_GAME_OVER,    // = 3
} State;

State state = STATE_TITLE;
```

**switch** is a neat way of writing "if it's this, do that; if it's that, do this":

```c
switch (state) {
case STATE_TITLE:
    /* ... */
    break;
case STATE_PLAYING:
    /* ... */
    break;
default:            // anything else
    break;
}
```

⚠️ Don't forget `break` at the end of each case. Without it, C carries straight on into the next case's code. That's occasionally useful (Tetris uses it to make TITLE and GAME_OVER share code), but usually it's a bug.

Python 3.10 has a similar `match`/`case`. Scratch doesn't have one; you'd use a stack of if blocks.

---

## 13. #define and #include

Lines starting with `#` are handled **before** the real compiling starts, by the **preprocessor**. Think of it as an automatic find-and-replace that runs first.

### #include: using other code

```c
#include <gint/display.h>
```

This is like Python's `import`. It pulls in the descriptions of a library's functions so you can use them. The `.h` files are called **headers**.

| You want | Include |
|---|---|
| `true`, `false`, `bool` | `<stdbool.h>` |
| `rand()`, `srand()` | `<stdlib.h>` |
| Drawing: `dclear`, `drect`, `dtext`... | `<gint/display.h>` |
| Keys: `keypressed`, `keydown`... | `<gint/keyboard.h>` |
| Timers | `<gint/timer.h>` |
| `sleep()` | `<gint/cpu.h>` |
| `rtc_ticks()` (a clock) | `<gint/rtc.h>` |
| `gint_setrestart`, `gint_osmenu` | `<gint/gint.h>` |

### #define: names for numbers

```c
#define COLS 10
#define CELL 10
#define BOARD_WIDTH (COLS * CELL)
```

Everywhere you write `COLS`, the preprocessor swaps in `10` before compiling. Use these for any number that means something, so you can change it in one place and so the code explains itself. `board[ROWS][COLS]` says a lot more than `board[20][10]`.

- **No `=` and no `;`.** It's `#define COLS 10`, not `#define COLS = 10;`. It's a find-and-replace, so those would be pasted in too!
- Put brackets around calculations, `(COLS * CELL)`, so they can't get mixed up with the maths around them.
- By tradition `#define` names are in CAPITALS.

---

## 14. static and const

You'll see these words a lot in the Tetris code.

**`const`** means "this never changes". If you try to change it, the compiler stops you:

```c
const int MAX_LIVES = 3;
static const Shape SHAPES[7] = { ... };    // a table that's fixed forever
```

**`static`** means two different things depending on where it is:

1. **Outside a function** (on a shared variable or a function): "only this file can use it". In a one-file game it changes nothing, so you can ignore it. It's just good habit.

2. **Inside a function**: "keep this variable's value between calls". Normally a variable made inside a function is created fresh every time the function is called and thrown away when it finishes, just like a variable inside a Python `def`. `static` makes one that remembers:

```c
void count_presses(void)
{
    static int presses = 0;     // set to 0 only once, at the very start
    presses++;                  // 1, then 2, then 3... on each call
}
```

---

## 15. Pointers: & * and ->

This is the part of C that has **no** equivalent in Scratch or Python, so take it slowly. You only need a little of it for these games.

### The idea

Every variable lives somewhere in the computer's memory, and each place in memory has a number, its **address**, like a house number on a street. A **pointer** is a variable that holds an address: it says where to *find* something, rather than holding the thing itself.

```
  memory:   ┌──────────┐
  address   │  score   │
  1000  ──▶ │    42    │
            └──────────┘

  int score = 42;       score holds 42
  int *where = &score;  where holds 1000 ("the address of score")
```

- **`&score`** means "the address of `score`" ("where to find it").
- **`int *where`** declares a pointer that holds the address of an int.
- **`*where`** means "the thing at that address". So `*where = 50;` changes `score` to 50.

### Why would you want that?

When you pass a variable to a function, C gives the function a **copy**. Changing the copy doesn't change the original:

```c
void add_one(int n)
{
    n++;                // changes the copy only
}

int lives = 3;
add_one(lives);         // lives is still 3!
```

If you pass the **address** instead, the function can reach the original:

```c
void add_one(int *n)
{
    (*n)++;             // changes the thing n points at
}

int lives = 3;
add_one(&lives);        // lives is now 4
```

That's exactly what Tetris's `key_repeat(KEY_LEFT, &left_held)` does: each key has its own counter, and the function updates the right one.

### -> for structs

When you have a pointer to a struct, you get at its parts with `->` instead of `.`:

```c
Piece current;
Piece *p = &current;

current.x = 3;      // the struct itself: use a dot
p->x = 3;           // a pointer to the struct: use an arrow (same thing)
```

The Tetris functions take `Piece const *p` ("where to find a Piece, which I promise not to change") rather than a copy of the Piece. That's partly habit and partly speed, since it saves copying the whole struct.

### What you actually need to remember

| You see | It means |
|---|---|
| `&thing` | where to find `thing` |
| `Type *p` (in a declaration) | `p` holds where to find a `Type` |
| `*p` (in normal code) | the thing `p` points at |
| `p->part` | the `part` of the struct `p` points at (same as `(*p).part`) |

If pointers feel confusing, that's completely normal. You can get a long way just copying the patterns in the Tetris code.

---

## 16. Things that bite

The mistakes everyone makes, and how to spot them:

| Mistake | What happens | Fix |
|---|---|---|
| Forgetting a `;` | Error on the **next** line | Look at the end of the line *before* the error |
| A `{` without a `}` | Error at the very end of the file | Check every function, if and loop is closed |
| `=` instead of `==` in an `if` | It *sets* the value, and the if behaves strangely | Use `==` to compare. Read the compiler's warnings |
| Going past the end of an array | Weird bugs, other variables changing by themselves, crashes | Check positions are `>= 0` and `< size` before using them |
| `board[x][y]` instead of `board[y][x]` | Things appear in the wrong place, or bugs as above | Row first: `board[y][x]` |
| `<=` instead of `<` in a loop | Goes one past the end of the array | `for (i = 0; i < SIZE; i++)` |
| `7 / 2` expecting 3.5 | You get 3 | Expected for whole numbers. Use `float` if you need the fraction |
| Using a function above where it's written | `implicit declaration of function` | Move the function higher up the file |
| `#define X = 5;` | Strange errors wherever you use `X` | `#define X 5` with no `=` and no `;` |
| Forgetting `break` in a `switch` | Runs into the next case too | End each case with `break;` |

> 💡 **Read the first error first.** One mistake (like a missing `;`) can cause a long list of errors after it. Fix the first one, rebuild, and many of the others usually vanish.

---

## 17. Cheat sheet

| Idea | Scratch | Python | C |
|---|---|---|---|
| Comment | (comment block) | `# note` | `// note` or `/* note */` |
| New variable | make a variable | `x = 0` | `int x = 0;` |
| Change variable | change x by 1 | `x += 1` | `x++;` or `x += 1;` |
| True / false | ⟨ ⟩ blocks | `True` / `False` | `true` / `false` (`<stdbool.h>`) |
| If | if ⟨ ⟩ then | `if x > 3:` | `if (x > 3) { }` |
| Else if | (nested if/else) | `elif` | `else if` |
| And / or / not | ⟨ and ⟩ ⟨ or ⟩ ⟨ not ⟩ | `and` `or` `not` | `&&` `\|\|` `!` |
| Forever | forever | `while True:` | `while (true) { }` |
| Repeat 10 | repeat (10) | `for i in range(10):` | `for (int i = 0; i < 10; i++) { }` |
| Repeat until | repeat until ⟨ ⟩ | `while not done:` | `while (!done) { }` |
| List | make a list | `a = [1, 2, 3]` | `int a[3] = {1, 2, 3};` |
| List item | item (1) of a | `a[0]` | `a[0]` |
| List length | length of a | `len(a)` | (you track it yourself) |
| Function | make a block | `def f(x):` | `int f(int x) { }` |
| Return | — | `return x` | `return x;` |
| Text with numbers | join | `f"Score {s}"` | `dprint(x, y, C_WHITE, "Score %d", s);` |
| Group of values | sprite's variables | dict / class | `struct` |
| Named choices | — | `Enum` | `enum` |
| Import | (extensions) | `import x` | `#include <x.h>` |
| Constant | — | `COLS = 10` (by habit) | `#define COLS 10` |
| Random number | pick random 1 to 10 | `random.randint(1, 10)` | `rand() % 10 + 1` (`<stdlib.h>`) |
| Start of program | when green flag clicked | top of the file | `int main(void) { }` |

Ready? Head to the [Tetris guide](../games/tetris/README.md) and build something!
