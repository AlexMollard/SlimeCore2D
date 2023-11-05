#pragma once
#include "ResourceManager.h"
#define GLEW_STATIC

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

// RENDERING CONSTANTS
constexpr size_t MAX_QUAD_COUNT = 2000;
constexpr size_t MAX_VERTEX_COUNT = MAX_QUAD_COUNT * 4;
constexpr size_t MAX_INDEX_COUNT = MAX_QUAD_COUNT * 6;
constexpr size_t MAX_TEXTURE_COUNT = 31;