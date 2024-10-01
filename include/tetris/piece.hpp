#include "tetris.hpp"
#include <glm/ext/vector_int2.hpp>
#include <raylib.h>

namespace tetris {

struct Piece {
    ivec2 position{};
    Orientation orientation{};
    Tetromino type{};

    explicit Piece(Tetromino t) { reset(t); }

    void reset(Tetromino t);
    void draw() const;
    void drawGhost() const;
};

inline void Piece::reset(Tetromino t) {
    type = t;
    orientation = Orientation::UP;
    position = piece_attributes[type].spawn_pos;
}

inline void Piece::draw() const {
    for (auto cell : piece_attributes[type].states[orientation]) {
        ivec2 abs_pos = position + cell;
        if (abs_pos.y < 2) {
            continue;
        }
        Rectangle r{.x = static_cast<float>(offset + abs_pos.x * cell_size),
                    .y = static_cast<float>(offset + (abs_pos.y - 2) * cell_size),
                    .width = cell_size,
                    .height = cell_size};
        DrawRectangleRounded(r, 0.4, 6, piece_attributes[type].color);
    }
}

inline void Piece::drawGhost() const {
    for (auto cell : piece_attributes[type].states[orientation]) {
        ivec2 abs_pos = position + cell;
        if (abs_pos.y < 2) {
            continue;
        }
        Rectangle r{.x = static_cast<float>(offset + abs_pos.x * cell_size - 1),
                    .y = static_cast<float>(offset + (abs_pos.y - 2) * cell_size),
                    .width = cell_size + 1,
                    .height = cell_size + 1};
        DrawRectangleLinesEx(r, 3, piece_attributes[type].color);
    }
}

} // namespace tetris