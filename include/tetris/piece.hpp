#include "tetris.h"

namespace tetris {

struct Piece {
    ivec2 position = {0, 0};
    Orientation orientation = UP;
    Tetromino type{};

    explicit Piece(Tetromino t) { reset(t); }

    void reset(Tetromino t);
    void draw() const;
};

inline void Piece::reset(Tetromino t) {
    type = t;
    orientation = UP;
    position = piece_attributes[type].spawn_pos;
}

inline void Piece::draw() const {
    for (auto cell : piece_attributes[type].states[orientation]) {
        ivec2 abs_pos = position + cell;
        if (abs_pos.y < 0) {
            continue;
        }
        Rectangle r{.x = static_cast<float>(offset + abs_pos.x * cell_size),
                    .y = static_cast<float>(offset + abs_pos.y * cell_size),
                    .width = cell_size,
                    .height = cell_size};
        DrawRectangleRounded(r, 0.3, 6, piece_attributes[type].color);
    }
}

} // namespace tetris