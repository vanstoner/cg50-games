# CG50 Playground

A place to make fun things for the **Casio fx-CG50** graphing calculator: games, little tools, experiments, whatever seems cool.

It's here to try ideas, break things and learn how real programs get built. Nothing has to be perfect. If it runs on the calculator and makes someone smile, it counts.

## What's in here

```
games/      Games you can play on the calculator
  tetris/   Full Tetris, plus a step-by-step guide to building it yourself
seasonal/   A project for each celebration through the year, with printables (Casio and micro:bit)
tools/      Handy programs and utilities (coming soon)
docs/       Guides, like C for Scratch and Python coders
docker/     The recipe for the build toolchain (you rarely need to touch this)
emulator/   A pretend CG50 that runs in your browser (see below)
.github/    The robots that build everything automatically
```

Each project lives in its own folder with a `CMakeLists.txt` and a `src/` folder of C code.

## How it works

The CG50 can't run code written on a laptop as-is. Programs have to be **compiled** into a special `.g3a` file that the calculator understands. We use:

- **[gint](https://git.planet-casio.com/Lephenixnoir/gint)**: a library that lets our C code draw on the screen, read the keys and keep time.
- **[fxSDK](https://git.planet-casio.com/Lephenixnoir/fxsdk)**: the tools that turn the C code into a `.g3a` file.

Setting these up by hand is fiddly, so it's all packed into a Docker image. GitHub builds it for us.

## Seasonal projects 🎄💗🎃

Short projects for celebrations through the year: Valentine's Day, Easter, Halloween, Diwali, Christmas and more. Each comes with a **printable** (1–2 A4 sides) to design something on paper first, and the code to bring it to life. Some projects also have a **BBC micro:bit** version in Python, so you can join in without a calculator. See the [seasonal calendar](seasonal/README.md). The first one, [Beating Heart](seasonal/valentines-day/beating-heart/), is ready to try on both.

## Getting a game onto the calculator

1. Open the **Actions** tab on GitHub and click the latest **Build CG50 Games** run.
2. Download the artifact for the game (for example `tetris-g3a`) and unzip it.
3. Plug the calculator into the computer with its USB cable. When it asks, choose **USB Flash** (press `F1`).
4. Copy the `.g3a` file onto the calculator drive (the top folder is fine).
5. Eject the drive properly, unplug, and the game shows up in the calculator's main menu.

## Making something new

New to C? If you know Scratch or Python, [C for Scratch and Python coders](docs/C-PRIMER.md) explains the differences side by side.

New to making calculator games? Start with the [Tetris guide](games/tetris/README.md). It builds a whole game from an empty file, one layer at a time, and finishes with a template and ideas for making your own.

1. **Copy an existing project.** Duplicate `games/tetris` into a new folder, e.g. `games/snake`.
2. **Rename it.** In the new `CMakeLists.txt`, change `Tetris`/`tetris` to your project's name.
3. **Add it to the build.** In `.github/workflows/build-games.yml`, add your project to the list:
   ```yaml
   - name: snake
     path: games/snake
   ```
4. **Write some code** in `src/main.c`, push it to GitHub, and the robots build it for you.

### Handy things to know

- The screen is **396 × 224** pixels (`DWIDTH` × `DHEIGHT`).
- Draw with `dclear`, `drect`, `dtext` and friends, then call `dupdate()` to show the frame.
- Read keys with `pollevent()` (doesn't wait) or `getkey()` (waits for a press).
- Colours: `C_BLACK`, `C_WHITE`, `C_RED`... or mix your own with `C_RGB(r, g, b)` (each 0–31).
- Always give the player a way out, e.g. the `EXIT` key.

### Building on your own computer (optional)

If Docker is installed you can build without waiting for GitHub:

```bash
docker build -t cg50-fxsdk docker
```

```bash
docker run --rm -v "$PWD/games/tetris:/workspace/game" -w /workspace/game cg50-fxsdk fxsdk build-cg
```

The first command takes a while (it builds a whole compiler). After that, builds take seconds.

## Test it in the emulator first 🖥️

You don't need the calculator to try your game. There's an emulator (a pretend calculator) that runs in your web browser:

```bash
./run-emulator.sh games/tetris
```

Then open **http://localhost:6080**. You'll see the calculator screen with its keypad underneath.

- The **arrow keys** on your keyboard work. Click the on-screen buttons for everything else (like **EXIT**).
- Each run rebuilds your game first, so you always see your latest code.
- There's no real calculator menu in the emulator, so **EXIT** and **MENU** just restart the game.
- Press **Ctrl+C** in the terminal to stop.

The emulator is [casio-emu](https://github.com/Heath123/casio-emu). It's very good but not perfect. If something looks odd, try it on the real calculator before spending ages hunting a bug.

If you built the toolchain yourself, point the script at your image: `FXSDK_IMAGE=cg50-fxsdk ./run-emulator.sh games/tetris`.

## Ideas board

Want something to make? Some ideas, from easier to harder:

- 🎲 Dice roller or coin flipper
- ⏱️ Reaction-time tester
- 🐍 Snake
- 🧮 A tool for your maths homework (times tables quiz, unit converter)
- 🏓 Pong (two players on one keypad!)
- 🧱 Build Tetris yourself with [the step-by-step guide](games/tetris/README.md), then add a hold piece or a line-clear animation
- 👾 Space Invaders

## Learn more

- [Planet Casio](https://www.planet-casio.com/): the community behind gint and fxSDK (mostly in French, but the forums are friendly)
- Look at the code in `games/` and change things to see what happens. That's how most programmers learned.

Have fun! 🎮
