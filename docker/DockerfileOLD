FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV PATH="/root/.local/bin:${PATH}"

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git python3 python3-pip python3-pil \
    libpng-dev libusb-1.0-0-dev libmpfr-dev libmpc-dev libgmp-dev \
    texinfo flex bison curl ca-certificates pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Install GiteaPC (the official fxSDK package manager)
RUN git clone https://gitea.planet-casio.com/Lephenixnoir/GiteaPC /tmp/giteapc \
    && pip3 install --break-system-packages /tmp/giteapc \
    && rm -rf /tmp/giteapc

# Install fxSDK, SuperH cross-compiler, fxlibc, and gint
RUN giteapc install -y \
    Lephenixnoir/fxsdk \
    Lephenixnoir/sh-elf-binutils \
    Lephenixnoir/sh-elf-gcc \
    Lephenixnoir/fxlibc \
    Lephenixnoir/gint

WORKDIR /workspace
