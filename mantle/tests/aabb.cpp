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

#include "mantle/math/aabb.h"

#include <gtest/gtest.h>

using mantle::AABB;

TEST(aabb_tests, intersects_overlapping_boxes) {
    AABB a {{0.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f}};
    AABB b {{1.0f, 1.0f, 1.0f}, {3.0f, 3.0f, 3.0f}};

    EXPECT_TRUE(a.intersects(b));
    EXPECT_TRUE(b.intersects(a));
}

TEST(aabb_tests, intersects_non_overlapping_boxes) {
    AABB a {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}};
    AABB b {{2.0f, 2.0f, 2.0f}, {3.0f, 3.0f, 3.0f}};

    EXPECT_FALSE(a.intersects(b));
    EXPECT_FALSE(b.intersects(a));
}

TEST(aabb_tests, intersects_touching_edges) {
    AABB a {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}};
    AABB b {{1.0f, 1.0f, 1.0f}, {2.0f, 2.0f, 2.0f}};

    EXPECT_TRUE(a.intersects(b));
}

TEST(aabb_tests, contains_point_inside) {
    AABB      box {{0.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f}};
    glm::vec3 p {1.0f, 1.0f, 1.0f};

    EXPECT_TRUE(box.contains(p));
}

TEST(aabb_tests, contains_point_on_boundary) {
    AABB      box {{0.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f}};
    glm::vec3 p {0.0f, 2.0f, 1.0f};

    EXPECT_TRUE(box.contains(p));
}

TEST(aabb_tests, contains_point_outside) {
    AABB      box {{0.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f}};
    glm::vec3 p {3.0f, 1.0f, 1.0f};

    EXPECT_FALSE(box.contains(p));
}

TEST(aabb_tests, size_computation) {
    AABB      box {{1.0f, 2.0f, 3.0f}, {4.0f, 6.0f, 9.0f}};
    glm::vec3 expected {3.0f, 4.0f, 6.0f};

    EXPECT_EQ(box.size(), expected);
}

TEST(aabb_tests, center_computation) {
    AABB      box {{0.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f}};
    glm::vec3 expected {1.0f, 1.0f, 1.0f};

    EXPECT_EQ(box.center(), expected);
}

TEST(aabb_tests, expand_with_point_inside) {
    AABB      box {{0.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f}};
    glm::vec3 p {1.0f, 1.0f, 1.0f};

    box.expand(p);

    EXPECT_EQ(box.min, glm::vec3(0.0f));
    EXPECT_EQ(box.max, glm::vec3(2.0f));
}

TEST(aabb_tests, expand_with_point_outside) {
    AABB      box {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}};
    glm::vec3 p {2.0f, -1.0f, 0.5f};

    box.expand(p);

    EXPECT_EQ(box.min, glm::vec3(0.0f, -1.0f, 0.0f));
    EXPECT_EQ(box.max, glm::vec3(2.0f, 1.0f, 1.0f));
}
