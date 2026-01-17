FROM mcr.microsoft.com/devcontainers/base:ubuntu-24.04

ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=1000

RUN groupadd -g $USER_GID $USERNAME || true \
  && useradd -m -u $USER_UID -g $USER_GID $USERNAME || true
RUN mkdir -p /home/$USERNAME/.vscode-server/extensions \
  && chown -R $USER_UID:$USER_GID /home/$USERNAME/.vscode-server /home/$USERNAME
RUN mkdir -p /home/$USERNAME/.conan2 \
  && chown -R $USER_UID:$USER_GID /home/$USERNAME/.conan2 /home/$USERNAME
RUN mkdir -p /home/$USERNAME/.cache \
  && chown -R $USER_UID:$USER_GID /home/$USERNAME/.cache /home/$USERNAME
RUN mkdir -p /home/$USERNAME/.local \
  && chown -R $USER_UID:$USER_GID /home/$USERNAME/.local /home/$USERNAME


ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y && apt-get install -y --no-install-recommends \
  ca-certificates \
  clang-18 \
  clang++-18 \
  clangd-18 \
  clang-tools-18 \
  cmake \
  direnv \
  g++-14 \
  gcc-14 \
  gdb \
  git \
  libclang-rt-18-dev \
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
  libxcb-present-dev \
  libxcb-composite0-dev \
  libxcb-ewmh-dev \
  libxcb-res0-dev \
  libxcb-util-dev \
  lsb-release \
  ninja-build \
  nodejs \
  npm \
  pkg-config \
  python3 \
  python3-venv \
  python3-pip \
  pipx \
  && rm -rf /var/lib/apt/lists/*

ENV PATH="/root/.local/bin:${PATH}"

RUN ln -s /usr/bin/clangd-18 /usr/bin/clangd

RUN npm install -g @openai/codex && npm cache clean --force

USER $USERNAME
WORKDIR /workspaces

RUN pipx install conan
RUN python3 -m venv .conan
RUN eval "$(direnv hook bash)" >> ~/.bashrc
RUN echo ". ./.conan/bin/activate" >> .envrc
RUN direnv allow
