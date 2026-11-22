#!/bin/sh
# Starts a virtual screen, shares it over VNC/noVNC, and runs the emulator.
set -e

G3A="${1:-/addin/game.g3a}"
if [ ! -f "$G3A" ]; then
    echo "No add-in found at $G3A" >&2
    exit 1
fi

# Screen sized to fit the emulator window (396x224 display + keypad)
Xvfb :99 -screen 0 420x800x24 -nolisten tcp >/dev/null 2>&1 &
sleep 1
x11vnc -display :99 -forever -shared -nopw -localhost -rfbport 5900 -quiet >/dev/null 2>&1 &
websockify --web /usr/share/novnc 6080 localhost:5900 >/dev/null 2>&1 &

echo ""
echo "  Emulator running: open http://localhost:6080 in your browser"
echo "  Press Ctrl+C here to stop it."
echo ""

# Restart the emulator if the add-in quits (e.g. after pressing EXIT)
trap 'kill $(jobs -p) 2>/dev/null; exit 0' INT TERM
while true; do
    # Run in the background and wait, so Ctrl+C / docker stop are handled at once
    calcemu "$G3A" &
    wait $! || true
    echo "  Add-in exited, restarting..."
    sleep 1
done
