#ifndef DEFS_H
#define DEFS_H

#if defined(PLATFORM_WEB)
    #define CUSTOM_MODAL_DIALOGS            // Force custom modal dialogs usage
    #include <emscripten/emscripten.h>      // Emscripten library - LLVM to JavaScript compiler
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <vector>
#include <random>

// Defines and Macros
// Simple log system to avoid printf() calls if required
// NOTE: Avoiding those calls, also avoids const strings memory usage
#define SUPPORT_LOG_INFO
#if defined(SUPPORT_LOG_INFO)
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...)
#endif

#define WAVE_HEIGHT 55
#define WAVE_WIDTH 50
#define WAVE_OFFSET 30
#define MAX_OFFSET 400
#define WAVE_SPEED 80
#define BOAT_WIDTH 80
#define FISH_WIDTH 80
#define MINE_WIDTH 80
#define NUM_WAVES 10

#endif
