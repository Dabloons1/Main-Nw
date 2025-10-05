@echo off
echo ========================================
echo  LSPosed Mod Menu - GitHub Setup
echo ========================================
echo.

REM Check if git is installed
git --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Git is not installed!
    echo Please install Git from: https://git-scm.com/download/win
    pause
    exit /b 1
)

REM Check if already initialized
if exist .git (
    echo Git repository already initialized.
    echo Current remote:
    git remote -v
    echo.
    goto :push
)

echo Initializing Git repository...
git init
if %errorlevel% neq 0 (
    echo ERROR: Failed to initialize git repository
    pause
    exit /b 1
)

echo.
echo Adding files...
git add .
git commit -m "Initial commit - LSPosed Mod Menu with ImGui integration"

echo.
echo ========================================
echo  Configure GitHub Remote
echo ========================================
echo.
echo Please create a repository on GitHub first:
echo 1. Go to: https://github.com/new
echo 2. Create your repository (e.g., LSPosed-ModMenu)
echo 3. DO NOT initialize with README, .gitignore, or license
echo.

set /p GITHUB_URL="Enter your GitHub repository URL (e.g., https://github.com/username/repo.git): "

if "%GITHUB_URL%"=="" (
    echo ERROR: No URL provided
    pause
    exit /b 1
)

echo.
echo Adding remote origin: %GITHUB_URL%
git remote add origin %GITHUB_URL%

echo Setting default branch to 'main'...
git branch -M main

:push
echo.
echo ========================================
echo  Push to GitHub
echo ========================================
echo.
set /p PUSH_NOW="Push to GitHub now? (y/n): "

if /i "%PUSH_NOW%"=="y" (
    echo Pushing to GitHub...
    git push -u origin main
    
    if %errorlevel% equ 0 (
        echo.
        echo ========================================
        echo  SUCCESS!
        echo ========================================
        echo.
        echo Your code has been pushed to GitHub!
        echo.
        echo Next steps:
        echo 1. Go to your GitHub repository
        echo 2. Click on "Actions" tab
        echo 3. Wait for the build to complete (3-5 minutes)
        echo 4. Download the APK from "Artifacts"
        echo.
        echo To view your repo:
        git remote get-url origin
        echo.
    ) else (
        echo.
        echo ERROR: Push failed!
        echo.
        echo If authentication failed, you may need to:
        echo 1. Set up a Personal Access Token on GitHub
        echo 2. Use: git push -u origin main
        echo 3. Enter your GitHub username
        echo 4. Enter your Personal Access Token as password
        echo.
    )
) else (
    echo.
    echo Setup complete! To push later, run:
    echo     git push -u origin main
    echo.
)

pause
