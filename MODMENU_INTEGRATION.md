# ImGui Mod Menu Integration Guide

## Overview
This LSPosed module now includes a native ImGui-based mod menu system integrated from the Android Zygisk64 project. The menu renders directly on top of games/apps using OpenGL hooking.

## Architecture

### Components
1. **Native Layer (C++)**
   - ImGui library for menu rendering
   - Dobby hooking framework for function hooking
   - Il2Cpp helpers for Unity game modding
   - `eglSwapBuffers` hook for rendering the menu overlay

2. **Java Layer**
   - `NativeLib.java` - JNI bridge to native code
   - `MainHook.java` - Xposed hook entry point

3. **Menu Features**
   - Animated background with green net effect
   - Rainbow-bordered tabs
   - Three tabs: Visual, Memory, Settings
   - Example toggles for various game modifications

## How It Works

1. **Module Loads**: When LSPosed loads the module into a target app, `MainHook` is executed
2. **Activity Hook**: The module hooks `Activity.onCreate`
3. **Native Init**: On first activity, it calls `NativeLib.initModMenu()` which starts a native thread
4. **EGL Hook**: The native thread hooks `eglSwapBuffers` (OpenGL rendering function)
5. **Menu Rendering**: Every frame, ImGui menu is drawn on top of the game

## Configuration

### Target Package
Edit `MainHook.java` line 16:
```java
private static final String TARGET_PACKAGE = ""; // Leave empty for all apps
// OR
private static final String TARGET_PACKAGE = "com.example.game"; // Target specific game
```

### Adding Custom Hooks

#### For Unity Il2Cpp Games:

1. **Find the method to hook** using Il2CppDumper or similar tools
2. **Edit `Hooks.h`** and add your hook:

```cpp
// Example: Hook currency getter
int (*old_GetCoins)(void *instance) = nullptr;

int GetCoins_Hook(void *instance) {
    if (instance != nullptr) {
        extern bool IsUnlimitedCoins; // Your toggle variable
        if (IsUnlimitedCoins) {
            return 999999;
        }
    }
    return old_GetCoins(instance);
}
```

3. **Install the hook** in `InstallGameHooks()` function:

```cpp
void InstallGameHooks() {
    void* coinsMethod = Il2CppGetMethodOffset(
        OBFUSCATE("Assembly-CSharp.dll"),  // DLL name
        OBFUSCATE("GameData"),              // Namespace
        OBFUSCATE("Player"),                // Class
        OBFUSCATE("GetCoins"),              // Method
        0                                   // Parameter count
    );
    
    if (coinsMethod) {
        DobbyHook(coinsMethod, (void*)GetCoins_Hook, (void**)&old_GetCoins);
        LOGI("Hooked GetCoins");
    }
}
```

4. **Add toggle to menu** in `ModMenu.cpp`:

```cpp
// Declare at top with other toggles
bool IsUnlimitedCoins = false;

// Add to menu in DrawModMenu()
ImGui::Checkbox("Unlimited Coins", &IsUnlimitedCoins);
```

### Menu Customization

#### Change Menu Title
Edit `ModMenu.cpp` line 312:
```cpp
if (ImGui::Begin(ICON_FA_GAMEPAD " Your Mod Name Here")) {
```

#### Add New Tab
1. Increase `tabCount` in `ModMenu.cpp`
2. Add tab button in the loop
3. Add tab content with `if (activeTab == X)` block

#### Change Colors/Theme
Edit the `SetupImGui()` function in `ModMenu.cpp` (lines 127-170)

## Building

### Requirements
- Android NDK r21 or higher
- CMake 3.22.1
- Gradle 7.4.2+

### Build Command
```bash
cd "C:\LSPosed Module Project"
gradlew assembleDebug
```

Or use the provided batch script:
```bash
simple_build.bat
```

### Output
APK will be in: `app/build/outputs/apk/debug/app-debug.apk`

## Installation

1. Build the APK
2. Install on your device
3. Enable in LSPosed Manager
4. Select target application(s)
5. Restart the target app
6. Menu should appear automatically

## Debugging

### View Logs
```bash
adb logcat | findstr "LSPosedModMenu"
```

Or in LSPosed Manager:
- Open module in LSPosed
- Go to "Logs" tab
- Filter by "LSPosedModMenu"

### Common Issues

#### Menu Doesn't Appear
- Check if native library loaded: Look for "Native library loaded!" in logs
- Check if hooks installed: Look for "Successfully hooked eglSwapBuffers!"
- Ensure target app uses OpenGL (not Vulkan)

#### Crashes on Launch
- Check target architecture matches (arm64-v8a or armeabi-v7a)
- Verify Il2Cpp game (if using Il2Cpp hooks)
- Check logcat for crash details

#### Menu Shows but Toggles Don't Work
- Verify hooks are installed (check logs)
- Ensure method signatures match your target game
- Use Il2CppDumper to get correct method names

## File Structure

```
app/src/main/
├── cpp/
│   ├── CMakeLists.txt          # Build configuration
│   ├── ModMenu.h               # Menu header
│   ├── ModMenu.cpp             # Menu implementation
│   ├── JNI_Bridge.cpp          # Java-C++ bridge
│   ├── Hooks.h                 # Game hook definitions
│   ├── ImGui/                  # ImGui library
│   ├── DobbyHook/              # Hooking library
│   └── Includes/               # Il2Cpp helpers
└── java/
    └── com/example/xposedmodule/
        ├── MainHook.java       # Xposed entry point
        └── NativeLib.java      # JNI interface
```

## Credits

- **ImGui** - Dear ImGui library
- **Dobby** - Hooking framework
- **Android Zygisk64** - Original menu implementation
- **LSPosed** - Xposed framework

## Notes

- This menu works best with Unity Il2Cpp games
- For non-Unity games, you'll need to adapt the hooking approach
- Some games may use Vulkan instead of OpenGL (menu won't work)
- Encrypted/obfuscated games may require additional reverse engineering

## Support

For issues or questions:
1. Check LSPosed logs
2. Check logcat output
3. Verify game is compatible (Unity + OpenGL)
4. Ensure correct architecture (arm64/arm32)

