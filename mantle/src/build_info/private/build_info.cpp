// Copyright 2026 Mantle
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "mantle/build_info/build_info.h"

#ifndef MANTLE_VERSION
#define MANTLE_VERSION "unknown"
#endif

#ifndef MANTLE_GIT_HASH
#define MANTLE_GIT_HASH "unknown"
#endif

#ifndef MANTLE_BUILD_TYPE
#define MANTLE_BUILD_TYPE "unknown"
#endif

#ifndef MANTLE_BUILD_DATE
#define MANTLE_BUILD_DATE "unknown"
#endif

#ifndef MANTLE_COMPILER
#define MANTLE_COMPILER "unknown"
#endif

namespace mantle {

    const char *version_string() { return "Mantle v" MANTLE_VERSION " (" MANTLE_GIT_HASH ")"; }

    const char *build_string() {
        return "Mantle v" MANTLE_VERSION " (" MANTLE_GIT_HASH ") | " MANTLE_BUILD_TYPE
               " | " MANTLE_COMPILER " | "
               "C++23";
    }
} // namespace mantle
