#include "camera/public/camera/camera.h"
#include "glm/matrix.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "mesh/mesh.h"
#include "renderer/renderer.h"
#include "spdlog/spdlog.h"
#include "window/window.h"


int main() {
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::trace);
#else
    spdlog::set_level(spdlog::level::warn);
#endif
    mantle::Window window;


    mantle::Window::Properties prop = {
        .title = "Mantle",
        .size = {
            .width = 2560,
            .height = 1600,
        }
    };
    window.init(prop);

    mantle::Renderer renderer;
    renderer.init(window);

    mantle::Camera camera;

    mantle::Mesh mesh;

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
        0, 1, 2, 2, 3, 0,
        5, 4, 7, 7, 6, 5,
        4, 0, 3, 3, 7, 4,
        1, 5, 6, 6, 2, 1,
        3, 2, 6, 6, 7, 3,
        4, 5, 1, 1, 0, 4
    };


    window.set_resize_callback([&](uint32_t w, uint32_t h) {
        renderer.resize(w, h);
        camera.aspect = static_cast<float>(w) / static_cast<float>(h);
    });

    mantle::GPUResourceManager &resources = renderer.get_resource_manager();

    mantle::MeshHandle mesh_handle = resources.upload_mesh(mesh.vertices, mesh.indices);

    auto model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    while (!window.should_close()) {
        window.on_update();

        renderer.set_camera(camera.view(), camera.projection());

        mantle::Renderer::Result result = renderer.begin_frame();
        if (result == mantle::Renderer::Result::NeedsResize) {
            auto [width, height] = window.get_framebuffer_size();
            renderer.resize(width, height);
            continue;
        }
        renderer.begin_pass();

        renderer.draw_mesh(mesh_handle, model);

        renderer.end_pass();

        result = renderer.end_frame();
        if (result == mantle::Renderer::Result::NeedsResize) {
            auto [width, height] = window.get_framebuffer_size();
            renderer.resize(width, height);
        }
    }

    renderer.destroy();
    window.destroy();
}
