@echo off
setlocal
cd /d "%~dp0"
echo ========================================================
echo  Online Examination System - Application Launcher
echo ========================================================
echo.

if exist desktop-app\build\Release\OnlineExamSystem.exe (
    cd desktop-app\build\Release
    start OnlineExamSystem.exe
    exit /b 0
)

if exist desktop-app\build\OnlineExamSystem.exe (
    cd desktop-app\build
    start OnlineExamSystem.exe
    exit /b 0
)

echo [INFO] Starting Online Examination Portal...
call START_WEBSITE.bat
