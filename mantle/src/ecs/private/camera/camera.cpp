// Copyright (c) 2026 Mantle. All rights reserved.

#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "components.h"
#include "input/components.h"

namespace mantle {

    void bootstrap_camera(const flecs::world &world, f32 aspect) {
        constexpr f32 camera_base_speed = 10.0f;
        constexpr f32 camera_sprint_multiplier = 1.5f;

        (void)world.component<Camera>();

        Camera cam {};
        cam.aspect = aspect;
        cam.fov = 75.0f;
        cam.position = glm::vec3(1.6f, 3.0f, 1.6f);
        world.set<Camera>(cam);

        world.system<>("Camera").kind(flecs::OnUpdate).run([](const flecs::iter &it) {
            auto        w = it.world();
            auto       &camera = w.ensure<Camera>();
            const auto &input_state = w.get<InputState>();

            camera.yaw += input_state.look_dx;
            camera.pitch = glm::clamp(camera.pitch + input_state.look_dy, -89.f, 89.f);

            glm::vec3 front;
            front.x = std::sin(glm::radians(camera.yaw)) * std::cos(glm::radians(camera.pitch));
            front.y = std::sin(glm::radians(camera.pitch));
            front.z = -std::cos(glm::radians(camera.yaw)) * std::cos(glm::radians(camera.pitch));
            camera.front = glm::normalize(front);
            camera.right = glm::normalize(glm::cross(camera.front, glm::vec3(0.f, 1.f, 0.f)));

            f32 speed = camera_base_speed * (input_state.sprint ? camera_sprint_multiplier : 1.0f) *
                        it.delta_time();

            camera.position += camera.front * input_state.move_forward * speed;
            camera.position += camera.right * input_state.move_strafe * speed;
            camera.position += glm::vec3(0.0f, 1.0f, 0.0f) * input_state.move_up * speed;

            glm::mat4 view = glm::lookAt(camera.position, camera.position + camera.front,
                                         glm::vec3(0.f, 1.f, 0.0f));
            glm::mat4 proj =
                glm::perspective(glm::radians(camera.fov), camera.aspect, 0.1f, 1000.f);
            proj[1][1] *= -1.0f;
            camera.view_proj = proj * view;
        });
    }

} // namespace mantle
