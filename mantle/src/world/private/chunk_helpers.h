#pragma once
#include "glm/vec3.hpp"
#include "world/chunk_storage_system.h"

namespace mantle {

inline void get_neighbors(const ChunkStorageSystem &storage, glm::ivec3 pos,
                          const Chunk *out[6]) {
    static const glm::ivec3 offsets[6] = {
        {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1},
    };
    for (u32 i = 0; i < 6; i++) {
        glm::ivec3 nb_pos = pos + offsets[i];
        out[i] = storage.has_chunk(nb_pos) ? &storage.get_chunk(nb_pos)
                                           : nullptr;
    }
}

}
