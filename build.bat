@echo off
setlocal
rem src\parse.c src\exec.c src\builtins.c src\util.c

if not exist build mkdir build

echo Compiling myshell...
zig cc src\main.c src\token.c src\env.c src\config.c -o build\myshell.exe -std=c99 -O2 -s -Wall -Wextra -Wpedantic


if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

echo Build complete: build\myshell.exe
endlocal
