# 🎲 Rainy Day Dice

*A Rainy Day Summer project.*

Turn a BBC micro:bit into five dice for a family dice game. Shake to roll, hold the dice you want to keep, and roll again up to three times. Then write your score on the printed score sheet and pass it on. For 2 to 4 players.

| | |
|---|---|
| **Device** | BBC micro:bit (V2 for the beeps; a V1 plays silently) |
| **Language** | Python (MicroPython) |
| **Code** | [microbit/main.py](microbit/main.py) |
| **Printable** | 📄 [rainy-day-dice-microbit.pdf](printable/rainy-day-dice-microbit.pdf): side 1 is how to play and the score sheet, side 2 is the code |
| **Age** | About 8+ to play, 10+ to type in the code |
| **Time** | 40 minutes to type and run, then as many games as you like |
| **Coding ideas** | Random numbers, lists, True/False values, counting turns |

## Run it

First time with a micro:bit? See the [setup guide](../../README.md#first-time-with-a-microbit): you'll need Chrome or Edge and a USB data cable.

1. Open **[python.microbit.org](https://python.microbit.org)** and paste in (or type) [microbit/main.py](microbit/main.py).
2. Press ▶ on the simulator and use its **shake** button to roll.
3. Click **Send to micro:bit**, or **Save** and drag the `.hex` file onto the `MICROBIT` drive.

## How to play

| Do this | What happens |
|---|---|
| **Shake** | Roll every die that isn't held (3 rolls per turn) |
| **A** | Pick the next die: shows its number (1–5), then its face close up. After die 5 it goes back to all five. |
| **B** | Hold or let go of the picked die: **beep beep** = held, **long beep** = let go |

**Reading the dice.** Each of the five columns is a die. Count the lights from the bottom. A 6 doesn't fit in 5 rows, so its column flashes. Held dice are bright and dice that will roll are dim.

After the third roll the micro:bit plays a tune and scrolls the five numbers. The player picks an empty box on the score sheet and writes the score in it, then the next player shakes to start their turn, which clears all the holds. After 13 turns each, the highest grand total wins.

The scoring is the classic "Yacht" family of dice games: ones to sixes with a 35-point bonus, three and four of a kind, a full house, small and large straights, five of a kind, and chance.

## How the code works

[microbit/main.py](microbit/main.py) is exactly the code printed on the sheet.

1. **`FACES`** holds the six die faces as 5 × 5 pictures, used for the close-up and the rolling animation.
2. **`dice`** and **`held`** are lists of five. `rolls` counts rolls this turn, and `chosen` is the die picked with A (-1 means none).
3. **`show_dice()`** draws one column per die. The brightness shows whether a die is held. `running_time() // 300 % 2` flips between 0 and 1 every 0.3 seconds, which makes a 6 flash.
4. **`roll()`** flashes random faces with a rattle sound, then gives every unheld die a new `random.randint(1, 6)`.
5. **The main loop** checks for a shake, A and B, then decides what to show: `?` before the first roll, one die close up, or all five.

## Ideas to go further

- Make held dice flash, or show the rolls left as dots along the top.
- Let the micro:bit spot five of a kind (all five values equal) and play a fanfare.
- Harder: work out the score for each box on the micro:bit, and scroll the best choice.
