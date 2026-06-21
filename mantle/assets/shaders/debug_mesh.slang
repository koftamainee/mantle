struct VSIn {
    float3 position : POSITION;
};

struct VSOut {
    float4 pos : SV_Position;
};

[[vk::push_constant]]
cbuffer PushConstants {
    float4x4 view_proj;
    float4x4 model;
};

[shader("vertex")]
VSOut vert_main(VSIn vertex) {
    VSOut output;
    float4 world_pos = mul(model, float4(vertex.position, 1.0));
    output.pos = mul(view_proj, world_pos);
    return output;
}

[shader("fragment")]
float4 frag_main(VSOut input) : SV_Target {
    return float4(0.8, 0.8, 0.8, 1.0);
}
