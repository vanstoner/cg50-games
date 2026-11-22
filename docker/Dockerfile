FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV PATH="/root/.local/bin:${PATH}"

# Build deps for the fxSDK host tools (libpng, libusb, udisks2, sdl2, ncurses)
# plus everything sh-elf-binutils/sh-elf-gcc check for via dpkg. Any package
# they find missing triggers an interactive `sudo apt install` prompt, which
# fails in a non-interactive Docker build.
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git python3 python3-pil \
    libpng-dev libusb-1.0-0-dev libmpfr-dev libmpc-dev libgmp-dev libppl-dev \
    libglib2.0-dev libsdl2-dev libncurses-dev libudisks2-dev \
    texinfo flex bison curl xz-utils ca-certificates pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Install GiteaPC
RUN git clone https://git.planet-casio.com/Lephenixnoir/GiteaPC /tmp/giteapc \
    && make -C /tmp/giteapc install \
    && rm -rf /tmp/giteapc

# -y skips giteapc's "Is that okay (Y/n)?" prompt; without it the build dies
# with EOFError because there is no stdin.

# Stage 1: fxSDK and SuperH cross-compiler
RUN giteapc install -y \
    Lephenixnoir/fxsdk \
    Lephenixnoir/sh-elf-binutils \
    Lephenixnoir/sh-elf-gcc

# Stage 2: Math and C standard libraries
RUN giteapc install -y \
    Lephenixnoir/OpenLibm \
    Vhex-Kernel-Core/fxlibc

# Stage 3: gint kernel
RUN giteapc install -y \
    Lephenixnoir/gint

WORKDIR /workspace
