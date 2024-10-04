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
constexpr float offset = 10;

constexpr float hold_width = 130;
constexpr float next_piece_width = 130;
constexpr int tiny_piece_size = 20;
constexpr int top_margin = 80;
constexpr glm::ivec2 hold_position{hold_width / 2 - 1.5 * tiny_piece_size + offset / 2, top_margin};
constexpr int screen_width = 2 * tetris::offset + tetris::cell_size * tetris::num_cols + hold_width + next_piece_width;
constexpr int screen_height = 2 * tetris::offset + tetris::cell_size * (tetris::num_rows - 2);
constexpr glm::ivec2 next_position{hold_width + 2 * offset + tetris::cell_size * tetris::num_cols +
                                       next_piece_width / 2 - 1.5 * tiny_piece_size - offset / 2,
                                   top_margin};
constexpr int next_position_spacing = 70;

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

constexpr int num_levels = 15;
constexpr double level_max_tick = 0.5;
constexpr double level_min_tick = 0.005;

// Difficulty curve is quadratic
constexpr double stationary = num_levels - 0.3;
constexpr double difficulty_coefficient =
    (level_max_tick - level_min_tick) / (2 * stationary * (num_levels - 1) - (num_levels - 1) * (num_levels - 1));
constexpr auto levelTickRates() -> std::array<double, num_levels> {
    std::array<double, num_levels> arr = {};
    for (int i = 0; i < num_levels; i++) {
        arr[i] = difficulty_coefficient * i * (i - 2 * stationary) + level_max_tick;
    }
    return arr;
}
constexpr std::array<double, num_levels> level_tick_rates = levelTickRates();
constexpr auto levelDownTickRates() -> std::array<double, num_levels> {
    std::array<double, num_levels> arr = {};
    for (size_t i = 0; i < num_levels; i++) {
        arr[i] = level_tick_rates[i] / 20;
    }
    return arr;
}
constexpr std::array<double, num_levels> level_down_tick_rates = levelDownTickRates();

constexpr int num_next_pieces = 6;

enum Orientation : size_t { UP = 0, RIGHT, DOWN, LEFT, NUM_ORIENTATIONS };

inline Orientation operator++(const Orientation& o, int) {
    Orientation out{};
    out = static_cast<Orientation>((o + 1) % NUM_ORIENTATIONS);
    return out;
}

inline Orientation operator--(const Orientation& o, int) {
    Orientation out{};
    out = static_cast<Orientation>((static_cast<int>(o) - 1 + Orientation::NUM_ORIENTATIONS) %
                                   Orientation::NUM_ORIENTATIONS);
    return out;
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
                                 .spawn_pos = {3, 2}};

constexpr PieceAttributes j_attr{.states = {{{{{0, 0}, {0, 1}, {1, 1}, {2, 1}}},
                                             {{{1, 0}, {2, 0}, {1, 1}, {1, 2}}},
                                             {{{0, 1}, {1, 1}, {2, 1}, {2, 2}}},
                                             {{{1, 0}, {1, 1}, {0, 2}, {1, 2}}}}},
                                 .color = blue,
                                 .spawn_pos = {3, 2}};

constexpr PieceAttributes l_attr{.states = {{{{{2, 0}, {0, 1}, {1, 1}, {2, 1}}},
                                             {{{1, 0}, {1, 1}, {1, 2}, {2, 2}}},
                                             {{{0, 1}, {1, 1}, {2, 1}, {0, 2}}},
                                             {{{0, 0}, {1, 0}, {1, 1}, {1, 2}}}}},
                                 .color = orange,
                                 .spawn_pos = {3, 2}};

constexpr PieceAttributes o_attr{.states = {{{{{0, 0}, {0, 1}, {1, 0}, {1, 1}}},
                                             {{{0, 0}, {0, 1}, {1, 0}, {1, 1}}},
                                             {{{0, 0}, {0, 1}, {1, 0}, {1, 1}}},
                                             {{{0, 0}, {0, 1}, {1, 0}, {1, 1}}}}},
                                 .color = yellow,
                                 .spawn_pos = {4, 2}};

constexpr PieceAttributes s_attr{.states = {{{{{1, 0}, {2, 0}, {0, 1}, {1, 1}}},
                                             {{{1, 0}, {1, 1}, {2, 1}, {2, 2}}},
                                             {{{1, 1}, {2, 1}, {0, 2}, {1, 2}}},
                                             {{{0, 0}, {0, 1}, {1, 1}, {1, 2}}}}},
                                 .color = green,
                                 .spawn_pos = {3, 2}};

constexpr PieceAttributes t_attr{.states = {{{{{1, 0}, {0, 1}, {1, 1}, {2, 1}}},
                                             {{{1, 0}, {1, 1}, {2, 1}, {1, 2}}},
                                             {{{0, 1}, {1, 1}, {2, 1}, {1, 2}}},
                                             {{{1, 0}, {0, 1}, {1, 1}, {1, 2}}}}},
                                 .color = purple,
                                 .spawn_pos = {3, 2}};

constexpr PieceAttributes z_attr{.states = {{{{{0, 0}, {1, 0}, {1, 1}, {2, 1}}},
                                             {{{2, 0}, {1, 1}, {2, 1}, {1, 2}}},
                                             {{{0, 1}, {1, 1}, {1, 2}, {2, 2}}},
                                             {{{1, 0}, {0, 1}, {1, 1}, {0, 2}}}}},
                                 .color = red,
                                 .spawn_pos = {3, 2}};

enum Tetromino : size_t { I = 0, J, L, O, S, T, Z, NUM_TETROMINOS };

constexpr std::array<PieceAttributes, NUM_TETROMINOS> piece_attributes = {
    {i_attr, j_attr, l_attr, o_attr, s_attr, t_attr, z_attr}};

using WallTests = std::array<std::array<std::array<ivec2, num_wall_tests>, 2>, Orientation::NUM_ORIENTATIONS>;

// Wall kick data from https://harddrop.com/wiki/SRS
// Format is orientation -> rotation(anti-clockwise=0, clockwise=1) -> (x, y) offset. y is negated in the dataset
// because of differences coordinate system
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