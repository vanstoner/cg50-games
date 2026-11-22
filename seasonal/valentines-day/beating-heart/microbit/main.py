# Beating Heart for the BBC micro:bit (MicroPython)
from microbit import *
import music
import radio

radio.on()

# Your designs! Each number is how bright that light is:
# 0 = off, 9 = brightest
HEART = Image("09090:"
              "99999:"
              "99999:"
              "09990:"
              "00900")

BEAT_IN = Image("03030:"     # the little heart in the
                "39393:"     # middle is bright...
                "39993:"
                "03930:"
                "00300")

BEAT_OUT = Image("09090:"    # ...then the ring
                 "93939:"    # around it
                 "93339:"
                 "09390:"
                 "00900")

def cupid():
    # The arrow flies along the middle row, through the heart
    for x in range(7):
        picture = HEART * 0.3            # a dim heart behind
        for i in range(3):               # the arrow is 3 lights long
            if 0 <= x - i <= 4:
                picture.set_pixel(x - i, 2, 9 - i * 3)
        display.show(picture)
        sleep(90)
    music.play(["C6:1", "E6:1", "G6:2"], wait=False)

cupid()
while True:
    display.show(BEAT_IN)
    sleep(150)
    display.show(BEAT_OUT)
    sleep(150)
    display.show(HEART * 0.3)
    sleep(500)

    if button_a.was_pressed():
        cupid()
    if button_b.was_pressed():
        radio.send("cupid")              # fire at a friend!
        display.show(Image.ARROW_E)
        sleep(400)
    if radio.receive() == "cupid":
        cupid()
