#include "camera/public/camera/camera.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/matrix.hpp"
#include "mesh/mesh.h"
#include "renderer/renderer.h"
#include "spdlog/spdlog.h"
#include "window/window.h"


namespace mantle {
    Mesh create_cube_mesh() {
        Mesh mesh;
        mesh.vertices = {
            {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
            {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
            {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
        };
        mesh.indices = {
            0, 1, 2, 2, 3, 0, 5, 4, 7, 7, 6, 5, 4, 0, 3, 3, 7, 4,
            1, 5, 6, 6, 2, 1, 3, 2, 6, 6, 7, 3, 4, 5, 1, 1, 0, 4,
        };
        return mesh;
    }
    i32 mantle_main() {
        Window window;
        Window::Properties prop = {
            .title = "Mantle",
            .size = {.width = 2560, .height = 1600},
        };
        window.init(prop);

        Renderer renderer;
        renderer.init(window);

        Camera camera;
        camera.aspect = 2560.0f / 1600.0f;

        window.set_resize_callback([&](u32 w, u32 h) {
            renderer.resize(w, h);
            camera.aspect = static_cast<f32>(w) / static_cast<f32>(h);
        });

        GPUResourceManager &resources = renderer.get_resource_manager();
        MeshHandle mesh_handle = resources.upload_mesh(
            create_cube_mesh().vertices, create_cube_mesh().indices);

        f32 rotation = 0.0f;
        f32 last_time = 0.0f;

        while (!window.should_close()) {
            auto current_time = static_cast<f32>(window.get_time());
            f32 delta_time = current_time - last_time;
            last_time = current_time;

            window.on_update();

            rotation += delta_time;


            glm::mat4 model =
                glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f)) *
                glm::rotate(glm::mat4(1.0f), rotation,
                            glm::vec3(0.0f, 1.0f, 0.0f));

            renderer.set_camera(camera.view(), camera.projection());

            Renderer::Result result = renderer.begin_frame();
            if (result == Renderer::Result::FrameNeedsResize) {
                auto [width, height] = window.get_framebuffer_size();
                renderer.resize(width, height);
                continue;
            }

            renderer.begin_pass();
            result = renderer.draw_mesh(mesh_handle, model);
            if (result == Renderer::Result::InvalidMeshHandle) {
                spdlog::critical("Invalid mesh handle. Should be unreachable");
                break;
            }
            renderer.end_pass();

            result = renderer.end_frame();
            if (result == Renderer::Result::FrameNeedsResize) {
                auto [width, height] = window.get_framebuffer_size();
                renderer.resize(width, height);
            }
        }

        renderer.destroy();
        window.destroy();
        return 0;
    }
} // namespace mantle

int main() {
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::trace);
#else
    spdlog::set_level(spdlog::level::info);
#endif

    return mantle::mantle_main();
}
