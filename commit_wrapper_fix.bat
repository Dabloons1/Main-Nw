@echo off
echo ========================================
echo  Commit Gradle Wrapper Fix
echo ========================================
echo.

REM Try to find git
where git >nul 2>&1
if %errorlevel% neq 0 (
    echo Git command line not found in PATH.
    echo.
    echo Please use one of these methods to commit:
    echo.
    echo 1. GitHub Desktop - Open the app and commit/push
    echo 2. VS Code - Use Source Control panel
    echo 3. Install Git: https://git-scm.com/download/win
    echo.
    echo See COMMIT_FIXES.txt for detailed instructions.
    echo.
    pause
    exit /b 1
)

echo Git found! Checking repository...
echo.

git status >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Not a git repository
    echo Please initialize git first with: setup_github.bat
    pause
    exit /b 1
)

echo Current changes:
echo.
git status --short
echo.
echo ========================================
echo  Files to Commit
echo ========================================
echo.
echo ✓ gradle/wrapper/gradle-wrapper.jar (FIXED - 61KB)
echo ✓ .gitattributes (binary file protection)
echo ✓ .github/workflows/build-apk.yml (updated)
echo ✓ fix_gradle_wrapper.bat (helper script)
echo ✓ BUILDING.md (updated docs)
echo ✓ Other new files
echo.

set /p CONFIRM="Commit and push all fixes? (y/n): "
if /i not "%CONFIRM%"=="y" (
    echo Cancelled.
    pause
    exit /b 0
)

echo.
echo Adding files...
git add gradle/wrapper/gradle-wrapper.jar
git add .gitattributes
git add .github/workflows/build-apk.yml
git add fix_gradle_wrapper.bat
git add BUILDING.md
git add GRADLE_WRAPPER_FIX.md
git add COMMIT_FIXES.txt
git add .

if %errorlevel% neq 0 (
    echo ERROR: Failed to stage files
    pause
    exit /b 1
)

echo.
echo Committing...
git commit -m "Fix Gradle wrapper corruption + binary file protection"

if %errorlevel% neq 0 (
    echo ERROR: Commit failed
    pause
    exit /b 1
)

echo.
echo Pushing to GitHub...
git push

if %errorlevel% neq 0 (
    echo.
    echo ERROR: Push failed
    echo.
    echo This might be because:
    echo 1. You need to set up authentication
    echo 2. Remote repository not configured
    echo 3. Network issue
    echo.
    echo Try pushing manually or use GitHub Desktop
    pause
    exit /b 1
)

echo.
echo ========================================
echo  SUCCESS!
echo ========================================
echo.
echo ✓ Gradle wrapper fix committed
echo ✓ Changes pushed to GitHub
echo.
echo Next steps:
echo 1. Go to your GitHub repository
echo 2. Click "Actions" tab
echo 3. Wait for build to complete (3-5 min)
echo 4. Download APK from "Artifacts"
echo.
echo The build should now succeed!
echo.

pause

