// Copyright (c) 2026 Mantle. All rights reserved.

#include "core/logger.h"
#include "engine/engine.h"

int main() {
    mantle::register_loggers();

    mantle::Engine engine;
    engine.init();
    engine.run();
    engine.destroy();

    mantle::raw_logger()->info("see you soon~\n");

    return 0;
}
