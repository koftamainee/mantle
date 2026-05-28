import ray;

struct AABB {
    float3 min;
    float3 max;
};

struct RayAABBResult {
    bool hit;
    float t_enter;
    float t_exit;
};

RayAABBResult ray_aabb(Ray ray, AABB aabb, float3 inv_dir) {
    float3 t_min = (aabb.min - ray.origin) * inv_dir;
    float3 t_max = (aabb.max - ray.origin) * inv_dir;

    float3 t1 = min(t_min, t_max);
    float3 t2 = max(t_min, t_max);

    float t_enter = max(max(t1.x, t1.y), t1.z);
    float t_exit = min(min(t2.x, t2.y), t2.z);

    return { t_exit >= max(t_enter, 0.0), t_enter, t_exit };
}