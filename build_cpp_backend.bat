@echo off
setlocal
cd /d "%~dp0"
title Build C++ Backend Server
call cpp-backend\build.bat
pause

