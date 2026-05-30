import bindings;

[[vk::binding(STORAGE_BUFFER_BINDING, 0)]]
RWStructuredBuffer<uint> g_storage_buffers[];

[[vk::push_constant]]
cbuffer PushConstants {
    uint chunk_buffer_index;
    uint metadata_buffer_index;
    uint vertex_buffer_index;
    uint index_buffer_index;
    uint indirect_buffer_index;
    uint counter_buffer_index;
    uint max_faces;
};

static const uint CHUNK_SIZE = 32;
static const uint CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
static const uint CHUNK_UINTS = CHUNK_VOLUME / 2;

static const float VOXEL_SCALE = 0.1;

static const int3 face_offsets[6] = {
    int3(1, 0, 0),
    int3(-1, 0, 0),
    int3(0, 1, 0),
    int3(0, -1, 0),
    int3(0, 0, 1),
    int3(0, 0, -1),
};

static const float3 face_vertices[6][4] = {
    { float3(1, 0, 0), float3(1, 1, 0), float3(1, 1, 1), float3(1, 0, 1) },
    { float3(0, 0, 1), float3(0, 1, 1), float3(0, 1, 0), float3(0, 0, 0) },
    { float3(0, 1, 1), float3(1, 1, 1), float3(1, 1, 0), float3(0, 1, 0) },
    { float3(0, 0, 1), float3(0, 0, 0), float3(1, 0, 0), float3(1, 0, 1) },
    { float3(0, 0, 1), float3(1, 0, 1), float3(1, 1, 1), float3(0, 1, 1) },
    { float3(1, 0, 0), float3(0, 0, 0), float3(0, 1, 0), float3(1, 1, 0) },
};

uint read_voxel(uint chunk_index, uint x, uint y, uint z) {
    uint flat = ((z * CHUNK_SIZE) + y) * CHUNK_SIZE + x;
    uint word_offset = chunk_index * CHUNK_UINTS + flat / 2;
    uint word = g_storage_buffers[chunk_buffer_index][word_offset];
    if ((flat & 1) == 0)
        return word & 0xFFFF;
    else
        return word >> 16;
}

[shader("compute")]
[numthreads(8, 8, 4)]
void main(uint3 dtid : SV_DispatchThreadID) {
    uint gx = dtid.x;
    uint gy = dtid.y;
    uint gz = dtid.z;

    uint chunk_index = gz / CHUNK_SIZE;
    uint lx = gx;
    uint ly = gy;
    uint lz = gz % CHUNK_SIZE;

    uint meta_offset = chunk_index * 3;
    int3 chunk_pos;
    chunk_pos.x = g_storage_buffers[metadata_buffer_index][meta_offset + 0];
    chunk_pos.y = g_storage_buffers[metadata_buffer_index][meta_offset + 1];
    chunk_pos.z = g_storage_buffers[metadata_buffer_index][meta_offset + 2];

    uint voxel = read_voxel(chunk_index, lx, ly, lz);

    if (voxel == 0)
        return;

    int3 world_pos = chunk_pos * int3(CHUNK_SIZE) + int3(lx, ly, lz);

    for (uint face = 0; face < 6; ++face) {
        int3 n = int3(lx, ly, lz) + face_offsets[face];

        bool visible;
        if (n.x < 0 || n.x >= CHUNK_SIZE || n.y < 0 || n.y >= CHUNK_SIZE || n.z < 0 || n.z >= CHUNK_SIZE) {
            visible = true;
        } else {
            uint neighbor = read_voxel(chunk_index, n.x, n.y, n.z);
            visible = (neighbor == 0);
        }

        if (visible) {
            uint face_index;
            InterlockedAdd(g_storage_buffers[counter_buffer_index][0], 1, face_index);

            if (face_index >= max_faces)
                continue;

            uint packed = face | (voxel << 8);

            for (uint i = 0; i < 4; ++i) {
                float3 v = (float3(world_pos) + face_vertices[face][i]) * VOXEL_SCALE;
                uint vu = face_index * 16 + i * 4;
                g_storage_buffers[vertex_buffer_index][vu + 0] = asuint(v.x);
                g_storage_buffers[vertex_buffer_index][vu + 1] = asuint(v.y);
                g_storage_buffers[vertex_buffer_index][vu + 2] = asuint(v.z);
                g_storage_buffers[vertex_buffer_index][vu + 3] = packed;
            }

            uint index_base = face_index * 6;
            uint base_vertex = face_index * 4;
            g_storage_buffers[index_buffer_index][index_base + 0] = base_vertex + 0;
            g_storage_buffers[index_buffer_index][index_base + 1] = base_vertex + 1;
            g_storage_buffers[index_buffer_index][index_base + 2] = base_vertex + 2;
            g_storage_buffers[index_buffer_index][index_base + 3] = base_vertex + 0;
            g_storage_buffers[index_buffer_index][index_base + 4] = base_vertex + 2;
            g_storage_buffers[index_buffer_index][index_base + 5] = base_vertex + 3;
        }
    }
}
