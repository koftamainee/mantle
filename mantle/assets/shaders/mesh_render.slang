import bindings;

struct MeshVertex {
    float3 position : POSITION;
    uint packed : TEXCOORD0;
};

struct VSOut {
    float4 pos : SV_Position;
    float3 color : TEXCOORD1;
    float ao : TEXCOORD2;
    float light : TEXCOORD3;
};

[[vk::push_constant]]
cbuffer PushConstants {
    float4x4 view_proj;
};

static const float3 face_normals[6] = {
    float3(-1, 0, 0),    // 0: axis=0 (X), positive=0  → -X
    float3(1, 0, 0),     // 1: axis=0 (X), positive=1  → +X
    float3(0, -1, 0),    // 2: axis=1 (Y), positive=0  → -Y
    float3(0, 1, 0),     // 3: axis=1 (Y), positive=1  → +Y
    float3(0, 0, -1),    // 4: axis=2 (Z), positive=0  → -Z
    float3(0, 0, 1),     // 5: axis=2 (Z), positive=1  → +Z
};

static const float3 block_colors[8] = {
    float3(1, 0, 1),             // 0: unused (magenta)
    float3(0.50, 0.52, 0.55),    // 1: Stone-grey
    float3(0.78, 0.75, 0.65),    // 2: Limestone-cream
    float3(1.00, 0.35, 0.00),    // 3: Moss orange
    float3(1.00, 0.45, 0.05),    // 4: Moss light orange
    float3(0.65, 0.20, 0.00),    // 5: Moss dark orange
};

[shader("vertex")]
VSOut vert_main(MeshVertex vertex) {
    VSOut output;
    output.pos = mul(view_proj, float4(vertex.position, 1.0));

    uint face_index = vertex.packed & 0x7;
    uint ao_raw = (vertex.packed >> 3) & 0x3;
    uint light_raw = (vertex.packed >> 5) & 0xF;
    uint block_type = (vertex.packed >> 9) & 0xFFFF;

    output.color = block_colors[block_type < 16 ? block_type : 0];
    output.ao = ao_raw / 3.0;
    output.light = light_raw / 15.0;

    return output;
}

[shader("fragment")]
float4 frag_main(VSOut input) : SV_Target {
    float ao_factor = 1.0 - input.ao * 0.4;
    float light_factor = max(input.light, 0.08);
    float3 color = input.color * ao_factor * light_factor;
    return float4(color, 1.0);
}
