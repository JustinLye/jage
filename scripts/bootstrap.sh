#!/bin/bash

set -euo pipefail

CI=${CI-}

if [ -z "${CI}" ]; then
  sudo apt-get install -y \
    gcc-14 \
    g++-14 \
    clang-18 \
    clang++-18
else
  sudo apt-get install -y \
    ${MATRIX_C_COMPILER}-${MATRIX_COMPILER_VERSION} \
    ${MATRIX_CPP_COMPILER}-${MATRIX_COMPILER_VERSION}
fi



sudo apt-get install -y \
  libgl-dev \
  libx11-dev \
  libx11-xcb-dev \
  libfontenc-dev \
  libice-dev \
  libsm-dev \
  libxau-dev \
  libxaw7-dev \
  libxcomposite-dev \
  libxcursor-dev \
  libxdamage-dev \
  libxdmcp-dev \
  libxext-dev \
  libxfixes-dev \
  libxi-dev \
  libxinerama-dev \
  libxkbfile-dev \
  libxmu-dev \
  libxmuu-dev \
  libxpm-dev \
  libxrandr-dev \
  libxrender-dev \
  libxres-dev \
  libxss-dev \
  libxt-dev \
  libxtst-dev \
  libxv-dev \
  libxxf86vm-dev \
  libxcb-glx0-dev \
  libxcb-render0-dev \
  libxcb-render-util0-dev \
  libxcb-xkb-dev \
  libxcb-icccm4-dev \
  libxcb-image0-dev \
  libxcb-keysyms1-dev \
  libxcb-randr0-dev \
  libxcb-shape0-dev \
  libxcb-sync-dev \
  libxcb-xfixes0-dev \
  libxcb-xinerama0-dev \
  libxcb-dri3-dev \
  uuid-dev \
  libxcb-cursor-dev \
  libxcb-dri2-0-dev \
  libxcb-dri3-dev \
  libxcb-present-dev \
  libxcb-composite0-dev \
  libxcb-ewmh-dev \
  libxcb-res0-dev \
  libxcb-util-dev

sudo apt-get update -y