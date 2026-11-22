# Seasonal projects

A calculator coding project for celebrations through the year. Each one is small enough to do in an afternoon, and each comes in two parts:

- **A printable** (1–2 A4 sides) with a hands-on, no-computer activity: design something on paper, which is half the fun.
- **The code** that brings the design to life on the Casio fx-CG50.

Each project also teaches one coding idea. Do them in any order, but they get gradually trickier through the year.

## The calendar

Some festivals follow the moon, so their dates **move each year**. Check this year's date before you plan.

| Month | Event | Project | Coding idea | Status |
|---|---|---|---|---|
| January | [New Year](new-year/) | Countdown Clock | Timers, big digits | Planned |
| January – February | [Winter Fun](winter-fun/) | Snowflake Maker | Symmetry | Planned |
| Jan – Feb *(moves)* | [Lunar New Year](lunar-new-year/) | Dragon Dance | Arrays that follow | Planned |
| 14 February | [Valentine's Day](valentines-day/) | **[Beating Heart](valentines-day/beating-heart/)** | Pixel art as data, loops | ✅ **Ready** |
| 14 March | [Pi Day](pi-day/) | Raining Pi | Random numbers, maths | Planned |
| *(moves)* | [Ramadan](ramadan/) | Crescent Moon Tracker | Drawing with circles | Planned |
| *(moves)* | [Eid](eid/) | Eid Star Patterns | Geometry, rotation | Planned |
| March *(moves)* | [Holi](holi/) | Colour Splash | Mixing colours, randomness | Planned |
| UK March / US May | [Mother's Day](mothers-day/) | Flower for Mum | Loops and angles | Planned |
| March – April *(moves)* | [Easter](easter/) | Egg Decorator | Patterns with `%` | Planned |
| 22 April | [Earth Day](earth-day/) | Grow a Forest | Growth, branching | Planned |
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
  README.md                      this calendar
  printable-kit/                 shared look and tools for every printable
    theme.css
    make-pdf.sh
  valentines-day/
    README.md                    the event, and its project(s)
    beating-heart/               one project
      README.md                  what it is, how to build and run it
      CMakeLists.txt
      src/main.c                 the code (the same as on the printable)
      printable/
        beating-heart.html       the printable (edit this)
        beating-heart.pdf        ready to print
```

An event can have more than one project. Just add another folder next to `beating-heart/`.

## Trying a project

From the top folder of the repo:

```bash
./run-emulator.sh seasonal/valentines-day/beating-heart
```

The GitHub build also compiles every seasonal project. Download its `.g3a` from the **Actions** tab, and copy it to the calculator as described in the [main README](../README.md#getting-a-game-onto-the-calculator).

## Adding a project

1. **Copy the example:** `cp -r seasonal/valentines-day/beating-heart seasonal/<event>/<your-project>`
2. **Rename it** in `CMakeLists.txt` (`project`, `add_executable`, `TARGET`, `OUTPUT`, `NAME`).
3. **Write the code** in `src/main.c` and test it with `./run-emulator.sh`.
4. **Make the printable:** edit the HTML in `printable/`, using the shared classes from [printable-kit/theme.css](printable-kit/theme.css). Keep it to **one or two A4 sides**. Then make the PDF:
   ```bash
   seasonal/printable-kit/make-pdf.sh seasonal/<event>/<your-project>/printable/<name>.html
   ```
   It tells you how many pages it came out as.
5. **Add it to the build:** add a line to the `include:` list in [.github/workflows/build-games.yml](../.github/workflows/build-games.yml).
6. **Update the tables:** mark it ready in the calendar above, and fill in the event's README.

**Good projects:**
- have a part kids can do on paper first
- teach **one** new coding idea
- keep the code short enough to type in (aim for under 60 lines)
- start with `gint_setrestart(1);` (see the [Tetris guide](../games/tetris/README.md#layer-1-the-game-loop) for why)
