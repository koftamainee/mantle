#include "world/chunk.h"

#include <mdspan>

namespace mantle {
    Chunk::VoxelSpan Chunk::voxels() { return VoxelSpan(m_voxels.data()); }

    Chunk::ConstVoxelSpan Chunk::voxels() const {
        return ConstVoxelSpan(m_voxels.data());
    }

    Chunk::VoxelArray &Chunk::voxel_array() { return m_voxels; }
} // namespace mantle
