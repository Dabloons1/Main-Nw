#include <jni.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>
#include "ModMenu.h"
#include "Hooks.h"
#include "Includes.h"
#include "Il2Cpp.h"
#include "dobby.h"

#define LOG_TAG "LSPosedModMenu"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Hook variables
typedef EGLBoolean (*EglSwapBuffers_t)(EGLDisplay dpy, EGLSurface surface);
EglSwapBuffers_t original_eglSwapBuffers = nullptr;

// Touch handling structures (from Unity)
struct UnityEngine_Vector2_Fields {
    float x;
    float y;
};

struct UnityEngine_Vector2_o {
    UnityEngine_Vector2_Fields fields;
};

enum TouchPhase {
    Began = 0,
    Moved = 1,
    Stationary = 2,
    Ended = 3,
    Canceled = 4
};

struct UnityEngine_Touch_Fields {
    int32_t m_FingerId;
    struct UnityEngine_Vector2_o m_Position;
    struct UnityEngine_Vector2_o m_RawPosition;
    struct UnityEngine_Vector2_o m_PositionDelta;
    float m_TimeDelta;
    int32_t m_TapCount;
    int32_t m_Phase;
    int32_t m_Type;
    float m_Pressure;
    float m_maximumPossiblePressure;
    float m_Radius;
    float m_fRadiusVariance;
    float m_AltitudeAngle;
    float m_AzimuthAngle;
};

// Hooked eglSwapBuffers function
EGLBoolean hooked_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    static int frameCount = 0;
    frameCount++;
    
    if (frameCount == 1) {
        LOGI("=== eglSwapBuffers HOOKED - First frame! ===");
    }
    
    if (!IsMenuInitialized) {
        LOGI("Setting up ImGui on frame %d", frameCount);
        SetupImGui();
    }

    ImGuiIO &io = ImGui::GetIO();
    
    // Handle touch input (if Unity game)
    static bool should_clear_mouse_pos = false;
    static int touchCount = 0;
    
    if (G_IL2CPP) {
        int (*TouchCount)(void*) = (int (*)(void*)) (Il2CppGetMethodOffset(
            OBFUSCATE("UnityEngine.dll"), 
            OBFUSCATE("UnityEngine"), 
            OBFUSCATE("Input"), 
            OBFUSCATE("get_touchCount"), 0));
        
        if (TouchCount) {
            int touchCount = TouchCount(nullptr);
            if (touchCount > 0) {
                UnityEngine_Touch_Fields touch = ((UnityEngine_Touch_Fields (*)(int)) (
                    Il2CppGetMethodOffset(
                        OBFUSCATE("UnityEngine.dll"), 
                        OBFUSCATE("UnityEngine"), 
                        OBFUSCATE("Input"), 
                        OBFUSCATE("GetTouch"), 1))) (0);
                
                float reverseY = io.DisplaySize.y - touch.m_Position.fields.y;

                switch (touch.m_Phase) {
                    case TouchPhase::Began:
                    case TouchPhase::Stationary:
                        io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
                        io.MouseDown[0] = true;
                        break;
                    case TouchPhase::Ended:
                    case TouchPhase::Canceled:
                        io.MouseDown[0] = false;
                        should_clear_mouse_pos = true;
                        
                        // Simple gesture: tap top-left corner to toggle menu
                        if (touch.m_Position.fields.x < 100 && touch.m_Position.fields.y < 100) {
                            IsMenuVisible = !IsMenuVisible;
                            LOGI("Menu toggled: %s", IsMenuVisible ? "visible" : "hidden");
                        }
                        break;
                    case TouchPhase::Moved:
                        io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
                        break;
                    default:
                        break;
                }
            } else {
                io.MouseDown[0] = false;
            }
        }
    } else {
        io.MouseDown[0] = false;
    }
    
    // Fallback: Simple keyboard input for non-Unity games
    // You can add more input methods here if needed
    static bool lastMouseState = false;
    if (io.MouseDown[0] && !lastMouseState) {
        // Simple click to toggle menu
        IsMenuVisible = !IsMenuVisible;
        LOGI("Menu toggled via mouse: %s", IsMenuVisible ? "visible" : "hidden");
    }
    lastMouseState = io.MouseDown[0];

    // Draw ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    DrawModMenu();
    ImGui::Render();
    
    if (frameCount % 60 == 0) { // Log every 60 frames (about once per second)
        LOGI("Rendering frame %d, menu visible: %d, initialized: %d", 
             frameCount, IsMenuVisible, IsMenuInitialized);
        
        // Check if Il2Cpp is ready and install hooks if needed
        if (G_IL2CPP && Il2CppIsAssembliesLoaded()) {
            static bool hooksInstalled = false;
            if (!hooksInstalled) {
                LOGI("Il2Cpp is ready - installing game hooks now");
                InstallGameHooks();
                hooksInstalled = true;
            }
        }
    }
    
    glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
    
    if (should_clear_mouse_pos) {
        io.MousePos = ImVec2(-1, -1);
        should_clear_mouse_pos = false;
    }

    return original_eglSwapBuffers(dpy, surface);
}

// Initialize hooks
void* hook_thread(void*) {
    LOGI("Hook thread started");
    
    // Wait for libil2cpp.so to load (Unity game)
    const char* targetLib = "libil2cpp.so";
    LOGI("Waiting for Il2Cpp to load...");
    
    int attempts = 0;
    const int maxAttempts = 30; // Wait up to 30 seconds
    
    while (!G_IL2CPP && attempts < maxAttempts) {
        G_IL2CPP = (uintptr_t)dlopen(targetLib, RTLD_NOLOAD);
        if (!G_IL2CPP) {
            void* handle = dlopen(targetLib, RTLD_NOW);
            if (handle) {
                G_IL2CPP = (uintptr_t)handle;
                LOGI("Found %s at 0x%lx (attempt %d)", targetLib, G_IL2CPP, attempts + 1);
                break;
            }
        } else {
            LOGI("Found existing %s at 0x%lx (attempt %d)", targetLib, G_IL2CPP, attempts + 1);
            break;
        }
        
        attempts++;
        LOGI("Il2Cpp not found yet, waiting... (attempt %d/%d)", attempts, maxAttempts);
        sleep(1);
    }
    
    if (!G_IL2CPP) {
        LOGE("Failed to find Il2Cpp after %d attempts - game hooks may not work", maxAttempts);
    }
    
    // Hook eglSwapBuffers for rendering
    void* eglSwapBuffers_addr = dlsym(RTLD_DEFAULT, "eglSwapBuffers");
    if (eglSwapBuffers_addr) {
        LOGI("Found eglSwapBuffers at %p", eglSwapBuffers_addr);
        
        if (DobbyHook(eglSwapBuffers_addr, 
                      (void*)hooked_eglSwapBuffers, 
                      (void**)&original_eglSwapBuffers) == 0) {
            LOGI("Successfully hooked eglSwapBuffers!");
        } else {
            LOGE("Failed to hook eglSwapBuffers");
        }
    } else {
        LOGE("Could not find eglSwapBuffers");
    }
    
    // Initialize Il2Cpp if available
    if (G_IL2CPP) {
        LOGI("Initializing Il2Cpp...");
        Il2CppAttach();
        LOGI("Il2Cpp attached successfully");
        
        // Wait a bit for Il2Cpp to fully initialize
        LOGI("Waiting for Il2Cpp to fully initialize...");
        sleep(3);
        
        // Check if Il2Cpp is ready
        if (Il2CppIsAssembliesLoaded()) {
            LOGI("Il2Cpp assemblies loaded - installing game hooks");
            InstallGameHooks();
        } else {
            LOGI("Il2Cpp assemblies not ready yet - hooks will be installed later");
        }
    } else {
        LOGE("Il2Cpp not found - game hooks will not work");
    }
    
    return nullptr;
}

// JNI function to initialize the mod menu
extern "C" JNIEXPORT void JNICALL
Java_shaizuro_xposedmenu_sti_NativeLib_initModMenu(JNIEnv* env, jobject thiz) {
    LOGI("=== NATIVE LIBRARY LOADED ===");
    LOGI("initModMenu called from Java");
    
    static bool initialized = false;
    if (!initialized) {
        LOGI("Creating hook thread...");
        pthread_t thread;
        int result = pthread_create(&thread, nullptr, hook_thread, nullptr);
        if (result == 0) {
            pthread_detach(thread);
            initialized = true;
            LOGI("Hook thread created successfully");
        } else {
            LOGE("Failed to create hook thread: %d", result);
        }
    } else {
        LOGI("Hook thread already initialized");
    }
}

// JNI function to show/hide menu
extern "C" JNIEXPORT void JNICALL
Java_shaizuro_xposedmenu_sti_NativeLib_setMenuVisible(JNIEnv* env, jobject thiz, jboolean visible) {
    IsMenuVisible = visible;
    LOGI("Menu visibility set to: %d", visible);
}

// JNI function to check if menu is initialized
extern "C" JNIEXPORT jboolean JNICALL
Java_shaizuro_xposedmenu_sti_NativeLib_isMenuInitialized(JNIEnv* env, jobject thiz) {
    return IsMenuInitialized;
}

// JNI_OnLoad - called when library is loaded
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("=== JNI_OnLoad called - Native library loaded! ===");
    LOGI("JavaVM pointer: %p", vm);
    return JNI_VERSION_1_6;
}

