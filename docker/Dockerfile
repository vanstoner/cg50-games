FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV PATH="/root/.local/bin:${PATH}"

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git python3 python3-pil \
    libpng-dev libusb-1.0-0-dev libmpfr-dev libmpc-dev libgmp-dev \
    libglib2.0-dev libsdl2-dev libncurses-dev libudisks2-dev \
    texinfo flex bison curl ca-certificates pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Install GiteaPC
RUN git clone https://git.planet-casio.com/Lephenixnoir/GiteaPC /tmp/giteapc \
    && make -C /tmp/giteapc install \
    && rm -rf /tmp/giteapc

# Stage 1: fxSDK and SuperH cross-compiler
RUN giteapc install \
    Lephenixnoir/fxsdk \
    Lephenixnoir/sh-elf-binutils \
    Lephenixnoir/sh-elf-gcc

# Stage 2: Math and C standard libraries
RUN giteapc install \
    Lephenixnoir/OpenLibm \
    Vhex-Kernel-Core/fxlibc

# Stage 3: gint kernel
RUN giteapc install \
    Lephenixnoir/gint

WORKDIR /workspace
