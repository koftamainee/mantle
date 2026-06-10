#include "physics/physics_system.h"

#include "core/assert.h"

namespace mantle {

    void PhysicsSystem::init(MemoryBlock block) {
        MANTLE_CHECK(!m_is_initialized);

        m_logger = spdlog::get("physics").get();

        m_is_initialized = true;
        m_logger->info("Physics system is initialized");
    }

    void PhysicsSystem::update(f32 dt) {
        MANTLE_CHECK(m_is_initialized);
    }

    void PhysicsSystem::destroy() {
        if (m_is_initialized) {
            m_is_initialized = false;
            m_logger->info("Physics system is destroyed");
        }
    }

} // namespace mantle
