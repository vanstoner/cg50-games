FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV PATH="/root/.local/bin:${PATH}"

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git python3 python3-pil \
    libpng-dev libusb-1.0-0-dev libmpfr-dev libmpc-dev libgmp-dev \
    libglib2.0-dev libsdl2-dev texinfo flex bison curl ca-certificates pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Install GiteaPC via make install from the current Forgejo URL
RUN git clone https://git.planet-casio.com/Lephenixnoir/GiteaPC /tmp/giteapc \
    && make -C /tmp/giteapc install \
    && rm -rf /tmp/giteapc

# Install fxSDK, SuperH cross-compiler, C/math libs, and gint in dependency order
RUN giteapc install -y \
    Lephenixnoir/fxsdk \
    Lephenixnoir/sh-elf-binutils \
    Lephenixnoir/sh-elf-gcc \
    && giteapc install -y \
    Lephenixnoir/OpenLibm \
    Vhex-Kernel-Core/fxlibc \
    && giteapc install -y \
    Lephenixnoir/gint

WORKDIR /workspace
