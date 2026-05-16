#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* List of platform features */
#if defined(_WIN32)
#define OS "win32"
#define IS_WINDOWS
// this macro suppresses Windows warning about "secure" deprecations, which only Windows cares about
// #define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
int get_cpu_count(void) {
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return (int)sysinfo.dwNumberOfProcessors || 1;
}
#define START_FOREACH_NODEJS \
  for (unsigned int i = 0; i < versionsQuantity; i++) { 
#define END_FOREACH_NODEJS }

#else // POSIX systems

#define START_FOREACH_NODEJS(i) \
  pid_t pids[versionsQuantity]; \
  for (unsigned int i = 0; i < versionsQuantity; i++) { \
    pids[i] = fork(); \
    if (pids[i] != 0) continue; 

#define END_FOREACH_NODEJS \
    exit(0); \
  } \
  for (unsigned int i = 0; i < versionsQuantity; i++) waitpid(pids[i], 0, 0);

#include <unistd.h>
#include <sys/wait.h>
int get_cpu_count(void) {
    long n = sysconf(_SC_NPROCESSORS_ONLN);
    return (n > 0) ? (int)n : 1;
}

#if defined(__linux)

#define OS "linux"
#define IS_LINUX

#elif defined(__APPLE__)
#define OS "darwin"
#define IS_MACOS
#endif

#endif



/* ASAN vs. optimized build flags (used via C string literal concatenation).
 * OPT_FLAGS / LINK_FLAGS: inserted mid-string, so each definition starts with a space.
 * LINUX_LINK_EXTRAS: passed as a standalone argument, so no leading space.
 * MACOS_LINK_EXTRAS: appended after "-undefined dynamic_lookup", so ASAN variant starts with a space. */
#ifdef WITH_ASAN
#define OPT_FLAGS " -fsanitize=address -fno-omit-frame-pointer -g -O1"
#define LINUX_LINK_EXTRAS "-fsanitize=address"
#define MACOS_LINK_EXTRAS " -fsanitize=address"
#else
#define OPT_FLAGS " -flto -O3"
#define LINUX_LINK_EXTRAS "-static-libstdc++ -static-libgcc -s"
#define MACOS_LINK_EXTRAS ""
#endif

const char *ARM = "arm";
const char *ARM64 = "arm64";
const char *X64 = "x64";

/* System, but with string replace */
int run(const char *cmd, ...) {
    char buf[2048];
    va_list args;
    va_start(args, cmd);
    vsprintf(buf, cmd, args);
    va_end(args);
    printf("--> %s\n\n", buf);
    return system(buf);
}

/* List of Node.js versions */
struct node_version {
    const char *name;
    const char *abi;
} versions[] = {
    {"v22.0.0", "127"},
    {"v24.0.0", "137"},
    {"v26.0.0", "147"}
};
const int versionsQuantity = sizeof(versions) / sizeof(struct node_version);
int threads_quantity;
