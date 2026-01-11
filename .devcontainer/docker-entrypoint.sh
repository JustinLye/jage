#!/usr/bin/env bash
set -euo pipefail

USERNAME="${USERNAME:-vscode}"

align_docker_gid() {
  if [ -S /var/run/docker.sock ]; then
    HOST_DOCKER_GID="$(stat -c '%g' /var/run/docker.sock 2>/dev/null || true)"
    if [ -n "$HOST_DOCKER_GID" ]; then
      if getent group docker >/dev/null 2>&1; then
        CONTAINER_DOCKER_GID="$(getent group docker | cut -d: -f3)"
        if [ "$CONTAINER_DOCKER_GID" != "$HOST_DOCKER_GID" ]; then
          groupmod -g "$HOST_DOCKER_GID" docker 2>/dev/null \
            || groupmod -o -g "$HOST_DOCKER_GID" docker 2>/dev/null || true
        fi
      else
        groupadd -g "$HOST_DOCKER_GID" docker 2>/dev/null \
          || groupadd -o -g "$HOST_DOCKER_GID" docker 2>/dev/null || true
      fi
      usermod -aG docker "$USERNAME" 2>/dev/null || true
    fi
  fi
}

align_docker_gid

if [ "$#" -eq 0 ]; then
  exec runuser -u "$USERNAME" -- /bin/bash
fi

exec runuser -u "$USERNAME" -- "$@"
