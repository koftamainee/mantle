#pragma once
#include "core/memory/arena_allocator.h"
#include "core/types.h"
#include "glm/vec3.hpp"
#include "world/chunk.h"
#include "world/chunk_mesh_data.h"

namespace mantle::bgm {

ChunkMeshData mesh_chunk(const Chunk &chunk, const Chunk *neighbors[6],
                         glm::ivec3 chunk_pos, ArenaAllocator &scratch);

} // namespace mantle::bgm
