import bindings;

[[vk::binding(STORAGE_IMAGE_BINDING, 0)]]
RWTexture2D<float4> g_storage_images[];

[[vk::push_constant]]
cbuffer PushConstants {
    uint storage_index;
    uint chunk_buffer_index;
    float4 camera_position;
    float4 camera_forward;
    float4 camera_right;
    float4 camera_up;
    float  camera_fov;
    float  camera_aspect;
};

[shader("compute")]
[numthreads(8, 8, 1)]
void main(uint3 dispatch_id : SV_DispatchThreadID) {
    uint width, height;
    g_storage_images[storage_index].GetDimensions(width, height);

    if (dispatch_id.x >= width || dispatch_id.y >= height)
        return;

    float2 uv = float2(dispatch_id.xy) / float2(width, height);
    g_storage_images[storage_index][dispatch_id.xy] = float4(uv, 0.0, 1.0);
}
