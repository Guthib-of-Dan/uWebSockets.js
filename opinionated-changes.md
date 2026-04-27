# Opinionated changes. You are free to avoid them.
Everything is just as I said. They don't affect functionality in almost any way, but help with daily grind within this repo.
- build.c is separated into build.c and build-conf.h, where build-conf.h contains contains all macro definitions, "node_versions" struct, "run" command, "get_cpu_count" helper
- Each command in build.c was extended to multiline strings to ensure readbility.
- Added .gitignore and "dirty = ignore" to .gitmodules, making it barely possible to commit unnecessary stuff.
- Makefile generates only "build.exe" to avoid confusion. It works on all platforms.

