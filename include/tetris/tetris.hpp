#pragma once

#include "raylib.h"
#include <array>
#include <cstddef>
#include <glm/ext/vector_int2.hpp>

using namespace glm;

namespace tetris {

constexpr int cell_size = 30;
constexpr int num_cols = 10;
constexpr int num_rows = 22;
constexpr int offset = 10;

constexpr int screen_width = 2 * tetris::offset + tetris::cell_size * tetris::num_cols;
constexpr int screen_height = 2 * tetris::offset + tetris::cell_size * (tetris::num_rows - 2);

constexpr Color red = {255, 0, 0, 255};
constexpr Color orange = {255, 135, 0, 255};
constexpr Color yellow = {255, 255, 0, 255};
constexpr Color cyan = {10, 239, 255, 255};
constexpr Color blue = {88, 10, 255, 255};
constexpr Color green = {161, 255, 10, 255};
constexpr Color purple = {190, 10, 255, 255};

constexpr int cells_in_tetromino = 4;
constexpr int num_wall_tests = 5;

constexpr double lock_delay_period = 0.5;

constexpr double level_tick_rate = 0.2;

enum Orientation : size_t { UP = 0, RIGHT, DOWN, LEFT, NUM_ORIENTATIONS };

inline Orientation& operator++(Orientation& o) {
    o = static_cast<Orientation>((o + 1) % NUM_ORIENTATIONS);
    return o;
}

inline Orientation& operator--(Orientation& o) {
    o = static_cast<Orientation>((static_cast<int>(o) - 1 + Orientation::NUM_ORIENTATIONS) %
                                 Orientation::NUM_ORIENTATIONS);
    return o;
}

using PieceStates = std::array<std::array<ivec2, cells_in_tetromino>, Orientation::NUM_ORIENTATIONS>;

struct PieceAttributes {
    PieceStates states;
    Color color;
    glm::ivec2 spawn_pos;
};

constexpr PieceAttributes i_attr{.states = {{{{{0, 1}, {1, 1}, {2, 1}, {3, 1}}},
                                             {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}},
                                             {{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},
                                             {{{1, 0}, {1, 1}, {1, 2}, {1, 3}}}}},
                                 .color = cyan,
                                 .spawn_pos = {3, 0}};

constexpr PieceAttributes j_attr{.states = {{{{{0, 0}, {0, 1}, {1, 1}, {2, 1}}},
                                             {{{1, 0}, {2, 0}, {1, 1}, {1, 2}}},
                                             {{{0, 1}, {1, 1}, {2, 1}, {2, 2}}},
                                             {{{1, 0}, {1, 1}, {0, 2}, {1, 2}}}}},
                                 .color = blue,
                                 .spawn_pos = {3, 0}};

constexpr PieceAttributes l_attr{.states = {{{{{2, 0}, {0, 1}, {1, 1}, {2, 1}}},
                                             {{{1, 0}, {1, 1}, {1, 2}, {2, 2}}},
                                             {{{0, 1}, {1, 1}, {2, 1}, {0, 2}}},
                                             {{{0, 0}, {1, 0}, {1, 1}, {1, 2}}}}},
                                 .color = orange,
                                 .spawn_pos = {3, 0}};

constexpr PieceAttributes o_attr{.states = {{{{{0, 0}, {0, 1}, {1, 0}, {1, 1}}},
                                             {{{0, 0}, {0, 1}, {1, 0}, {1, 1}}},
                                             {{{0, 0}, {0, 1}, {1, 0}, {1, 1}}},
                                             {{{0, 0}, {0, 1}, {1, 0}, {1, 1}}}}},
                                 .color = yellow,
                                 .spawn_pos = {4, 0}};

constexpr PieceAttributes s_attr{.states = {{{{{1, 0}, {2, 0}, {0, 1}, {1, 1}}},
                                             {{{1, 0}, {1, 1}, {2, 1}, {2, 2}}},
                                             {{{1, 1}, {2, 1}, {0, 2}, {1, 2}}},
                                             {{{0, 0}, {0, 1}, {1, 1}, {1, 2}}}}},
                                 .color = green,
                                 .spawn_pos = {3, 0}};

constexpr PieceAttributes t_attr{.states = {{{{{1, 0}, {0, 1}, {1, 1}, {2, 1}}},
                                             {{{1, 0}, {1, 1}, {2, 1}, {1, 2}}},
                                             {{{0, 1}, {1, 1}, {2, 1}, {1, 2}}},
                                             {{{1, 0}, {0, 1}, {1, 1}, {1, 2}}}}},
                                 .color = purple,
                                 .spawn_pos = {3, 0}};

constexpr PieceAttributes z_attr{.states = {{{{{0, 0}, {1, 0}, {1, 1}, {2, 1}}},
                                             {{{2, 0}, {1, 1}, {2, 1}, {1, 2}}},
                                             {{{0, 1}, {1, 1}, {1, 2}, {2, 2}}},
                                             {{{1, 0}, {0, 1}, {1, 1}, {0, 2}}}}},
                                 .color = red,
                                 .spawn_pos = {3, 0}};

enum Tetromino : size_t { I = 0, J, L, O, S, T, Z, NUM_TETROMINOS };

constexpr std::array<PieceAttributes, NUM_TETROMINOS> piece_attributes = {
    {i_attr, j_attr, l_attr, o_attr, s_attr, t_attr, z_attr}};

using WallTests = std::array<std::array<std::array<ivec2, num_wall_tests>, 2>, Orientation::NUM_ORIENTATIONS>;

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
constexpr double slide_rate = 0.04;
constexpr double slide_delay_period = 0.08;

} // namespace tetris