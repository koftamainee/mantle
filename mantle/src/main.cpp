#include "engine/engine.h"
#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "spdlog/spdlog.h"

int main() {
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::trace);
#else
    spdlog::set_level(spdlog::level::info);
#endif

    mantle::Engine engine;
    engine.init();
    engine.run();
    engine.destroy();

    fflush(stdout);
    printf("see you soon~\n");

    return 0;
}
