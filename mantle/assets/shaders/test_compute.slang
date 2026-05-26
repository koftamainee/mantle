[[vk::binding(2, 0)]]
RWStructuredBuffer<float4> output_buffer;

[numthreads(1, 1, 1)]
[shader("compute")]
void compute_main(uint3 tid : SV_DispatchThreadID) {
    output_buffer[tid.x] = float4(tid.x * 0.1, 0.0, 0.0, 1.0);
}
