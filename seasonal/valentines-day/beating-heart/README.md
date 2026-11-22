# 💘 Beating Heart

*A Valentine's Day project.*

Design a pixel heart on paper, then code it into an animation. Cupid's arrow flies in and passes through a stack of hearts inside hearts. When it hits, the little heart in the middle starts beating and sends ripples out through the rings.

It comes in two versions, so use whichever device you have:

| | Casio fx-CG50 | BBC micro:bit |
|---|---|---|
| **Language** | C | Python (MicroPython) |
| **Code** | [casio/src/main.c](casio/src/main.c) | [microbit/main.py](microbit/main.py) |
| **Printable** | 📄 [beating-heart-casio.pdf](printable/beating-heart-casio.pdf) | 📄 [beating-heart-microbit.pdf](printable/beating-heart-microbit.pdf) |
| **Age** | About 10+ (side 1 alone works for younger children) | About 8+ |
| **Time** | 20 min to design, 45–60 min to type and run | 15 min to design, 30 min to type and run |
| **Coding ideas** | Pixel art as data, functions, drawing in layers, a frame timer | Brightness as numbers, animation frames, functions, radio |

Each printable is two A4 sides, ready to print double-sided. **Side 1** is a pencil-and-paper design activity with no computer needed. **Side 2** is the code, with numbered notes explaining each part, plus how to run it and ideas to go further.

## Casio fx-CG50 version

What you see on the 396 × 224 colour screen:

- Seven copies of your heart, each smaller than the last, drawn biggest first, so they nest inside each other.
- An arrow built from your tip and tail designs. It flies across, going *behind* the hearts on the left and *in front* on the right.
- When the arrow reaches the middle, the colours start shifting outward in ripples, and the small heart in the centre beats.

### Run it

In the emulator, from the top of the repo:

```bash
./run-emulator.sh seasonal/valentines-day/beating-heart/casio
```

On the calculator, either download `heart-g3a` from the latest **Build CG50 Games** run on GitHub's Actions tab, or build it yourself. Then copy `Heart.g3a` over.

| Key | Does |
|---|---|
| EXE | Fire the arrow again |
| EXIT | Stop |

### How the code works

[casio/src/main.c](casio/src/main.c) is exactly the code printed on the sheet.

1. **`HEART`, `TIP` and `TAIL`** are the designs: rows of text with `#` for a filled square.
2. **`draw()`** turns any design into squares at any position and size. Everything on screen is drawn by this one function.
3. **`heart()`** draws the heart centred on the screen at a given size, so the rings line up.
4. **A hardware timer** sets `tick` 30 times a second, and the loop waits for it. That keeps the speed the same whatever is being drawn. (The [Tetris guide](../../../games/tetris/README.md#layer-1-the-game-loop) explains this in more depth.)
5. **The painter's trick.** Things drawn later cover things drawn earlier. The order is: the back of the arrow, then the seven hearts, then the front of the arrow. That's what makes the arrow look like it goes *through* the heart.
6. **Ripples.** Ring `k` gets colour `colours[(k + wave) % 3]`. As `wave` goes up, every ring moves on to the next colour, so the pattern seems to flow outward.
7. **The beat.** `t % 20 < 4` is true for 4 ticks out of every 20, and the centre heart is drawn bigger during those ticks.

## micro:bit version

The 5 × 5 lights show a dim heart, and a three-light arrow (bright tip, fading tail) flies across the middle row and plays a little tune. Then the heart beats: first the little heart in the middle lights up, then the ring around it, a 5 × 5 version of the concentric hearts.

### Run it

1. Open **[python.microbit.org](https://python.microbit.org)** and paste in (or type) [microbit/main.py](microbit/main.py).
2. Press ▶ on the simulator to try it in the browser.
3. Plug in a micro:bit and click **Send to micro:bit**. Or click **Save** and drag the `.hex` file onto the `MICROBIT` drive.

| Button | Does |
|---|---|
| A | Fire the arrow again |
| B | Fire an arrow at a friend's micro:bit (by radio) |

Two micro:bits running the same code fire arrows at each other. The tune needs a micro:bit V2 (which has a speaker) or headphones on pin 0.

## Ideas to go further

- **Casio:** change the colours, the number of rings, or the arrow's speed. Make the heart beat faster after each arrow.
- **micro:bit:** add a third frame so the ripple goes further, or change the `sleep()` times for a racing heartbeat.
- **Both:** design a completely different shape, like a star or a smiley. On the Casio, change the `8, 9` (rows, columns) passed to `draw` if it isn't 9 × 8.
