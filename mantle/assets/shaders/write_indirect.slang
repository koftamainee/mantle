import bindings;

[[vk::binding(STORAGE_BUFFER_BINDING, 0)]]
RWStructuredBuffer<uint> g_storage_buffers[];

struct DrawIndexedIndirectCommand {
    uint index_count;
    uint instance_count;
    uint first_index;
    int  vertex_offset;
    uint first_instance;
};

[[vk::push_constant]]
cbuffer PushConstants {
    uint counter_buffer_index;
    uint indirect_buffer_index;
    uint max_faces;
};

[shader("compute")]
[numthreads(1, 1, 1)]
void main() {
    uint face_count = min(g_storage_buffers[counter_buffer_index][0], max_faces);

    DrawIndexedIndirectCommand cmd;
    cmd.index_count = face_count * 6;
    cmd.instance_count = 1;
    cmd.first_index = 0;
    cmd.vertex_offset = 0;
    cmd.first_instance = 0;

    g_storage_buffers[indirect_buffer_index][0] = cmd.index_count;
    g_storage_buffers[indirect_buffer_index][1] = cmd.instance_count;
    g_storage_buffers[indirect_buffer_index][2] = cmd.first_index;
    g_storage_buffers[indirect_buffer_index][3] = asuint(cmd.vertex_offset);
    g_storage_buffers[indirect_buffer_index][4] = cmd.first_instance;

    g_storage_buffers[counter_buffer_index][0] = face_count;
}
