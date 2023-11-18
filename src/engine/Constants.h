#pragma once
// RENDERING CONSTANTS
constexpr size_t RES_WIDTH  = 1920;
constexpr size_t RES_HEIGHT = 1080;

constexpr size_t MAX_QUAD_COUNT    = 2000;
constexpr size_t MAX_VERTEX_COUNT  = MAX_QUAD_COUNT * 4;
constexpr size_t MAX_INDEX_COUNT   = MAX_QUAD_COUNT * 6;
constexpr size_t MAX_TEXTURE_COUNT = 31;
constexpr size_t COUNT_UNDEFINED   = -1;