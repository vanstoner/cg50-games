#!/bin/sh
# Build a game and play it in the CG50 emulator in your browser.
#
#   ./run-emulator.sh games/tetris
#
# Then open http://localhost:6080. Press Ctrl+C to stop.
#
# Options (environment variables):
#   FXSDK_IMAGE   toolchain image used to compile (default: the one CI publishes)
#   SKIP_BUILD=1  don't recompile, just run the existing .g3a
#   PORT          browser port (default 6080)
set -e

cd "$(dirname "$0")"

GAME_DIR="${1:-}"
if [ -z "$GAME_DIR" ] || [ ! -f "$GAME_DIR/CMakeLists.txt" ]; then
    echo "Usage: $0 <project folder>   e.g. $0 games/tetris" >&2
    exit 1
fi

FXSDK_IMAGE="${FXSDK_IMAGE:-ghcr.io/vanstoner/cg50-fxsdk:latest}"
EMU_IMAGE="cg50-emulator"
PORT="${PORT:-6080}"

# 1. Compile the game so we always test the latest code
if [ -z "$SKIP_BUILD" ]; then
    echo "==> Building $GAME_DIR"
    # CMake remembers the folder it was configured in; if an old build was
    # made with the game mounted somewhere else, start that build fresh.
    CACHE="$GAME_DIR/build-cg/CMakeCache.txt"
    if [ -f "$CACHE" ] && ! grep -q "CMAKE_HOME_DIRECTORY:INTERNAL=/workspace/game$" "$CACHE"; then
        rm -rf "$GAME_DIR/build-cg"
    fi
    docker run --rm -v "$PWD/$GAME_DIR:/workspace/game" -w /workspace/game \
        "$FXSDK_IMAGE" fxsdk build-cg
fi

G3A=$(ls "$GAME_DIR"/*.g3a 2>/dev/null | head -n 1)
if [ -z "$G3A" ]; then
    echo "No .g3a found in $GAME_DIR - did the build fail?" >&2
    exit 1
fi

# 2. Build the emulator image the first time
if ! docker image inspect "$EMU_IMAGE" >/dev/null 2>&1; then
    echo "==> Building emulator image (first time only)"
    docker build -t "$EMU_IMAGE" emulator
fi

# 3. Run it
echo "==> Starting emulator with $G3A"
TTY_FLAGS=""
[ -t 0 ] && TTY_FLAGS="-it"
# shellcheck disable=SC2086
docker run --rm $TTY_FLAGS --init --name cg50-emulator \
    -p "127.0.0.1:$PORT:6080" \
    -v "$PWD/$G3A:/addin/game.g3a:ro" \
    "$EMU_IMAGE"
