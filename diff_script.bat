@echo off

REM Verify we're inside a Git repository
git rev-parse --is-inside-work-tree >nul 2>&1
if errorlevel 1 (
    echo Error: Current directory is not a Git repository.
    pause
    exit /b 1
)

REM Write git diff output to output.txt
git diff > output.txt

echo Git diff written to output.txt