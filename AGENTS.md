# Project Instructions

- Commit messages must start with an emoji and `[area]` tag (e.g., `✨ [memory] Title`).
- When a follow-up section is requested, add a `Follow-up` heading with bullet links like: `[Issue #N: short description](https://github.com/JustinLye/jage/issues/N)`.
- Do not add code comments unless explicitly requested.
- All builds, tests, and CMake commands must be run inside the devcontainer. Use `docker exec --user vscode <container_name> bash -c "cd /workspaces/jage && <command>"` to run commands from the host. Always use `--user vscode` to avoid changing file ownership to root. Never install build tools on the host machine.
