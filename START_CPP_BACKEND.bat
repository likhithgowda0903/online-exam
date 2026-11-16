@echo off
setlocal
cd /d "%~dp0"
title Online Examination System - C++ Backend Server

echo ========================================================
echo   ONLINE EXAMINATION SYSTEM - C++17 BACKEND SERVER
echo ========================================================
echo.

REM Check if server.exe is compiled in root or cpp-backend
if exist cpp-backend\server.exe (
    echo [RUNNING] Launching C++ Backend Server...
    cd cpp-backend
    start "" server.exe 5500
    exit /b 0
)

if exist server.exe (
    echo [RUNNING] Launching C++ Backend Server...
    start "" server.exe 5500
    exit /b 0
)

REM If not yet compiled, attempt automated build
echo [INFO] C++ backend executable not found. Attempting to build...
call cpp-backend\build.bat

if exist cpp-backend\server.exe (
    cd cpp-backend
    start "" server.exe 5500
    exit /b 0
)

echo.
echo [INFO] Opening Web Portal in Browser...
start "" "%~dp0index.html"
exit /b 0

