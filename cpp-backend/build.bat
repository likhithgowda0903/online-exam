@echo off
setlocal
cd /d "%~dp0"

echo ========================================================
echo  Online Examination System - C++ Backend Compiler
echo ========================================================
echo.

where g++ >nul 2>&1
if %ERRORLEVEL% equ 0 goto COMPILE_GCC

where cl >nul 2>&1
if %ERRORLEVEL% equ 0 goto COMPILE_MSVC

where clang++ >nul 2>&1
if %ERRORLEVEL% equ 0 goto COMPILE_CLANG

echo [INFO] No C++ compiler (g++, cl, or clang++) was found in system PATH.
echo.
echo To compile the C++ Backend Server:
echo   Option A (MinGW / GCC):
echo     g++ -std=c++17 -O2 server.cpp -o server.exe -lws2_32 -ladvapi32
echo.
echo   Option B (Microsoft Visual C++):
echo     cl /std:c++17 /O2 /EHsc server.cpp ws2_32.lib advapi32.lib /Fe:server.exe
echo.
exit /b 1

:COMPILE_GCC
echo [FOUND] MinGW G++ Compiler detected.
echo [BUILD] Compiling server.cpp with C++17 ...
g++ -std=c++17 -O2 server.cpp -o server.exe -lws2_32 -ladvapi32
if %ERRORLEVEL% equ 0 (
    echo.
    echo [SUCCESS] C++ Backend Server built successfully: server.exe
    exit /b 0
)
echo [ERROR] G++ compilation failed.
exit /b 1

:COMPILE_MSVC
echo [FOUND] Microsoft Visual C++ compiler detected.
echo [BUILD] Compiling server.cpp with C++17 ...
cl /std:c++17 /O2 /EHsc server.cpp ws2_32.lib advapi32.lib /Fe:server.exe
if %ERRORLEVEL% equ 0 (
    echo.
    echo [SUCCESS] C++ Backend Server built successfully: server.exe
    exit /b 0
)
echo [ERROR] MSVC compilation failed.
exit /b 1

:COMPILE_CLANG
echo [FOUND] Clang++ Compiler detected.
echo [BUILD] Compiling server.cpp with C++17 ...
clang++ -std=c++17 -O2 server.cpp -o server.exe -lws2_32 -ladvapi32
if %ERRORLEVEL% equ 0 (
    echo.
    echo [SUCCESS] C++ Backend Server built successfully: server.exe
    exit /b 0
)
echo [ERROR] Clang compilation failed.
exit /b 1
