@echo off
setlocal
cd /d "%~dp0"
title Online Examination Portal

echo ========================================================
echo   ONLINE EXAMINATION PORTAL - ONE-CLICK LAUNCHER
echo ========================================================
echo.
echo Launching Online Examination System...

REM Method 1: C++17 Native Backend Server (Primary)
if exist cpp-backend\server.exe (
    echo Starting C++17 Backend Server on port 5500...
    cd cpp-backend
    start "" server.exe 5500
    exit /b 0
)
if exist server.exe (
    echo Starting C++17 Backend Server on port 5500...
    start "" server.exe 5500
    exit /b 0
)

REM Method 2: Node.js Local Server
where node >nul 2>&1
if %ERRORLEVEL% equ 0 (
    echo Starting server with Node.js at http://localhost:5500 ...
    start "" "http://localhost:5500/index.html"
    node dev-server.js
    exit /b 0
)

REM Method 2: Direct Web Browser Launch
echo Opening directly in your default web browser...
start "" "%~dp0index.html"
exit /b 0

