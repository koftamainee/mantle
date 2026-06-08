// Copyright (c) 2026 Mantle. All rights reserved.

#pragma once

#include "core/types.h"

namespace mantle {

    struct MeshVertex {
        f32 x, y, z;
        u32 packed;
    };
    static_assert(sizeof(MeshVertex) == 16);

    struct ChunkMeshData {
        MeshVertex *vertices = nullptr;
        u32        *indices = nullptr;
        u32         quad_count = 0;

        u32 vertex_count() const noexcept { return quad_count * 4; }
        u32 index_count() const noexcept { return quad_count * 6; }

        usize vertex_bytes() const noexcept { return vertex_count() * sizeof(MeshVertex); }
        usize index_bytes() const noexcept { return index_count() * sizeof(u32); }

        bool empty() const noexcept { return quad_count == 0; }
    };

} // namespace mantle
