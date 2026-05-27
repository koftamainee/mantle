#pragma once
#include "core/types.h"

namespace mantle {
    struct Chunk {
        static constexpr u32 size = 32;
        static constexpr u32 volume = size * size * size;

        u32 voxels[volume];
    };
}