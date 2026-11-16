@echo off
echo ========================================================
echo  Online Examination System - Build Script (C++ / CMake)
echo ========================================================

REM Check if CMake is installed
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] CMake is not found in your system PATH.
    echo Please install CMake or open OnlineExamSystem.pro in Qt Creator.
    pause
    exit /b 1
)

if not exist build (
    mkdir build
)

cd build
echo [INFO] Running CMake Configuration...
cmake -DCMAKE_BUILD_TYPE=Release ..
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

echo [INFO] Compiling Project Targets...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo [ERROR] Compilation failed.
    pause
    exit /b 1
)

echo.
echo ========================================================
echo  BUILD SUCCESSFUL!
echo ========================================================
echo Executables built in: build/
echo - OnlineExamSystem.exe (Main Application)
echo - test_exam_system.exe (Unit Tests)
echo.
pause
