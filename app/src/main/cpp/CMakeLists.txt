cmake_minimum_required(VERSION 3.22.1)

project("xposedmodmenu")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compile definitions
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_CUSTOM)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/ImGui
    ${CMAKE_SOURCE_DIR}/ImGui/backends
    ${CMAKE_SOURCE_DIR}/Includes
    ${CMAKE_SOURCE_DIR}/DobbyHook
)

# ImGui source files
set(IMGUI_SOURCES
    ImGui/imgui.cpp
    ImGui/imgui_demo.cpp
    ImGui/imgui_draw.cpp
    ImGui/imgui_tables.cpp
    ImGui/imgui_widgets.cpp
    ImGui/backends/imgui_impl_opengl3.cpp
)

# ByNameModding sources
set(BYNAME_SOURCES
    Includes/fake_dlfcn.cpp
    Includes/Il2Cpp.cpp
    Includes/Tools.cpp
)

# Main native library
add_library(${CMAKE_PROJECT_NAME} SHARED
    ${IMGUI_SOURCES}
    ${BYNAME_SOURCES}
    ModMenu.cpp
    JNI_Bridge.cpp
    Hooks.cpp
)

# Link Dobby static library based on architecture
if(${ANDROID_ABI} STREQUAL "arm64-v8a")
    add_library(dobby STATIC IMPORTED)
    set_target_properties(dobby PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/DobbyHook/arm64-v8a/libdobby.a)
elseif(${ANDROID_ABI} STREQUAL "armeabi-v7a")
    add_library(dobby STATIC IMPORTED)
    set_target_properties(dobby PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/DobbyHook/armeabi-v7a/libdobby.a)
endif()

# Link libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    dobby
    android
    log
    EGL
    GLESv2
    GLESv3
)

