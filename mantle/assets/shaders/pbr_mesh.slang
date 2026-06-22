import bindings;

[[vk::binding(SAMPLED_IMAGE_BINDING, 0)]]
Texture2D<float4> uTextures[];

[[vk::binding(STORAGE_BUFFER_BINDING, 0)]]
ByteAddressBuffer uStorageBuffers[];

[[vk::binding(SAMPLER_BINDING, 0)]]
SamplerState uSamplers[];

struct VSInput {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD0;
};

struct VSOutput {
    float4 position : SV_Position;
    float3 world_pos : TEXCOORD0;
    float3 world_normal : TEXCOORD1;
    float2 uv : TEXCOORD2;
    float3 world_tangent : TEXCOORD3;
    float tangent_sign : TEXCOORD4;
};

struct PushConstants {
    float4x4 view_proj;
    uint entity_index;
    uint material_index;
    uint material_buf;
    uint frame_data_buf;
    uint light_buf;
};
[[vk::push_constant]] PushConstants pc;

struct MaterialData {
    float4 base_color;
    float4 metallic_roughness;
    float4 emissive_alpha_cutoff;
    uint base_color_tex;
    uint normal_tex;
    uint metallic_roughness_tex;
    uint emissive_tex;
    uint occlusion_tex;
    uint sampler_idx;
    uint flags;
};

MaterialData load_material(uint idx) {
    uint offset = idx * 96;
    ByteAddressBuffer buf = uStorageBuffers[pc.material_buf];
    MaterialData m;
    m.base_color = buf.Load<float4>(offset + 0);
    m.metallic_roughness = buf.Load<float4>(offset + 16);
    m.emissive_alpha_cutoff = buf.Load<float4>(offset + 32);
    m.base_color_tex = buf.Load(offset + 48);
    m.normal_tex = buf.Load(offset + 52);
    m.metallic_roughness_tex = buf.Load(offset + 56);
    m.emissive_tex = buf.Load(offset + 60);
    m.occlusion_tex = buf.Load(offset + 64);
    m.sampler_idx = buf.Load(offset + 68);
    m.flags = buf.Load(offset + 72);
    return m;
}

struct DirectionalLight {
    float4 direction;
    float4 color;
};

struct GPULightData {
    float4 camera_pos;
    float4 ambient;
    DirectionalLight lights[8];
    uint light_count;
};

GPULightData load_light_data() {
    ByteAddressBuffer buf = uStorageBuffers[pc.light_buf];
    GPULightData l;
    l.camera_pos = buf.Load<float4>(0);
    l.ambient = buf.Load<float4>(16);
    for (uint i = 0; i < 8; i++) {
        uint off = 32 + i * 32;
        l.lights[i].direction = buf.Load<float4>(off);
        l.lights[i].color = buf.Load<float4>(off + 16);
    }
    l.light_count = buf.Load(32 + 8 * 32);
    return l;
}

static const float PI = 3.14159265;

float3 fresnel_schlick(float3 f0, float cos_theta) {
    return f0 + (1.0 - f0) * pow(max(1.0 - cos_theta, 0.0), 5.0);
}

float distribution_ggx(float3 N, float3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
    return a2 / (PI * max(denom * denom, 0.0001));
}

float geometry_smith(float3 N, float3 V, float3 L, float roughness) {
    float a = roughness + 1.0;
    float k = (a * a) / 8.0;
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float g1 = NdotV / (NdotV * (1.0 - k) + k);
    float g2 = NdotL / (NdotL * (1.0 - k) + k);
    return g1 * g2;
}

[shader("vertex")]
VSOutput vert_main(VSInput vertex) {
    VSOutput output;

    float4x4 model = uStorageBuffers[pc.frame_data_buf].Load<float4x4>(pc.entity_index * 64);
    float4 world_pos = mul(model, float4(vertex.position, 1.0));
    output.position = mul(pc.view_proj, world_pos);
    output.world_pos = world_pos.xyz;

    float3x3 normal_mat = float3x3(model[0].xyz, model[1].xyz, model[2].xyz);
    output.world_normal = normalize(mul(normal_mat, vertex.normal));

    output.uv = vertex.texcoord;

    output.world_tangent = normalize(mul(normal_mat, vertex.tangent.xyz));
    output.tangent_sign = vertex.tangent.w;

    return output;
}

[shader("fragment")]
float4 frag_main(VSOutput input) : SV_Target {
    MaterialData mat = load_material(pc.material_index);
    GPULightData light = load_light_data();

    float4 base_rgba = uTextures[mat.base_color_tex].SampleLevel(uSamplers[mat.sampler_idx], input.uv, 0);
    float3 base_color = base_rgba.rgb * mat.base_color.rgb;
    float alpha = base_rgba.a * mat.base_color.a;

    float metallic = mat.metallic_roughness.x;
    float roughness = mat.metallic_roughness.y;
    float emissive_strength = mat.metallic_roughness.z;
    float4 mr_tex = uTextures[mat.metallic_roughness_tex].SampleLevel(uSamplers[mat.sampler_idx], input.uv, 0);
    metallic *= mr_tex.x;
    roughness *= mr_tex.y;
    roughness = max(roughness, 0.08);

    float ao = 1.0;
    if (mat.occlusion_tex != ~0u) {
        ao = uTextures[mat.occlusion_tex].SampleLevel(uSamplers[mat.sampler_idx], input.uv, 0).r;
    }

    float3 N = normalize(input.world_normal);
    if (mat.normal_tex != ~0u) {
        float3 n_sample = uTextures[mat.normal_tex].SampleLevel(uSamplers[mat.sampler_idx], input.uv, 0).xyz;
        n_sample = normalize(n_sample * 2.0 - 1.0);
        float3 T = normalize(input.world_tangent - dot(input.world_tangent, N) * N);
        float3 B = cross(N, T) * input.tangent_sign;
        float3x3 tbn = float3x3(T, B, N);
        N = normalize(mul(n_sample, tbn));
    }

    float3 V = normalize(light.camera_pos.xyz - input.world_pos);

    float3 f0 = lerp(float3(0.04, 0.04, 0.04), base_color, metallic);

    float3 Lo = 0.0;

    for (uint i = 0; i < light.light_count; i++) {
        float3 L = normalize(-light.lights[i].direction.xyz);
        float3 H = normalize(V + L);

        float NdotL = max(dot(N, L), 0.0);

        float3 radiance = light.lights[i].color.rgb * light.lights[i].direction.w;

        float NDF = distribution_ggx(N, H, roughness);
        float G   = geometry_smith(N, V, L, roughness);
        float3 F  = fresnel_schlick(f0, max(dot(H, V), 0.0));

        float3 kS = F;
        float3 kD = (1.0 - kS) * (1.0 - metallic);

        float3 specular = (NDF * G * F) / max(4.0 * max(dot(N, V), 0.0) * NdotL, 0.0001);
        float3 diffuse  = kD * base_color / PI;

        Lo += (diffuse + specular) * radiance * NdotL;
    }

    float3 ambient = light.ambient.rgb * light.ambient.w * base_color * ao;

    float3 emissive = mat.emissive_alpha_cutoff.rgb * emissive_strength;

    float3 final_color = ambient + Lo + emissive;

    return float4(final_color, alpha);
}
