#include "tetris.h"
#include "raylib.h"
#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <iostream>
#include <optional>
#include <set>
#include <vector>

struct Piece {
    ivec2 position = {0, 0};
    int orientation = 0;
    Tetromino type{};

    explicit Piece(Tetromino t) { reset(t); }

    void reset(Tetromino t) {
        type = t;
        orientation = 0;
        auto type_ = static_cast<size_t>(type);
        position = piece_attributes[type_].spawn_pos;
    }

    void draw() const {
        auto type_ = static_cast<size_t>(type);
        for (auto cell : piece_attributes[type_].states[orientation]) {
            ivec2 abs_pos = position + cell;
            if (abs_pos.y < 0) {
                continue;
            }
            Rectangle r{.x = static_cast<float>(offset + abs_pos.x * cell_size),
                        .y = static_cast<float>(offset + abs_pos.y * cell_size),
                        .width = cell_size,
                        .height = cell_size};
            DrawRectangleRounded(r, 0.3, 6, piece_attributes[type_].color);
        }
    }
};

struct Board {
    std::array<std::array<std::optional<Tetromino>, num_cols>, num_rows> state{};
    Piece active_piece{Tetromino::Z};
    double last_update_time = 0;

    auto tick(double interval) -> bool {
        double current_time = GetTime();
        if (current_time - last_update_time >= interval) {
            last_update_time = current_time;
            return true;
        }
        return false;
    }

    auto collisionCheck(ivec2 pos_bound, int orientation) -> bool {
        auto type_ = static_cast<size_t>(active_piece.type);
        const auto& piece_rel_pos = piece_attributes[type_].states[orientation];
        return std::any_of(piece_rel_pos.begin(), piece_rel_pos.end(), [pos_bound, this](const ivec2& rel_pos) {
            ivec2 absolute_pos = rel_pos + pos_bound;
            bool within_bounds =
                0 <= absolute_pos.x && absolute_pos.x < num_cols && 0 <= absolute_pos.y && absolute_pos.y < num_rows;
            return !within_bounds || state[absolute_pos.y][absolute_pos.x].has_value();
        });
    }

    // TODO: update SRS
    void updateRotation() {
        if (IsKeyPressed(KEY_J)) {
            int new_orientation = (num_orientations + active_piece.orientation - 1) % num_orientations;
            if (!collisionCheck(active_piece.position, new_orientation)) {
                active_piece.orientation = new_orientation;
            }
        } else if (IsKeyPressed(KEY_K)) {
            int new_orientation = (num_orientations + active_piece.orientation + 1) % num_orientations;
            if (!collisionCheck(active_piece.position, new_orientation)) {
                active_piece.orientation = new_orientation;
            }
        }
    }

    void updateTranslation() {
        if (IsKeyPressed(KEY_A)) {
            ivec2 new_position = active_piece.position - ivec2{1, 0};
            if (!collisionCheck(new_position, active_piece.orientation)) {
                active_piece.position = new_position;
            }
        } else if (IsKeyPressed(KEY_D)) {
            ivec2 new_position = active_piece.position + ivec2{1, 0};
            if (!collisionCheck(new_position, active_piece.orientation)) {
                active_piece.position = new_position;
            }
        }
    }

    void clearLines(std::set<int>& lines) {
        for (auto line = lines.begin(); line != lines.end();) {
            std::cout << "line to check: " << *line << std::endl;
            if (std::any_of(state[*line].begin(), state[*line].end(), [](auto&& c) { return !c.has_value(); })) {
                lines.erase(line++);
            } else {
                line++;
            }
        }

        // TODO: this can be optimized to comptue all line updates in one go rather than looping through each line clear
        for (auto line : lines) {
            std::cout << "clear line " << line << std::endl;
        }
    }

    void updateFall() {
        ivec2 new_position = active_piece.position + ivec2{0, 1};
        if (collisionCheck(new_position, active_piece.orientation)) {
            auto type_ = static_cast<size_t>(active_piece.type);
            const auto& piece_rel_pos = piece_attributes[type_].states[active_piece.orientation];
            std::set<int> clear_lines{};
            for (const auto& pos_rel : piece_rel_pos) {
                ivec2 absolute_pos = active_piece.position + pos_rel;
                state[absolute_pos.y][absolute_pos.x] = active_piece.type;
                clear_lines.insert(absolute_pos.y);
            }
            clearLines(clear_lines);
            active_piece.reset(getRandomTetromino());
        } else {
            active_piece.position = new_position;
        }
    }

    void drawCell(size_t row, size_t col) const {
        if (state[row][col].has_value()) {
            auto type = static_cast<size_t>(state[row][col].value());
            Rectangle r{.x = static_cast<float>(offset + col * cell_size),
                        .y = static_cast<float>(offset + row * cell_size),
                        .width = cell_size,
                        .height = cell_size};
            DrawRectangleRounded(r, 0.3, 6, piece_attributes[type].color);
        }
    }

    void update() {
        updateRotation();
        updateTranslation();
        if (tick(0.2)) {
            updateFall();
        }
    }

    void draw() const {
        for (size_t row = 0; row < num_rows; row++) {
            for (size_t col = 0; col < num_cols; col++) {
                drawCell(row, col);
            }
        }

        active_piece.draw();
    }
};

auto main() -> int {
    InitWindow(2 * offset + cell_size * num_cols, 2 * offset + cell_size * num_rows, "Tetris");
    SetTargetFPS(60);

    Board board{};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        board.update();

        board.draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}