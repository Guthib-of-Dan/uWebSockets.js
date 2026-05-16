#include "build.h"
void nodejs_headers(const char *version) {
  run("curl -sSL \"https://nodejs.org/dist/%s/node-%s-headers.tar.gz\" -o "
      "tmp/node-%s-headers.tar.gz"
      " && "
      "tar xzf tmp/node-%s-headers.tar.gz -C targets",
      version, version, version, version);
  run(
      /*fetch v8 fast-api manually*/
      "curl -sSfL "
      "\"https://raw.githubusercontent.com/nodejs/node/%s/deps/v8/include/"
      "v8-fast-api-calls.h\" -o "
      "\"targets/node-%s/include/node/v8-fast-api-calls.h\"",
      version, version);

#ifdef IS_WINDOWS /* fetch node.lib */
  run("curl -sSL \"https://nodejs.org/dist/%s/win-x64/node.lib\" -o "
      "\"targets/node-%s/node.lib\"",
      version, version);
#endif
}

/* Downloads headers, creates folders */
void prepare() {
    /* see console output IMMEDIATELY for debugging purposes */
    setbuf(stdout, 0);
    run("mkdir tmp");
    for (unsigned int i = 0; i < versionsQuantity; i++) {
      run("mkdir \"tmp/c-%s\"", versions[i].abi);
    }
    if (run("mkdir targets")) {
      printf("[NodeJS headers are already installed (v22,v24,v26)]\n");
      return ;
    }

    printf("\n<-- [Installing NodeJS headers] -->\n");
    START_FOREACH_NODEJS(i);
      nodejs_headers(versions[i].name);
    END_FOREACH_NODEJS;
    printf("[Fetched NodeJS headers v22,v24,v26]\n");
}

void build_lsquic(const char *arch) {
  printf("\n<-- [Started building lsquic: %s] -->\n", arch);
    
#if defined(IS_LINUX)
#define MACRO " "
#elif defined (IS_MACOS)

#if defined(CROSS_COMPILE_MACOS)
#define MACRO " -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 -DCMAKE_OSX_ARCHITECTURES=x86_64 "
#else
#define MACRO " -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 -DCMAKE_OSX_ARCHITECTURES=arm64 " 
#endif

#elif defined(IS_WINDOWS)
    /* Download zlib */
    run("curl -sSOL https://github.com/madler/zlib/releases/download/v1.3.1/zlib-1.3.1.tar.gz");
    run("tar xzf zlib-1.3.1.tar.gz");
#define MACRO " -DCMAKE_C_FLAGS=\"-DWIN32 -Wno-language-extension-token -I../../../zlib-1.3.1\" " \
    " -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++" \
    " -DZLIB_INCLUDE_DIR=../../../zlib-1.3.1" \
    " -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded "
#endif

  run("cd uWebSockets/uSockets/lsquic &&"
      " cmake -G Ninja . " MACRO
      " -DCMAKE_POSITION_INDEPENDENT_CODE=ON"
      " -DBORINGSSL_DIR=../boringssl"
      " -DCMAKE_BUILD_TYPE=Release"
      " -DLSQUIC_BIN=Off"
      " && ninja -j%i lsquic", threads_quantity);
  printf("\n[Finished building lsquic: %s]\n", arch);
#undef MACRO
}

void build_boringssl(const char *arch) {
  printf("\n<-- [Started building boringssl: %s] -->\n", arch);

#if defined(IS_MACOS)

#if defined(CROSS_COMPILE_MACOS)
#define MACRO " -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 "
#else 
#define MACRO " -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 "
#endif

#elif defined(IS_WINDOWS)
#define MACRO " -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ "
#else
#define MACRO " "
#endif

  run("cd uWebSockets/uSockets/boringssl &&"
      " cmake -G Ninja . " MACRO 
      " -DCMAKE_BUILD_TYPE=Release"
      " -DCMAKE_POSITION_INDEPENDENT_CODE=ON"
      " && ninja -j%i crypto ssl", 
      threads_quantity);
  printf("\n[Finished building boringssl: %s]\n", arch);

#undef MACRO
}

void build_uSockets_and_PCH(char *compiler, char *cpp_compiler) {

#define SHARED_MACRO \
  " -DUWS_WITH_PROXY" \
  " -DLIBUS_USE_QUIC" \
  " -DLIBUS_USE_LIBUV" \
  " -DLIBUS_USE_OPENSSL " \
  " -DWIN32_LEAN_AND_MEAN" \
  " -DUWS_REMOTE_ADDRESS_USERSPACE" 

#define SHARED_INCLUDE(CWD, node_version) \
  " -I " CWD "uWebSockets/src" \
  " -I " CWD "uWebSockets/uSockets/src" \
  " -I " CWD "uWebSockets/uSockets/lsquic/include/" \
  " -I " CWD "uWebSockets/uSockets/lsquic/wincompat" \
  " -I " CWD "uWebSockets/uSockets/boringssl/include/" \
  " -I " CWD "targets/node-" node_version "/include/node "

#if !defined(IS_WINDOWS)
#define UNIX_MACRO " -pthread -fPIC "
#else
#define UNIX_MACRO ""
#endif

  START_FOREACH_NODEJS(i);
  run("cd tmp/c-%s && %s" SHARED_MACRO UNIX_MACRO OPT_FLAGS SHARED_INCLUDE("../../", "%s")
      " -c ../../uWebSockets/uSockets/src/*.c "
      " ../../uWebSockets/uSockets/src/eventing/*.c "
      " ../../uWebSockets/uSockets/src/crypto/*.c",

      versions[i].abi, compiler, versions[i].name);

  run("%s" SHARED_MACRO UNIX_MACRO OPT_FLAGS SHARED_INCLUDE("./", "%s")
        "-std=c++20 -c src/pch.hpp -o targets/node-%s/pch.hpp.pch",

      cpp_compiler, versions[i].name, versions[i].name);
  END_FOREACH_NODEJS;

#undef SHARED_MACRO
#undef SHARED_INCLUDE
#undef UNIX_MACRO
}

void build(char *compiler, char *cpp_compiler, char *cpp_linker, char *os, const char *arch) {
  printf("\n<-- [Started building uWebSockets.js: %s] -->\n", arch);

#define SHARED_MACRO \
  " -DUWS_WITH_PROXY" \
  " -DLIBUS_USE_QUIC" \
  " -DLIBUS_USE_LIBUV" \
  " -DLIBUS_USE_OPENSSL" \
  " -DWIN32_LEAN_AND_MEAN" \
  " -DUWS_REMOTE_ADDRESS_USERSPACE"

#define SHARED_INCLUDE(node_version) \
  " -I uWebSockets/src" \
  " -I uWebSockets/uSockets/src" \
  " -I uWebSockets/uSockets/lsquic/include" \
  " -I uWebSockets/uSockets/lsquic/wincompat" \
  " -I uWebSockets/uSockets/boringssl/include" \
  " -I targets/node-" node_version "/include/node"

#if !defined(IS_WINDOWS)
#define UNIX_MACRO " -pthread -fPIC "
#define STATIC_LIB(lib) lib ".a "
#else 
#define UNIX_MACRO ""
#define STATIC_LIB(lib) " ./" lib ".lib "
#endif

/* Build for Unix systems */
  START_FOREACH_NODEJS(i);
    run("%s" OPT_FLAGS SHARED_MACRO SHARED_INCLUDE("%s")
        UNIX_MACRO
        " -std=c++20 "
        " -include-pch targets/node-%s/pch.hpp.pch "
        STATIC_LIB("uWebSockets/uSockets/boringssl/libssl")
        STATIC_LIB("uWebSockets/uSockets/boringssl/libcrypto")
        STATIC_LIB("uWebSockets/uSockets/lsquic/src/liblsquic/liblsquic")
        " -shared %s ./tmp/c-%s/*.o src/addon.cpp -o dist/uws_%s_%s_%s.node",

        cpp_compiler, versions[i].name,
        versions[i].name,
        cpp_linker, versions[i].abi, os, arch, versions[i].abi);
  END_FOREACH_NODEJS;

  printf("\n[Finished building uWebSockets.js: %s]\n", arch);

#undef SHARED_MACRO
#undef SHARED_INCLUDE
#undef UNIX_MACRO
#undef STATIC_LIB
}


#if defined(IS_WINDOWS)
/* Special case for windows */
void build(char *compiler, char *cpp_compiler, char *cpp_linker, char *os, const char *arch) {
  printf("\n<-- [Building uWebSockets.js] -->\n");
  char *c_shared =
      "-DWIN32_LEAN_AND_MEAN -DLIBUS_USE_LIBUV -DLIBUS_USE_QUIC "
      "-I../../uWebSockets/uSockets/lsquic/include "
      "-I../../uWebSockets/uSockets/lsquic/wincompat "
      "-I../../uWebSockets/uSockets/boringssl/include -DLIBUS_USE_OPENSSL -O3 -c "
      "-I../../uWebSockets/uSockets/src ../../uWebSockets/uSockets/src/*.c "
      "../../uWebSockets/uSockets/src/eventing/*.c "
      "../../uWebSockets/uSockets/src/crypto/*.c";
  char *cpp_shared =
      "-DWIN32_LEAN_AND_MEAN -DUWS_WITH_PROXY -DLIBUS_USE_LIBUV "
      "-DLIBUS_USE_QUIC -I../../uWebSockets/uSockets/lsquic/include "
      "-DUWS_REMOTE_ADDRESS_USERSPACE "
      "-I../../uWebSockets/uSockets/lsquic/wincompat "
      "-I../../uWebSockets/uSockets/boringssl/include -DLIBUS_USE_OPENSSL -O3 -c "
      "-std=c++20 -I../../uWebSockets/uSockets/src -I../../uWebSockets/src ../../src/addon.cpp "
      "../../uWebSockets/uSockets/src/crypto/sni_tree.cpp";

  for (unsigned int i = 0; i < versionsQuantity; i++) {
    run("cd \"tmp\\c-%s\" && %s %s -I../../targets/node-%s/include/node", versions[i].abi, compiler, c_shared,
        versions[i].name);
    run("cd \"tmp\\cpp-%s\" && %s %s -I../../targets/node-%s/include/node", versions[i].abi, cpp_compiler, cpp_shared,
        versions[i].name);
    run("%s -O3 " " ./tmp/c-%s/*.o ./tmp/cpp-%s/*.o " "./uWebSockets/uSockets/boringssl/%s/ssl.lib "
        "./uWebSockets/uSockets/boringssl/%s/crypto.lib "
        "./uWebSockets/uSockets/lsquic/src/liblsquic/Debug/lsquic.lib "
        "./targets/node-%s/node.lib -ladvapi32 -std=c++20 -shared -o dist/uws_win32_%s_%s.node ", cpp_compiler, versions[i].abi, versions[i].abi,
        arch, arch, versions[i].name, arch, versions[i].abi);
  }
  printf("\n[Finished building uWebSockets.js]\n");
}
#endif

int main(int argc, const char* argv[]) {
    threads_quantity = get_cpu_count();
    printf("<-- ENTRY POINT!!! -->\n[Parallel threads available: %i]\n", threads_quantity);
    
    const char *arch = X64;

#if !defined(CROSS_COMPILE_MACOS)

#if defined(__arm__)
    arch = ARM;
#elif defined(__aarch64__)
    arch = ARM64;
#endif

#endif



  if(argc == 1 || argc > 1 && !strcmp(argv[1], "deps")) {
    printf("<--[Fetching + Compiling dependencies]-->\n");
    prepare();
    /* for MacOS we compile one architecture at a time */
    build_boringssl(arch);
    build_lsquic(arch);
    build_uSockets_and_PCH("clang", "clang++");
    printf("\n[Finished fetching + compiling dependencies]\n");
    if (argc > 1) return 0;
  }

#ifdef IS_WINDOWS
    build_windows("clang -fms-runtime-lib=static",
          "clang++ -fms-runtime-lib=static",
          "-ladvapi32",
          OS,
          X64);
#elif defined(IS_MACOS)
    /* Try and build for arm64 macOS 12 */
    build("clang -target arm64-apple-macos12",
          "clang++ -stdlib=libc++ -target arm64-apple-macos12",
          "-undefined dynamic_lookup" MACOS_LINK_EXTRAS,
          OS,
          ARM64);

#else
    /* Linux does not cross-compile but picks whatever arch the host is on (we run on both x64 and ARM64) */
    build("clang",
          "clang++",
          LINUX_LINK_EXTRAS,
          OS,
          arch);
#endif
}
