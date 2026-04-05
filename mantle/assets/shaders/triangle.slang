struct PushConstants {
    float4x4 mvp;
}

[[vk::push_constant]]
PushConstants pc;

struct VSInput {
    float3 position : POSITION;
};

struct VSOutput {
    float4 position : SV_Position;
};

[shader("vertex")]
VSOutput vert_main(VSInput input) {
    VSOutput output;
    output.position = mul(pc.mvp, float4(input.position, 1.0));
    return output;
}

[shader("fragment")]
float4 frag_main() : SV_Target {
    return float4(1.0, 1.0, 1.0, 1.0);
}