@echo off
echo ========================================
echo  Fix Gradle Wrapper
echo ========================================
echo.
echo This script will fix a corrupted Gradle wrapper JAR
echo.

REM Check if PowerShell is available
powershell -Command "Write-Host 'PowerShell OK'" >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: PowerShell is required but not found
    pause
    exit /b 1
)

echo Removing corrupted gradle-wrapper.jar...
if exist gradle\wrapper\gradle-wrapper.jar (
    del /F /Q gradle\wrapper\gradle-wrapper.jar
    echo Old JAR removed
) else (
    echo No existing JAR found
)

echo.
echo Downloading fresh gradle-wrapper.jar...
powershell -Command "$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/gradle/gradle/v7.6.0/gradle/wrapper/gradle-wrapper.jar' -OutFile 'gradle\wrapper\gradle-wrapper.jar'"

if %errorlevel% neq 0 (
    echo ERROR: Download failed!
    echo Trying alternative source...
    powershell -Command "$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri 'https://github.com/gradle/gradle/raw/v7.6.0/gradle/wrapper/gradle-wrapper.jar' -OutFile 'gradle\wrapper\gradle-wrapper.jar'"
)

if not exist gradle\wrapper\gradle-wrapper.jar (
    echo ERROR: Could not download gradle-wrapper.jar
    echo.
    echo Manual fix:
    echo 1. Download from: https://github.com/gradle/gradle/raw/v7.6.0/gradle/wrapper/gradle-wrapper.jar
    echo 2. Save to: gradle\wrapper\gradle-wrapper.jar
    pause
    exit /b 1
)

echo.
echo Testing Gradle wrapper...
gradlew.bat --version

if %errorlevel% neq 0 (
    echo ERROR: Gradle wrapper test failed
    pause
    exit /b 1
)

echo.
echo ========================================
echo  SUCCESS!
echo ========================================
echo.
echo Gradle wrapper has been fixed successfully!
echo You can now build your project with: gradlew.bat assembleDebug
echo.

pause

