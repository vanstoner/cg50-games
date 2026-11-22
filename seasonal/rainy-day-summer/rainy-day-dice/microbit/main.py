# Rainy Day Dice for the BBC micro:bit (MicroPython)
from microbit import *
import music
import random

# The six dice faces: 9 = a dot, 0 = no dot
FACES = [Image("00000:00000:00900:00000:00000"),
         Image("00009:00000:00000:00000:90000"),
         Image("00009:00000:00900:00000:90000"),
         Image("90009:00000:00000:00000:90009"),
         Image("90009:00000:00900:00000:90009"),
         Image("90009:00000:90009:00000:90009")]

dice = [1, 1, 1, 1, 1]   # the five dice
held = [False] * 5       # which dice are held
rolls = 0                # rolls used (3 max)
chosen = -1              # picked die (-1 = none)

def show_dice():
    # One column per die: count the lights
    picture = Image(5, 5)
    flash = running_time() // 300 % 2
    for x in range(5):
        height = dice[x]
        if height == 6:
            height = 5 * flash   # too tall: flash
        level = 3                # dim = not held
        if held[x]:
            level = 9            # bright = held
        for y in range(height):
            picture.set_pixel(x, 4 - y, level)
    display.show(picture)

def roll():
    # Rattle, then roll the dice that aren't held
    for i in range(8):
        display.show(random.choice(FACES))
        music.pitch(random.randint(200, 800), 40)
    for x in range(5):
        if not held[x]:
            dice[x] = random.randint(1, 6)

display.scroll("SHAKE!")
while True:
    if accelerometer.was_gesture("shake"):
        if rolls == 3:          # next player's go
            held = [False] * 5
            rolls = 0
        roll()
        rolls = rolls + 1
        chosen = -1
        if rolls == 3:          # turn over: write it down
            music.play(music.POWER_UP)
            text = ""
            for d in dice:
                text = text + str(d) + " "
            display.scroll(text)
        else:
            display.show(3 - rolls)  # rolls left
            sleep(600)

    if button_a.was_pressed() and rolls > 0:
        chosen = chosen + 1
        if chosen == 5:
            chosen = -1         # back to all five
        else:
            display.show(chosen + 1)  # which die
            sleep(300)

    if button_b.was_pressed() and chosen >= 0 and rolls < 3:
        held[chosen] = not held[chosen]
        if held[chosen]:
            music.pitch(880, 100)  # beep beep = held
            sleep(80)
            music.pitch(880, 100)
        else:
            music.pitch(440, 500)  # long beep = cleared

    if rolls == 0:
        display.show("?")
    elif chosen >= 0:
        face = FACES[dice[chosen] - 1]
        if not held[chosen]:
            face = face * 0.3
        display.show(face)
    else:
        show_dice()
    sleep(50)
