#pragma once

#include "raylib.h"
#include <array>
#include <cstddef>
#include <glm/ext/vector_int2.hpp>

using namespace glm;

namespace tetris {

constexpr int cell_size = 30;
constexpr int num_cols = 10;
constexpr int num_rows = 20;
constexpr int offset = 10;

constexpr int screen_width = 2 * tetris::offset + tetris::cell_size * tetris::num_cols;
constexpr int screen_height = 2 * tetris::offset + tetris::cell_size * tetris::num_rows;

constexpr Color red = {255, 0, 0, 255};
constexpr Color orange = {255, 135, 0, 255};
constexpr Color yellow = {255, 255, 0, 255};
constexpr Color cyan = {10, 239, 255, 255};
constexpr Color blue = {88, 10, 255, 255};
constexpr Color green = {161, 255, 10, 255};
constexpr Color purple = {190, 10, 255, 255};

constexpr int cells_in_tetromino = 4;
constexpr int num_orientations = 4;
constexpr int num_wall_tests = 5;

constexpr double lock_delay_period = 0.5;

using States = std::array<std::array<ivec2, cells_in_tetromino>, num_orientations>;

struct PieceAttributes {
    States states;
    Color color;
    glm::ivec2 spawn_pos;
};

constexpr PieceAttributes i_attr{.states = {{{{{0, 1}, {1, 1}, {2, 1}, {3, 1}}},
                                             {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}},
                                             {{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},
                                             {{{1, 0}, {1, 1}, {1, 2}, {1, 3}}}}},
                                 .color = cyan,
                                 .spawn_pos = {3, -1}};

constexpr PieceAttributes j_attr{.states = {{{{{0, 0}, {0, 1}, {1, 1}, {2, 1}}},
                                             {{{1, 0}, {2, 0}, {1, 1}, {1, 2}}},
                                             {{{0, 1}, {1, 1}, {2, 1}, {2, 2}}},
                                             {{{1, 0}, {1, 1}, {0, 2}, {1, 2}}}}},
                                 .color = blue,
                                 .spawn_pos = {3, -1}};

constexpr PieceAttributes l_attr{.states = {{{{{2, 0}, {0, 1}, {1, 1}, {2, 1}}},
                                             {{{1, 0}, {1, 1}, {1, 2}, {2, 2}}},
                                             {{{0, 1}, {1, 1}, {2, 1}, {0, 2}}},
                                             {{{0, 0}, {1, 0}, {1, 1}, {1, 2}}}}},
                                 .color = orange,
                                 .spawn_pos = {3, -1}};

constexpr PieceAttributes o_attr{.states = {{{{{0, 0}, {0, 1}, {1, 0}, {1, 1}}},
                                             {{{0, 0}, {0, 1}, {1, 0}, {1, 1}}},
                                             {{{0, 0}, {0, 1}, {1, 0}, {1, 1}}},
                                             {{{0, 0}, {0, 1}, {1, 0}, {1, 1}}}}},
                                 .color = yellow,
                                 .spawn_pos = {4, -1}};

constexpr PieceAttributes s_attr{.states = {{{{{1, 0}, {2, 0}, {0, 1}, {1, 1}}},
                                             {{{1, 0}, {1, 1}, {2, 1}, {2, 2}}},
                                             {{{1, 1}, {2, 1}, {0, 2}, {1, 2}}},
                                             {{{0, 0}, {0, 1}, {1, 1}, {1, 2}}}}},
                                 .color = green,
                                 .spawn_pos = {3, -1}};

constexpr PieceAttributes t_attr{.states = {{{{{1, 0}, {0, 1}, {1, 1}, {2, 1}}},
                                             {{{1, 0}, {1, 1}, {2, 1}, {1, 2}}},
                                             {{{0, 1}, {1, 1}, {2, 1}, {1, 2}}},
                                             {{{1, 0}, {0, 1}, {1, 1}, {1, 2}}}}},
                                 .color = purple,
                                 .spawn_pos = {3, -1}};

constexpr PieceAttributes z_attr{.states = {{{{{0, 0}, {1, 0}, {1, 1}, {2, 1}}},
                                             {{{2, 0}, {1, 1}, {2, 1}, {1, 2}}},
                                             {{{0, 1}, {1, 1}, {1, 2}, {2, 2}}},
                                             {{{1, 0}, {0, 1}, {1, 1}, {0, 2}}}}},
                                 .color = red,
                                 .spawn_pos = {3, -1}};

enum Tetromino : size_t { I, J, L, O, S, T, Z, COUNT };
enum Orientation : size_t { UP, RIGHT, DOWN, LEFT };
enum Rotation : size_t { AntiClockwise, Clockwise };

inline auto rotateClockwise(Orientation o) -> Orientation {
    return static_cast<Orientation>((static_cast<int>(o) + 1) % num_orientations);
}

inline auto rotateAntiClockwise(Orientation o) -> Orientation {
    return static_cast<Orientation>((static_cast<int>(o) - 1 + num_orientations) % num_orientations);
}

constexpr std::array<PieceAttributes, static_cast<size_t>(Tetromino::COUNT)> piece_attributes = {
    {i_attr, j_attr, l_attr, o_attr, s_attr, t_attr, z_attr}};

using WallTests = std::array<std::array<std::array<ivec2, num_wall_tests>, 2>, num_orientations>;

// Wall kick data from https://harddrop.com/wiki/SRS
constexpr WallTests wall_kick_tests_not_i{{
    {{{{{0, 0}, {+1, 0}, {+1, +1}, {0, -2}, {+1, -2}}}, {{{0, 0}, {-1, 0}, {-1, +1}, {0, -2}, {-1, -2}}}}},
    {{{{{0, 0}, {+1, 0}, {+1, -1}, {0, +2}, {+1, +2}}}, {{{0, 0}, {+1, 0}, {+1, -1}, {0, +2}, {+1, +2}}}}},
    {{{{{0, 0}, {-1, 0}, {-1, +1}, {0, -2}, {-1, -2}}}, {{{0, 0}, {+1, 0}, {+1, +1}, {0, -2}, {+1, -2}}}}},
    {{{{{0, 0}, {-1, 0}, {-1, -1}, {0, +2}, {-1, +2}}}, {{{0, 0}, {-1, 0}, {-1, -1}, {0, +2}, {-1, +2}}}}},
}};

constexpr WallTests wall_kick_tests_i{{
    {{{{{0, 0}, {-1, 0}, {+2, 0}, {-1, +2}, {+2, -1}}}, {{{0, 0}, {-2, 0}, {+1, 0}, {-2, -1}, {+1, +2}}}}},
    {{{{{0, 0}, {+2, 0}, {-1, 0}, {+2, +1}, {-1, -2}}}, {{{0, 0}, {-1, 0}, {+2, 0}, {-1, +2}, {+2, -1}}}}},
    {{{{{0, 0}, {+1, 0}, {-2, 0}, {+1, -2}, {-2, +1}}}, {{{0, 0}, {+2, 0}, {-1, 0}, {+2, +1}, {-1, -2}}}}},
    {{{{{0, 0}, {-2, 0}, {+1, 0}, {-2, -1}, {+1, +2}}}, {{{0, 0}, {+1, 0}, {-2, 0}, {+1, -2}, {-2, +1}}}}},
}};

enum class SlideState { Inactive, StartDelay, Slide };
constexpr double slide_rate = 0.05;
constexpr double slide_delay_period = 0.1;

} // namespace tetris