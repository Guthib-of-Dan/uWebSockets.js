# Opinionated changes. You are free to avoid them.
Everything is just as I said. They don't affect functionality in almost any way, but help with daily grind within this repo.
- build.c is separated into build.c and build-conf.h, where build-conf.h contains contains all macro definitions, "node_versions" struct, "run" command, "get_cpu_count" helper
- Each command in build.c was extended to multiline strings to ensure readbility.
- Added .gitignore and "dirty = ignore" to .gitmodules, making it barely possible to commit unnecessary stuff.
- Makefile generates only "build.exe" to avoid confusion. It works on all platforms.
- Created "tmp" directory, where all intermediate artifacts (like object files, .tar archives) are saved.
- "dist" directory now contains index.d.ts, package.json, ESM wrapper - all files, present in "dist" branch. This enables repo to update versions in all files with a single script - update-version.sh
- C++ functions like "getHttpRequest" perform checks and throw JS errors if something failed. If there was an error, they return "nullptr". If they return nothing, iniator caller does not need to do any other task. That's why instead of `if (req) { // do smth }` there is `if (!req) return; // do smth`
- `const FunctionCallbackInfo<Value>&` is now `args_t`. Opiniated.
- Functions, which register handlers to JS objects now use lambdas to reduce boilerplate. Almost every "init" function has such.
- "App.h" and "Http3App.h" are included only in Utilities.h, while Utilities.h is everywhere
