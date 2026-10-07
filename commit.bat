@echo off
setlocal
REM ============================================================
REM  commit.bat - save today's work to GitHub
REM  Repo: https://github.com/vijayanca/Rect12P_GETEK_v4
REM  Double-click this file, type a short note, press Enter.
REM ============================================================

cd /d "%~dp0"
title Commit Rect12P_GETEK_v4 to GitHub

where git >nul 2>nul
if errorlevel 1 (
    echo.
    echo  Git is not installed. Please install "Git for Windows" from
    echo  https://git-scm.com/download/win  and run this file again.
    echo.
    pause
    exit /b 1
)

REM ---------- One-time setup (runs only the first time) ----------
if not exist ".git" (
    echo.
    echo  First run: connecting this folder to GitHub...
    git init -q -b main
    git config user.name "Vijayan"
    git config user.email "vijayanca@gmail.com"
    git remote add origin https://github.com/vijayanca/Rect12P_GETEK_v4.git
    git fetch -q origin
    if errorlevel 1 (
        echo  Could not reach GitHub. Check the internet connection.
        rmdir /s /q .git
        pause
        exit /b 1
    )
    git reset -q origin/main
    git checkout -- .gitignore 2>nul
    git branch -q -u origin/main
    echo  Done. This folder is now linked to GitHub.
)

REM ---------- Show what changed ----------
echo.
echo  Changes since last commit:
echo  --------------------------
git status --short
git add -A
git diff --cached --quiet
if not errorlevel 1 (
    echo  ^(nothing changed^)
    echo.
    echo  Nothing to commit today.
    echo.
    pause
    exit /b 0
)

REM ---------- Commit ----------
echo.
set "MSG="
set /p "MSG=What did you change today? : "
if "%MSG%"=="" set "MSG=Daily update %date%"

git commit -q -m "%MSG%"
if errorlevel 1 (
    echo  Commit failed.
    pause
    exit /b 1
)

REM ---------- Push ----------
echo.
echo  Sending to GitHub...
git push -u origin main
if errorlevel 1 (
    echo.
    echo  *** Push failed. Your work IS saved locally as a commit. ***
    echo  Check the internet connection / GitHub sign-in and run this again.
    echo.
    pause
    exit /b 1
)

echo.
echo  Saved to GitHub:
git log --oneline -1
echo.
pause
