import bindings;

struct MeshVertex {
    float3 position : POSITION;
    uint packed : TEXCOORD0;
};

struct VSOut {
    float4 pos : SV_Position;
    float3 normal : TEXCOORD1;
    float3 color : TEXCOORD2;
};

[[vk::push_constant]]
cbuffer PushConstants {
    float4x4 view_proj;
};

static const float3 face_normals[6] = {
    float3(1, 0, 0),
    float3(-1, 0, 0),
    float3(0, 1, 0),
    float3(0, -1, 0),
    float3(0, 0, 1),
    float3(0, 0, -1),
};

static const float3 block_colors[3] = {
    float3(1, 0, 1),
    float3(0.5, 0.35, 0.2),
    float3(0.5, 0.47, 0.43),
};

[shader("vertex")]
VSOut vert_main(MeshVertex vertex) {
    VSOut output;
    output.pos = mul(view_proj, float4(vertex.position, 1.0));

    uint face_index = vertex.packed & 0x7;
    uint block_type = (vertex.packed >> 8) & 0xFF;

    output.normal = face_normals[face_index];
    output.color = block_colors[block_type < 16 ? block_type : 0];

    return output;
}

[shader("fragment")]
float4 frag_main(VSOut input) : SV_Target {
    float3 N = normalize(input.normal);

    float3 light_dir = normalize(float3(0.5, 1.0, 0.3));

    float diff = max(dot(N, light_dir), 0.0);
    float3 lighting = 0.3 + diff * 0.7;

    float3 color = input.color * lighting;

    return float4(color, 1.0);
}
