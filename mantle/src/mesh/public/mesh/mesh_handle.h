#pragma once
#include <core/types.h>

namespace mantle {
    struct MeshHandle {
        u32 id = 0;
        u32 generation = 0;

        bool is_valid() const { return generation != 0; }
    };
}