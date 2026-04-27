# Changes in functionalty
This repo introduces ways to address certain issues faster, more ergonomically and more in a cenrtalised manner.

## Compilation
- build.c contains "setbuf" to avoid buffering, which makes debugging GitHub Actions CI a nightmare. 

- compilation results are written to the "tmp" folder, as well as fetching archives of NodeJS headers. Each NodeJS version has its own Application Binary Interface (ABI), so uSockets is compiled to "tmp/c-%abi%" and uWebSockets.js is compiled (not linked) to "tmp/cpp-%abi%", where %abi% is a corresponding ABI version (number). This opens multithreading possibilities.

- Added "get_cpu_count" helper to build-conf.c (opinionated file) and extended "make", "ninja", "msbuild" scripts to utilize all available thread, when compiling dependencies

- Added "-DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 " option to cmake in build.c, when compiling binaries for darwin. This resolves version mismatch of the compiler "clang -target x86_64-apple-macos12" and arm64 as well

- added "fork()" in build.c to Unix platforms, when compiling C/C++ and linking libraries. uWS.js currently supports NodeJS 20, 22, 24, 25, so 4 versions in total. GitHub Actions Ubuntu runners provide us with 4 vCPU, while MacOS-15 runners - 3 vCPU. Given such quantity, it lets perform building process several times faster.

## Versioning

- moved "docs/index.d.ts", "src/uws.js", ESM_wrapper.mjs (from binaries branch), "package.json" (from binaries branch) to "dist" folder. This lets code be centralised, helps with creating such utilities as "update-version.sh", simplify "history rewind" of "binaries/binaries-asan" or "dist" branches.

- created "update-version.sh" script, which uses "Stream Editor" to update version tag of uWebSockets.js in many files at once. It also updates documentation in "docs/" folder with "typedoc" and runs "git add" to all updated files. This is not TOO opiniated because versioning is indeed a problem.

- Branches "binaries", "binaries-asan" and "dist" are the ones receiving constant commits from GitHub Actions CI. When, in fact, GitHub Releases make persistent snapshots of the repo at certain commits, Git history doesn't provide much benefit over a simple disk storage. If some change in code has to be reverted, "master" is always updated, hence "binaries" receive new (reverted) artifacts in a new commit. Reverting this storage is barely ever used. That is why we can easily "reset" their history, removing 1.5GB+ of Git commits, that never get used. First step is to delete "binaries" and "binaries-asan" binaries from the repo, which does not hurt GitHub Releases in any way. Secondly, we can run "git checkout --orphan binaries && git reset" to create empty branch with no history and staged files (working directory is not changed at all. All files are where they were). Then add README.md and LICENSE (dist branch is for JS in this uWS.js fork), commit->push - now branches weigh almost nothing, 2400 commits less than before, 800 commits less than "master"

## CI GitHub Actions pipeline

- CI uses caching for Git Submodules. It does not bring any difficulties if CI structure is well-established, which it is. The longest it takes to compile binaries is 4 minutes.

- Repo is cloned only with its last commit and submodules as well. No fetching whole history can cut ~10 seconds.

- CI exports compiled binaries as artifacts, letting VMs be started many times and reuse them for different purposes.

- Build step combines both production binaries and onse with AddressSanitizer. Less parallel jobs, more reuse of cached dependcies. Windows does not build ASAN binaries, yet it usually compiles the longest (1 vCPU in CI).

- Testing now happens for all architectures (almost), all platforms in parallel immediately after building. Overall 16 VMs spawning to test each case. The only one missing is MacOS x64. Currently only "tests/smoke.js" is run, but these parallel VMs open huge possibilities for extensions.
