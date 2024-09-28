
#include "raylib.h"
#include <array>
#include <glm/glm.hpp>

using namespace glm;

constexpr int cell_size = 30;
constexpr int num_cols = 10;
constexpr int num_rows = 20;
constexpr int offset = 10;

constexpr Color red = {255, 0, 0, 255};
constexpr Color orange = {255, 135, 0, 255};
constexpr Color yellow = {255, 255, 0, 255};
constexpr Color cyan = {10, 239, 255, 255};
constexpr Color blue = {88, 10, 255, 255};
constexpr Color green = {161, 255, 10, 255};
constexpr Color purple = {190, 10, 255, 255};

constexpr int cells_in_tetromino = 4;
constexpr int num_orientations = 4;

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

enum class Tetromino : size_t { I = 0, J, L, O, S, T, Z, COUNT };
inline auto getRandomTetromino() -> Tetromino {
    return static_cast<Tetromino>(GetRandomValue(0, static_cast<int>(Tetromino::COUNT) - 1));
}

constexpr std::array<PieceAttributes, static_cast<size_t>(Tetromino::COUNT)> piece_attributes = {
    {i_attr, j_attr, l_attr, o_attr, s_attr, t_attr, z_attr}};
