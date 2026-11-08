@echo off
rem Builds moviescale.bin (flat code for VA 00ED0000) and moviescale.sym from moviescale.c.
setlocal
set BIN=G:\msys64\mingw64\bin
cd /d "%~dp0"
"%BIN%\gcc.exe" -m32 -march=i686 -mno-sse -O2 -ffreestanding -fno-builtin -fno-tree-loop-distribute-patterns -fno-asynchronous-unwind-tables -fno-ident -fno-stack-protector -c moviescale.c -o moviescale.o || exit /b 1
"%BIN%\ld.exe" -m i386pe --image-base=0 -T moviescale.ld -o moviescale.elf moviescale.o || exit /b 1
"%BIN%\objcopy.exe" -O binary -j .text moviescale.elf moviescale.bin || exit /b 1
"%BIN%\nm.exe" moviescale.elf > moviescale.sym
