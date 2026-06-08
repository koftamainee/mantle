// Copyright (c) 2026 Mantle. All rights reserved.

#pragma once

#include "world/chunk.h"

namespace mantle {

    void init_chunk_light(Chunk &chunk);
    void propagate_chunk_light_level(Chunk &chunk, Chunk *neighbors[6], u8 level);

} // namespace mantle
