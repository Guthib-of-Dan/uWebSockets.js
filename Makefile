all:
	gcc build.c -o build.exe
	./build.exe || .\build.exe
prepare:
	gcc build.c -o build.exe
prepare-macos-x64:
	gcc build.c -DCROSS_COMPILE_MACOS -o build.exe
build-deps:
	./build.exe deps || .\build.exe deps
build-proj:
	./build.exe proj || .\build.exe proj
prebuilt-asan:
	gcc build.c -DWITH_ASAN -o build.exe
	./build.exe || .\build.exe
