#include "tetris.h"
#include "raylib.h"
#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <iostream>
#include <optional>
#include <random>
#include <set>

struct Piece {
    ivec2 position = {0, 0};
    int orientation = 0;
    Tetromino type{};

    explicit Piece(Tetromino t) { reset(t); }

    void reset(Tetromino t) {
        type = t;
        orientation = 0;
        position = piece_attributes[type].spawn_pos;
    }

    void draw() const {
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
};

struct Board {
    std::array<std::array<std::optional<Tetromino>, num_cols>, num_rows> state{};
    Piece active_piece{Tetromino{}};
    double last_update_time = GetTime();
    bool lock_delay = false;
    double lock_delay_start_time = 0;
    bool running = true;
    double level_tick_rate = 0.2;
    double tick_rate = level_tick_rate;

    std::random_device rd;
    std::mt19937 g{rd()};
    std::array<Tetromino, Tetromino::COUNT> random_bag_0 = {I, J, L, O, S, T, Z};
    std::array<Tetromino, Tetromino::COUNT> random_bag_1 = {I, J, L, O, S, T, Z};
    std::array<Tetromino, Tetromino::COUNT>::iterator next_piece;
    bool bag_0 = true;

    void reset() {
        state = {};
        std::shuffle(random_bag_0.begin(), random_bag_0.end(), g);
        std::shuffle(random_bag_1.begin(), random_bag_1.end(), g);
        next_piece = random_bag_0.begin();
        active_piece.type = *next_piece;
    }

    auto getNextTetromino() -> Tetromino {
        next_piece++;
        if (bag_0 && next_piece == random_bag_0.end()) {
            std::shuffle(random_bag_0.begin(), random_bag_0.end(), g);
            next_piece = random_bag_1.begin();
            bag_0 = false;
        } else if (!bag_0 && next_piece == random_bag_1.end()) {
            std::shuffle(random_bag_0.begin(), random_bag_0.end(), g);
            next_piece = random_bag_0.begin();
            bag_0 = true;
        }
        return *next_piece;
    }

    auto tick(double interval) -> bool {
        double current_time = GetTime();
        if (current_time - last_update_time >= interval) {
            last_update_time = current_time;
            return true;
        }
        return false;
    }

    auto collisionCheck(ivec2 pos_bound, int orientation) -> bool {
        const auto& piece_rel_pos = piece_attributes[active_piece.type].states[orientation];
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
            lock_delay = false;
            int new_orientation = (num_orientations + active_piece.orientation - 1) % num_orientations;
            const auto* wall_tests = &wall_kick_tests_not_i;
            if (active_piece.type == I) {
                wall_tests = &wall_kick_tests_i;
            }
            for (const ivec2& wall_test : (*wall_tests)[active_piece.orientation][0]) {
                ivec2 new_position{active_piece.position.x + wall_test.x, active_piece.position.y - wall_test.y};
                if (!collisionCheck(new_position, new_orientation)) {
                    active_piece.orientation = new_orientation;
                    active_piece.position = new_position;
                    break;
                }
            }
        } else if (IsKeyPressed(KEY_K)) {
            lock_delay = false;
            int new_orientation = (num_orientations + active_piece.orientation + 1) % num_orientations;
            const auto* wall_tests = &wall_kick_tests_not_i;
            if (active_piece.type == I) {
                wall_tests = &wall_kick_tests_i;
            }
            for (const ivec2& wall_test : (*wall_tests)[active_piece.orientation][1]) {
                ivec2 new_position{active_piece.position.x + wall_test.x, active_piece.position.y - wall_test.y};
                if (!collisionCheck(new_position, new_orientation)) {
                    active_piece.orientation = new_orientation;
                    active_piece.position = new_position;
                    break;
                }
            }
        }
    }

    void updateTranslation() {
        if (IsKeyPressed(KEY_A)) {
            ivec2 new_position = active_piece.position - ivec2{1, 0};
            if (!collisionCheck(new_position, active_piece.orientation)) {
                lock_delay = false;
                active_piece.position = new_position;
            }
        } else if (IsKeyPressed(KEY_D)) {
            ivec2 new_position = active_piece.position + ivec2{1, 0};
            if (!collisionCheck(new_position, active_piece.orientation)) {
                lock_delay = false;
                active_piece.position = new_position;
            }
        }
        if (IsKeyDown(KEY_S)) {
            tick_rate = 0.05;
        } else {
            tick_rate = level_tick_rate;
        }
    }

    void clearLine(int line) {
        assert(0 <= line && line < num_rows);
        for (int line_write = line; line_write > 0; line_write--) {
            state[line_write] = state[line_write - 1];
        }
    }

    void clearLines(std::set<int>& lines) {
        for (auto line = lines.begin(); line != lines.end();) {
            if (std::any_of(state[*line].begin(), state[*line].end(), [](auto&& c) { return !c.has_value(); })) {
                lines.erase(line++);
            } else {
                line++;
            }
        }

        for (auto line : lines) {
            clearLine(line);
        }
    }

    void triggerLock() {
        lock_delay = false;
        const auto& piece_rel_pos = piece_attributes[active_piece.type].states[active_piece.orientation];
        std::set<int> clear_lines{};
        for (const auto& pos_rel : piece_rel_pos) {
            ivec2 absolute_pos = active_piece.position + pos_rel;
            if (absolute_pos.y < 0) {
                std::cout << "Game Over" << std::endl;
                running = false;
                break;
            }
            state[absolute_pos.y][absolute_pos.x] = active_piece.type;
            clear_lines.insert(absolute_pos.y);
        }
        clearLines(clear_lines);
        active_piece.reset(getNextTetromino());
    }

    void updateFall() {
        ivec2 new_position = active_piece.position + ivec2{0, 1};
        if (collisionCheck(new_position, active_piece.orientation)) {
            if (!lock_delay) {
                lock_delay = true;
                lock_delay_start_time = GetTime();
            } else if (lock_delay && GetTime() - lock_delay_start_time >= lock_delay_period) {
                triggerLock();
            }
        } else {
            lock_delay = false;
            active_piece.position = new_position;
        }
    }

    void drawCell(size_t row, size_t col) const {
        if (state[row][col].has_value()) {
            auto type = state[row][col].value();
            Rectangle r{.x = static_cast<float>(offset + col * cell_size),
                        .y = static_cast<float>(offset + row * cell_size),
                        .width = cell_size,
                        .height = cell_size};
            DrawRectangleRounded(r, 0.3, 6, piece_attributes[type].color);
        }
    }

    void update() {
        if (!running) {
            return;
        }
        updateRotation();
        updateTranslation();
        if (tick(tick_rate)) {
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
    board.reset();

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