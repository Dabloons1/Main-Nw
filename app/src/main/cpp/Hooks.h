#pragma once

#include <jni.h>
#include <android/log.h>

// Example hook for Int method (Unity Il2Cpp games)
// This is referenced by the "Unlimited Diamonds" toggle in the menu

// Declarations for hook function pointers
extern int (*old_GetHardCurrency)(void *instance);
extern float (*old_GetHealth)(void *instance);
extern int (*old_GetAmmo)(void *instance);

// Declarations for hook functions
int GetHardCurrency_Hook(void *instance);
float GetHealth_Hook(void *instance);
int GetAmmo_Hook(void *instance);

// Declarations for install function
void InstallGameHooks();
