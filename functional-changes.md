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

- Branches "binaries", "binaries-asan" and "dist" are the ones receiving constant commits from GitHub Actions CI. When, in fact, GitHub Releases make persistent snapshots of the repo at certain commits, Git history doesn't provide much benefit over a simple disk storage. If some change in code has to be reverted, "master" is always update, hence "binaries" receive updated artifacts. Reverting this storage is never barely ever used. That is why we can easily "reset" their history, removing 1.5GB+ of Git commits, that never get used. First step is to delete "binaries" and "binaries-asan" binaries from the repo, which does not hurt GitHub Releases in any way. Secondly, we can run "git checkout --orphan binaries && git rm -rf --cached ." to create empty
