#pragma once
#include "core/macros.h"
#include "core/types.h"
#include <flecs.h>

#include "spdlog/spdlog.h"
#include <glm/glm.hpp>

namespace mantle {
    class Window;

    class Ecs final {
    public:
        Ecs() = default;
        ~Ecs();

        MANTLE_NO_COPY_NO_MOVE(Ecs);

        void init(Window &window, f32 camera_aspect);
        void destroy();
        void update(f32 delta_time);

        glm::mat4 camera_view_proj() const;
        void set_camera_aspect(f32 aspect);

    private:
        bool m_is_initialized = false;
        spdlog::logger *m_logger = nullptr;

        flecs::world m_world{};
    };
}
