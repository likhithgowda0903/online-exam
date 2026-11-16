@echo off
echo ========================================================
echo  Online Examination System - Desktop App (C++ / Qt6)
echo ========================================================

if exist build\Release\OnlineExamSystem.exe (
    cd build\Release
    start OnlineExamSystem.exe
    exit /b 0
)

if exist build\OnlineExamSystem.exe (
    cd build
    start OnlineExamSystem.exe
    exit /b 0
)

echo [INFO] Native desktop binary not yet built.
echo.
echo To compile and run:
echo   1. Run 'build.bat' in this folder (requires CMake + Qt6/Qt5)
echo   2. Or open 'OnlineExamSystem.pro' in Qt Creator
echo.
pause

