#ifndef MODMENU_H
#define MODMENU_H

#include <jni.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

#define LOG_TAG "LSPosedModMenu"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Menu state variables
extern bool IsMenuInitialized;
extern bool IsMenuVisible;

// Feature toggle variables (examples - add your own!)
extern bool IsUnlimitedDiamonds;
extern bool IsGodMode;
extern bool IsUnlimitedAmmo;
extern bool IsESPEnabled;
extern float ESPWidth;

// Menu functions
void SetupImGui();
void DrawModMenu();
void CleanupImGui();

// Hook functions
void InstallGameHooks();

#endif // MODMENU_H

