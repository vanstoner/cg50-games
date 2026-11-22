# CG50 emulator you can use from a web browser.
#
# Runs casio-emu (https://github.com/Heath123/casio-emu) on a virtual X
# display and serves it over noVNC at http://localhost:6080.
# Use ../run-emulator.sh rather than running this by hand.

# ---- Build casio-emu (Qt GUI, which has a clickable keypad) ----
FROM ubuntu:24.04 AS build

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git ca-certificates qtbase5-dev \
    && rm -rf /var/lib/apt/lists/*

# Pinned so the image doesn't change under us
ARG CASIO_EMU_COMMIT=5d34e298b94be65f7ab0505d3c0f9b9ed802ac4b
RUN git clone https://github.com/Heath123/casio-emu /src \
    && git -C /src checkout "$CASIO_EMU_COMMIT" \
    # The keypad's arrow labels use rare Unicode arrows that show as empty
    # boxes with normal fonts; swap them for common triangles.
    && sed -i 's/🡅/▲/; s/🡆/▶/; s/🡇/▼/; s/🡄/◀/' /src/src/gui/skin/default/skin.h \
    && cmake -S /src -B /src/build -DCMAKE_BUILD_TYPE=Release \
    && make -C /src/build -j"$(nproc)"

# ---- Runtime: virtual display + VNC + noVNC web client ----
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    libqt5widgets5t64 libqt5gui5t64 fonts-dejavu-core \
    xvfb x11vnc novnc websockify procps \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /src/build/calcemu /usr/local/bin/calcemu
COPY start.sh /usr/local/bin/start-emulator
COPY index.html /usr/share/novnc/index.html
RUN chmod +x /usr/local/bin/start-emulator

ENV DISPLAY=:99 QT_QPA_PLATFORM=xcb
EXPOSE 6080
ENTRYPOINT ["/usr/local/bin/start-emulator"]
CMD ["/addin/game.g3a"]
