#!/usr/bin/env python3
"""Draws the Tetris menu icons for the CG50.

The calculator needs two 92x64 pictures:
  icon-uns.png  the icon normally
  icon-sel.png  the icon when it's highlighted in the MENU

Change the colours or the STACK pattern below, then run (from the repo root):

  docker run --rm -v "$PWD:/w" -w /w/games/tetris/assets-cg cg50-fxsdk python3 make-icons.py
"""

from PIL import Image, ImageDraw

W, H = 92, 64

# Same colours as the game, as (red, green, blue) from 0 to 255
COLORS = {
    "I": (0, 226, 255),
    "O": (255, 226, 0),
    "T": (165, 48, 230),
    "S": (32, 226, 32),
    "Z": (255, 32, 32),
    "J": (48, 97, 255),
    "L": (255, 130, 0),
}

# The blocks in the bottom half of the icon. "." is empty.
STACK = [
    "....TTT.....",
    ".....T......",
    "I.........OO",
    "I.JJJ.SS..OO",
    "IZZJ.SSLLL.L",
    "I.ZZLLLL.LLL",
]

# The word at the top, drawn with a tiny 3x5 pixel font, one colour per letter
WORD = [("T", "Z"), ("E", "L"), ("T", "O"), ("R", "S"), ("I", "I"), ("S", "T")]
FONT = {
    "T": ["111", "010", "010", "010", "010"],
    "E": ["111", "100", "110", "100", "111"],
    "R": ["110", "101", "110", "101", "101"],
    "I": ["111", "010", "010", "010", "111"],
    "S": ["011", "100", "010", "001", "110"],
}

CELL = 6        # block size in the icon, in pixels
TEXT_SCALE = 2  # each font pixel becomes 2x2


def shade(color, amount):
    """Lighter (amount > 0) or darker (amount < 0) version of a colour."""
    return tuple(max(0, min(255, int(c + (255 - c if amount > 0 else c) * amount)))
                 for c in color)


def draw_block(d, x, y, color):
    s = CELL - 1
    d.rectangle([x, y, x + s - 1, y + s - 1], fill=color)
    d.line([x, y, x + s - 1, y], fill=shade(color, 0.5))          # light top
    d.line([x, y, x, y + s - 1], fill=shade(color, 0.5))          # light left
    d.line([x, y + s - 1, x + s - 1, y + s - 1], fill=shade(color, -0.4))  # dark bottom
    d.line([x + s - 1, y, x + s - 1, y + s - 1], fill=shade(color, -0.4))  # dark right


def make_icon(outside, path):
    img = Image.new("RGB", (W, H), outside)
    d = ImageDraw.Draw(img)

    # The tile: a dark rounded rectangle
    d.rounded_rectangle([2, 2, W - 3, H - 3], radius=7,
                        fill=(16, 18, 34), outline=(70, 76, 110), width=1)

    # The word "TETRIS"
    letter_w = 3 * TEXT_SCALE
    gap = 2
    total = len(WORD) * letter_w + (len(WORD) - 1) * gap
    x = (W - total) // 2
    y = 7
    for letter, piece in WORD:
        for row, bits in enumerate(FONT[letter]):
            for col, bit in enumerate(bits):
                if bit == "1":
                    px = x + col * TEXT_SCALE
                    py = y + row * TEXT_SCALE
                    d.rectangle([px, py, px + TEXT_SCALE - 1, py + TEXT_SCALE - 1],
                                fill=COLORS[piece])
        x += letter_w + gap

    # The stack of blocks
    cols = len(STACK[0])
    bx = (W - cols * CELL) // 2
    by = H - 6 - len(STACK) * CELL
    for row, line in enumerate(STACK):
        for col, ch in enumerate(line):
            if ch != ".":
                draw_block(d, bx + col * CELL, by + row * CELL, COLORS[ch])

    img.save(path)
    print("wrote", path)


make_icon((255, 255, 255), "icon-uns.png")
make_icon((60, 150, 240), "icon-sel.png")
