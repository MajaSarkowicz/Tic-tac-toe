@echo off
echo Rozpoczynam kompilacje za pomoca GCC (MinGW)...

:: Opcje kompilatora (C++17)
set CXX_FLAGS=-std=c++17 -Wall -O2

:: Sciezki do folderow vcpkg (wersja dla MinGW)
set INCLUDES=-I".\vcpkg\installed\x64-mingw-dynamic\include"
set LIB_PATHS=-L".\vcpkg\installed\x64-mingw-dynamic\lib"

:: Biblioteki do zlinkowania (kolejnosc ma znaczenie w GCC!)
set LIBS=-limgui -lglfw3dll -lopengl32 -lgdi32 -luser32 -lshell32

:: Wywolanie kompilatora
g++ %CXX_FLAGS% main.cpp Board.cpp AI.cpp GameApp.cpp %INCLUDES% %LIB_PATHS% %LIBS% -o GraMinimax.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Sukces! Przygotowuje wymagane biblioteki runtime...
    set "VCPKG_BIN=%~dp0vcpkg\installed\x64-mingw-dynamic\bin"
    set "MINGW_BIN=C:\msys64\mingw64\bin"
    set "PATH=%VCPKG_BIN%;%MINGW_BIN%;%PATH%"
    if exist "%VCPKG_BIN%\glfw3.dll" copy /y "%VCPKG_BIN%\glfw3.dll" "%~dp0" >nul
    if exist "%MINGW_BIN%\libgcc_s_seh-1.dll" copy /y "%MINGW_BIN%\libgcc_s_seh-1.dll" "%~dp0" >nul
    if exist "%MINGW_BIN%\libstdc++-6.dll" copy /y "%MINGW_BIN%\libstdc++-6.dll" "%~dp0" >nul
    if exist "%MINGW_BIN%\libwinpthread-1.dll" copy /y "%MINGW_BIN%\libwinpthread-1.dll" "%~dp0" >nul
    echo.
    echo Sukces! Uruchamianie gry...
    GraMinimax.exe
) else (
    echo.
    echo Wystapil blad kompilacji. Sprawdz komunikaty powyzej.
)