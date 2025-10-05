#include <iostream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>

#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <thread>

#include <pthread.h>
#include <dirent.h>
#include <libgen.h>

#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/uio.h>

#include <fcntl.h>

#include <jni.h>
#include <android/log.h>

#define IS_DEBUG

// OBFUSCATE macro for string obfuscation
#define OBFUSCATE(str) str

// Global IL2CPP variable
extern uintptr_t G_IL2CPP;

// IL2CPP function declarations - Il2CppAttach is declared in Il2Cpp.h

#if defined(IS_DEBUG)
#define LOG_TAG "DarkTeam_LOG"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define LOGI(...)
#define LOGD(...)
#define LOGW(...)
#define LOGE(...)
#endif
