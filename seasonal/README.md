# Seasonal projects

A coding project for celebrations through the year. Each one is small enough to do in an afternoon, and each comes in two parts:

- **A printable** (1–2 A4 sides) with a hands-on, no-computer activity: design something on paper, which is half the fun.
- **The code** that brings the design to life. A project can have versions for different devices and languages, such as the Casio fx-CG50 in C or the BBC micro:bit in Python, each with its own printable.

Each project also teaches one coding idea. Do them in any order, but they get gradually trickier through the year.

## The calendar

Some festivals follow the moon, so their dates **move each year**. Check this year's date before you plan.

| Month | Event | Project | Coding idea | Status |
|---|---|---|---|---|
| January | [New Year](new-year/) | Countdown Clock | Timers, big digits | Planned |
| January – February | [Winter Fun](winter-fun/) | Snowflake Maker | Symmetry | Planned |
| Jan – Feb *(moves)* | [Lunar New Year](lunar-new-year/) | Dragon Dance | Arrays that follow | Planned |
| 14 February | [Valentine's Day](valentines-day/) | **[Beating Heart](valentines-day/beating-heart/)** | Pixel art as data, drawing in layers, animation | ✅ **Ready** (Casio, micro:bit) |
| 14 March | [Pi Day](pi-day/) | Raining Pi | Random numbers, maths | Planned |
| *(moves)* | [Ramadan](ramadan/) | Crescent Moon Tracker | Drawing with circles | Planned |
| *(moves)* | [Eid](eid/) | Eid Star Patterns | Geometry, rotation | Planned |
| March *(moves)* | [Holi](holi/) | Colour Splash | Mixing colours, randomness | Planned |
| UK March / US May | [Mother's Day](mothers-day/) | Flower for Mum | Loops and angles | Planned |
| March – April *(moves)* | [Easter](easter/) | Egg Decorator | Patterns with `%` | Planned |
| 22 April | [Earth Day](earth-day/) | Grow a Forest | Growth, branching | Planned |
| June – August | [Rainy Day Summer](rainy-day-summer/) | **[Rainy Day Dice](rainy-day-summer/rainy-day-dice/)** | Random numbers, lists, True/False | ✅ **Ready** (micro:bit) |
| June – August | [Rainy Day Summer](rainy-day-summer/) | Raindrop Catcher | Falling objects, collision | Planned |
| September | [Back to School](back-to-school/) | Times Table Racer | Number input, scoring | Planned |
| 31 October | [Halloween](halloween/) | Pumpkin Carver | Pixel art, animation | Planned |
| Oct – Nov *(moves)* | [Diwali](diwali/) | Rangoli Lights | Rotational symmetry | Planned |
| 5 November | [Bonfire Night](bonfire-night/) | Firework Show | Particles, gravity | Planned |
| 4th Thursday of Nov | [Thanksgiving](thanksgiving/) | Thankful Turkey | Lists of text | Planned |
| Nov – Dec *(moves)* | [Hanukkah](hanukkah/) | Menorah Lights | Counting loops | Planned |
| 25 December | [Christmas](christmas/) | Twinkly Christmas Tree | Random animation | Planned |

## How each project is laid out

```
seasonal/
  README.md                        this calendar
  printable-kit/                   shared look and tools for every printable
    theme.css
    make-pdf.sh                    printable HTML -> A4 PDF
    code-to-html.py                puts real source code onto a printable
  valentines-day/
    README.md                      the event, and its project(s)
    beating-heart/                 one project
      README.md                    what it is, and how to run each version
      casio/                       Casio fx-CG50 version (C)
        CMakeLists.txt
        src/main.c
      microbit/                    BBC micro:bit version (MicroPython)
        main.py
      printable/
        beating-heart-casio.html   one printable per version (edit these)
        beating-heart-casio.pdf    ready to print
        beating-heart-microbit.html
        beating-heart-microbit.pdf
```

Each version of a project lives in a folder named after the device:

| Folder | Device | Language | How to run it |
|---|---|---|---|
| `casio/` | Casio fx-CG50 | C (gint) | `./run-emulator.sh`, or the `.g3a` from GitHub Actions |
| `microbit/` | BBC micro:bit | MicroPython | [python.microbit.org](https://python.microbit.org) |

A project doesn't need every version. To add a new kind of output (Scratch, desktop Python, MakeCode...), give it a new folder name and add it to this table.

An event can have more than one project. Just add another folder next to `beating-heart/`.

## Trying a project

From the top folder of the repo:

```bash
./run-emulator.sh seasonal/valentines-day/beating-heart/casio
```

The GitHub build also compiles every Casio version. Download its `.g3a` from the **Actions** tab, and copy it to the calculator as described in the [main README](../README.md#getting-a-game-onto-the-calculator).

For a micro:bit version, paste its `main.py` into [python.microbit.org](https://python.microbit.org). It has a simulator, and a button to send the code to a plugged-in micro:bit. New to the micro:bit? Read the next section first.

## First time with a micro:bit?

You don't need to install anything. The editor runs in a web browser, on a Mac, Windows PC or Chromebook.

1. **Use Chrome or Edge.** "Send to micro:bit" talks to the board over USB from the browser, which Safari and Firefox can't do. (The Save-and-drag method in step 6 works in any browser.)
2. **Use a USB data cable.** Some micro-USB cables only charge and carry no data. If the computer doesn't see the micro:bit, try another cable.
3. **Plug it in.** A drive called **MICROBIT** appears (in Finder on a Mac). That's how you know the cable works.
4. **Open [python.microbit.org](https://python.microbit.org)**, delete the starter code, and paste in the project's `main.py`. (On GitHub, open the file and use the copy button.)
5. **Try it in the simulator** first: press ▶ on the pretend micro:bit.
6. **Put it on the micro:bit:**
   - Click **Send to micro:bit**, pick your micro:bit in the browser's pop-up, and click **Connect**. The light on the back flashes while it copies.
   - **Or** click **Save** to download a `.hex` file, then drag it onto the **MICROBIT** drive. The drive disappears and comes back afterwards, which is normal.
7. **Unplug it** and run it from a battery pack if you like.

**Is mine a V1 or a V2?** A V2 says "V2" or has a gold microphone symbol on the back. It has a built-in speaker, so it can play sounds. A V1 is silent unless you connect headphones to pin 0 and GND. Everything else in these projects works on both.

**If Send to micro:bit won't connect,** the editor may ask you to [update the micro:bit's firmware](https://microbit.org/get-started/user-guide/firmware/) (the small program that handles USB). Follow its steps, or just use Save and drag, which always works.

More help: microbit.org's [getting started guide](https://microbit.org/get-started/getting-started/introduction/) and [Python editor guide](https://microbit.org/get-started/user-guide/python-editor/).

## Adding a project

1. **Copy the example:** `cp -r seasonal/valentines-day/beating-heart seasonal/<event>/<your-project>`, and delete any version folders you don't need.
2. **Casio version:** rename it in `casio/CMakeLists.txt` (`project`, `add_executable`, `TARGET`, `OUTPUT`, `NAME`), write `casio/src/main.c`, and test it with `./run-emulator.sh seasonal/<event>/<your-project>/casio`.
3. **micro:bit version:** write `microbit/main.py` and test it in the [python.microbit.org](https://python.microbit.org) simulator.
4. **Make a printable for each version:** edit `printable/<your-project>-<device>.html`, using the shared classes from [printable-kit/theme.css](printable-kit/theme.css). Keep it to **one or two A4 sides**.
   - Put the real code on it with `code-to-html.py`, so the sheet always matches the source. In the HTML, mark where the code goes with `<!--CODE--><!--/CODE-->`, then:
     ```bash
     python3 seasonal/printable-kit/code-to-html.py <source> <printable.html> c|py CODE=<first>-<last line> --anno 1=<line> 2=<line> --hl <first>-<last>
     ```
     `--anno` puts numbered markers on lines, to explain beside the code. `--hl` shades the lines kids should change (their designs). Use two markers, such as `CODE_A` and `CODE_B`, to split long code into two columns.
   - Then make the PDF. It tells you how many pages it came out as:
     ```bash
     seasonal/printable-kit/make-pdf.sh seasonal/<event>/<your-project>/printable/<name>.html
     ```
5. **Add Casio versions to the build:** add a line to the `include:` list in [.github/workflows/build-games.yml](../.github/workflows/build-games.yml).
6. **Update the tables:** mark it ready in the calendar above, and fill in the event's README.

**Good projects:**
- have a part kids can do on paper first
- teach **one** new coding idea
- keep the code short enough to type in and fit on one printed side
- have a twist that makes it feel special (Beating Heart's arrow goes *through* the hearts)
- on the Casio, start with `gint_setrestart(1);` (see the [Tetris guide](../games/tetris/README.md#layer-1-the-game-loop) for why). For animation, use a hardware timer as Tetris and Beating Heart do. The emulator doesn't handle `sleep_us()` well.
