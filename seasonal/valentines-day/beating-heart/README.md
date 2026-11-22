# 💗 Beating Heart

*A Valentine's Day project.*

Design a pixel heart on paper, then turn it into a program that draws it on the Casio fx-CG50. The arrow keys make it grow and shrink, and F1/F2 change its colour.

| | |
|---|---|
| **Age** | About 8+. The design part on its own (side 1) works for younger children. |
| **Time** | 15 minutes to design, 30–45 minutes to type in and run |
| **Coding idea** | Pixel art as **data**: a list of rows like `".##...##."` becomes a picture. Two loops (one inside the other) visit every square. |
| **You'll need** | Coloured pencils · Casio fx-CG50 and its USB cable · a computer with the build tools (see the [main README](../../../README.md)) |

## The printable

📄 **[beating-heart.pdf](printable/beating-heart.pdf)**: two A4 sides, which you can print double-sided.

- **Side 1 – Design it:** colour a heart on a 9×8 grid, write each row as `#` and `.`, and learn what pixels are. No computer needed.
- **Side 2 – Code it:** the program, with numbered notes explaining each part, how to put it on the calculator, and challenges.

To change the printable, edit [printable/beating-heart.html](printable/beating-heart.html) and rebuild the PDF:

```bash
seasonal/printable-kit/make-pdf.sh seasonal/valentines-day/beating-heart/printable/beating-heart.html
```

## Run it

In the emulator:

```bash
./run-emulator.sh seasonal/valentines-day/beating-heart
```

On the calculator: download `heart-g3a` from the latest **Build CG50 Games** run on GitHub's Actions tab, or build it yourself, then copy `Heart.g3a` over.

| Key | Does |
|---|---|
| ▲ / ▼ | Heart bigger / smaller |
| F1 / F2 | Red / pink |
| EXIT | Stop |

## How the code works

[src/main.c](src/main.c) is exactly the code printed on the sheet.

1. **`HEART[]`** is the design: 8 rows of text, `#` for a filled square.
2. **`size`** is how many pixels wide each square is, and **`colour`** is what colour to draw it.
3. **`while (1)`** repeats forever: clear the screen, draw, then wait for a key.
4. **`left` and `top`** work out where to start drawing so the heart sits in the middle of the 396×224 screen, whatever its size.
5. **Two `for` loops** visit every row and every column. Where there's a `#`, `drect` draws a square. (Each square is drawn 1 pixel smaller than its spacing, so you can see the grid.)
6. **`getkey()`** waits for a key press, and the `if`s change `size` or `colour`. The `size < 24` and `size > 4` checks stop the heart getting too big for the screen or shrinking to nothing.

## Ideas to go further

- Draw a different design: a star, a ghost, your initials. If it isn't 9×8, change the `9` and `8` in the code too.
- Mix your own colour with `C_RGB(red, green, blue)`. Each number is 0–31.
- Make it beat **by itself**. This needs the game loop from the [Tetris guide, Layer 1](../../../games/tetris/README.md#layer-1-the-game-loop) instead of `getkey()`, which waits for a key.
