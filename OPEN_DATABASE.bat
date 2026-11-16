\cdc@echo off
setlocal
cd /d "%~dp0"
title Open Project Database - Online Examination System

echo ========================================================
echo   OPEN PROJECT DATABASE (SQLite 3 - online_exam.db)
echo ========================================================
echo.

set "DB_PATH=%~dp0online_exam.db"

if not exist "%DB_PATH%" (
    if exist "%~dp0..\online_exam.db" (
        set "DB_PATH=%~dp0..\online_exam.db"
    )
)

echo [INFO] Database Technology: SQLite 3
echo [INFO] Actual Database File:
echo        %DB_PATH%
echo.

REM 1. Check Standard 64-bit Program Files
if exist "C:\Program Files\DB Browser for SQLite\DB Browser for SQLite.exe" (
    echo [OK] Launching DB Browser for SQLite...
    start "" "C:\Program Files\DB Browser for SQLite\DB Browser for SQLite.exe" "%DB_PATH%"
    exit /b 0
)

REM 2. Check 32-bit Program Files
if exist "C:\Program Files (x86)\DB Browser for SQLite\DB Browser for SQLite.exe" (
    echo [OK] Launching DB Browser for SQLite...
    start "" "C:\Program Files (x86)\DB Browser for SQLite\DB Browser for SQLite.exe" "%DB_PATH%"
    exit /b 0
)

REM 3. Check Local AppData Programs
if exist "%LOCALAPPDATA%\Programs\DB Browser for SQLite\DB Browser for SQLite.exe" (
    echo [OK] Launching DB Browser for SQLite...
    start "" "%LOCALAPPDATA%\Programs\DB Browser for SQLite\DB Browser for SQLite.exe" "%DB_PATH%"
    exit /b 0
)

REM 4. Check PATH for sqlitebrowser
where sqlitebrowser >nul 2>&1
if %ERRORLEVEL% equ 0 (
    echo [OK] Launching sqlitebrowser...
    start "" sqlitebrowser "%DB_PATH%"
    exit /b 0
)

REM 5. Fallback: If DB Browser is not installed
echo ========================================================
echo  DB Browser for SQLite is not installed on this system.
echo ========================================================
echo.
echo Your project's actual database file is located at:
echo %DB_PATH%
echo.
echo To open and view your tables (users, quizzes, questions, results):
echo   1. Download DB Browser for SQLite from:
echo      https://sqlitebrowser.org/dl/
echo   2. Run the installer.
echo   3. Re-run this launcher to open your database directly!
echo.
echo Opening database file in Windows File Explorer...
explorer.exe /select,"%DB_PATH%"
echo.
pause

