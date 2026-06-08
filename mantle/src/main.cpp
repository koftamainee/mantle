#include "core/logger.h"
#include "engine/engine.h"
#include "core/types.h"

int main() {
    mantle::init_logger();

    MANTLE_FATAL(true, "test fatal");

    mantle::Engine engine;
    engine.init();
    engine.run();
    engine.destroy();

    mantle::raw_logger()->info("see you soon~\n");

    return 0;
}
