default:
	gcc build.c -o build.exe
	./build.exe || .\build.exe
prebuilt-asan:
	gcc build.c -DWITH_ASAN -o build.exe
	./build.exe || .\build.ex
