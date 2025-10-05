# Building the LSPosed Mod Menu

This guide explains how to build the APK locally and on GitHub.

---

## 🌐 Building on GitHub (Easiest Method)

GitHub Actions will automatically build your APK when you push code.

### Initial Setup

1. **Push your code to GitHub:**
   ```bash
   cd "C:\LSPosed Module Project"
   git init
   git add .
   git commit -m "Initial commit - LSPosed Mod Menu with ImGui"
   git branch -M main
   git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO.git
   git push -u origin main
   ```

2. **Enable GitHub Actions:**
   - Go to your GitHub repository
   - Click on "Actions" tab
   - If prompted, click "I understand my workflows, go ahead and enable them"

3. **Build automatically:**
   - Every push to `main`, `master`, or `develop` branches triggers a build
   - Or click "Actions" → "Build APK" → "Run workflow" to build manually

4. **Download built APK:**
   - Go to "Actions" tab
   - Click on the latest successful workflow run
   - Scroll down to "Artifacts"
   - Download "LSPosed-ModMenu-Debug" or "LSPosed-ModMenu-Release"

### Creating Releases

To create a GitHub release with your APK:

```bash
git tag v1.0
git push origin v1.0
```

The APK will automatically be attached to the release!

---

## 💻 Building Locally (Advanced)

### Prerequisites

1. **Android Studio** (or at minimum, Android SDK + NDK)
   - Download from: https://developer.android.com/studio

2. **Java Development Kit (JDK) 11**
   - Check: `java -version`

3. **Android SDK & NDK**
   - SDK Location: Usually `C:\Users\YourName\AppData\Local\Android\Sdk`
   - NDK: Install via Android Studio SDK Manager

### Setup Steps

1. **Create `local.properties` file:**
   ```bash
   copy local.properties.template local.properties
   notepad local.properties
   ```

2. **Edit `local.properties` with your SDK path:**
   
   **Windows:**
   ```properties
   sdk.dir=C\:\\Users\\YourUsername\\AppData\\Local\\Android\\Sdk
   ```
   
   **Linux/Mac:**
   ```properties
   sdk.dir=/home/username/Android/Sdk
   ```
   
   ⚠️ **Important:** Use double backslashes `\\` on Windows!

3. **Install NDK and CMake:**
   - Open Android Studio
   - Go to: Tools → SDK Manager → SDK Tools
   - Check:
     - ✅ NDK (Side by side) version 25.x
     - ✅ CMake version 3.22.1
   - Click "Apply" to install

### Building Commands

#### Using Gradle Wrapper (Recommended)

**Debug Build:**
```bash
cd "C:\LSPosed Module Project"
.\gradlew.bat assembleDebug
```

**Release Build:**
```bash
.\gradlew.bat assembleRelease
```

#### Using Provided Scripts

**Simple Build:**
```bash
.\simple_build.bat
```

**Full Build with Logs:**
```bash
.\build_apk.bat
```

### Build Outputs

APKs are located at:
- **Debug:** `app\build\outputs\apk\debug\app-debug.apk`
- **Release:** `app\build\outputs\apk\release\app-release.apk`

---

## 🔧 Troubleshooting

### "no main manifest attribute, in gradle/wrapper/gradle-wrapper.jar"
**Cause:** The Gradle wrapper JAR is corrupted (common when committing binary files)

**Solution:** Run the fix script:
```bash
fix_gradle_wrapper.bat
```

Or manually:
```bash
# Delete corrupted JAR
Remove-Item gradle\wrapper\gradle-wrapper.jar

# Download fresh copy
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/gradle/gradle/v7.6.0/gradle/wrapper/gradle-wrapper.jar" -OutFile "gradle\wrapper\gradle-wrapper.jar"
```

### "SDK location not found"
**Solution:** Create `local.properties` file with correct SDK path (see above)

### "NDK not found" or CMake errors
**Solution:** 
```bash
# Via command line (if SDK tools are in PATH)
sdkmanager "ndk;25.1.8937393"
sdkmanager "cmake;3.22.1"

# Or via Android Studio: Tools → SDK Manager → SDK Tools
```

### "Execution failed for task ':app:externalNativeBuildDebug'"
**Possible causes:**
1. NDK not installed
2. CMakeLists.txt error
3. Missing source files

**Solution:** Check the error log and ensure all ImGui files are copied correctly

### Build succeeds but native library doesn't load
**Check:**
1. All `.cpp` files are listed in `CMakeLists.txt`
2. Dobby library (`.a` files) exists in `DobbyHook/arm64-v8a/` and `armeabi-v7a/`
3. ImGui files are in `cpp/ImGui/`

---

## 📱 Installing & Testing

1. **Install APK:**
   ```bash
   adb install -r app-debug.apk
   ```

2. **Enable in LSPosed:**
   - Open LSPosed Manager
   - Enable your module
   - Select target app
   - Force stop target app

3. **Check logs:**
   ```bash
   adb logcat | findstr "LSPosedModMenu"
   ```

4. **Test the menu:**
   - Launch target app
   - Menu should appear automatically
   - Look for "LSPosed Mod Menu Active" toast

---

## 🚀 Quick Start (GitHub Build)

If you just want to build ASAP:

```bash
# 1. Initialize git (if not already done)
git init

# 2. Add all files
git add .

# 3. Commit
git commit -m "LSPosed Mod Menu - Initial Release"

# 4. Create repo on GitHub, then:
git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO.git
git branch -M main
git push -u origin main

# 5. Wait 3-5 minutes, then check GitHub Actions tab for your APK!
```

---

## 📦 What Gets Built

Your module includes:
- ✅ ImGui-based visual menu system
- ✅ Native C++ hooking with Dobby
- ✅ Il2Cpp support for Unity games
- ✅ OpenGL rendering hook (eglSwapBuffers)
- ✅ Touch input handling
- ✅ Example game hooks (customizable)

---

## 🎯 Next Steps

After building:
1. Read `MODMENU_INTEGRATION.md` for customization
2. Edit `MainHook.java` to set your target package
3. Edit `Hooks.h` to add game-specific hooks
4. Customize menu in `ModMenu.cpp`

---

## 📝 Notes

- GitHub builds are **slower** but **easier** (no local setup needed)
- Local builds are **faster** for iteration during development
- Release builds are **signed with debug key** (change for production)
- Native libraries are built for **arm64-v8a** and **armeabi-v7a** only

---

## ❓ Need Help?

1. Check build logs: `gradlew assembleDebug --stacktrace --info`
2. Check GitHub Actions logs: Actions tab → Latest run → Click on jobs
3. Verify all files copied: `ls app/src/main/cpp/ImGui/`
4. Check NDK path: `echo %ANDROID_NDK_HOME%`

