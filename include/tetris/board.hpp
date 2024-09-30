#pragma once

#include "piece.hpp"
#include "raylib.h"
#include "tetris.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <glm/ext/vector_int2.hpp>
#include <iostream>
#include <optional>
#include <random>
#include <set>

namespace tetris {

struct Board {
    std::array<std::array<std::optional<Tetromino>, num_cols>, num_rows> state{};
    Piece active_piece{Tetromino{}};

    double last_update_time = GetTime();
    bool lock_delay = false;
    double lock_delay_start_time = 0;
    bool running = true;
    double level_tick_rate = 0.2;
    double tick_rate = level_tick_rate;

    bool start_slide = false;
    double slide_time_start = 0;
    double slide_rate = 0.06;

    // 7 bag randomization
    std::random_device rd;
    std::mt19937 g{rd()};
    std::array<Tetromino, Tetromino::COUNT> random_bag_0 = {I, J, L, O, S, T, Z};
    std::array<Tetromino, Tetromino::COUNT> random_bag_1 = {I, J, L, O, S, T, Z};
    std::array<Tetromino, Tetromino::COUNT>::iterator next_piece;
    bool bag_0 = true;

    Board() { reset(); }

    void reset();
    auto getNextTetromino() -> Tetromino;
    auto tick(double interval) -> bool;
    auto collisionCheck(ivec2 pos_bound, int orientation) -> bool;
    void handleRotationTests(Orientation current_orientation, Rotation rotation);
    void updateRotation();
    void updateTranslation();
    void clearLine(int line);
    void clearLines(std::set<int>& lines);
    void triggerLock();
    void updateFall();
    void translate(ivec2 translation);
    void drawCell(size_t row, size_t col) const;
    void update();
    void draw() const;
};

inline void Board::reset() {
    state = {};
    std::shuffle(random_bag_0.begin(), random_bag_0.end(), g);
    std::shuffle(random_bag_1.begin(), random_bag_1.end(), g);
    next_piece = random_bag_0.begin();
    active_piece.type = *next_piece;
}

inline auto Board::getNextTetromino() -> Tetromino {
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

inline auto Board::tick(double interval) -> bool {
    double current_time = GetTime();
    if (current_time - last_update_time >= interval) {
        last_update_time = current_time;
        return true;
    }
    return false;
}

inline auto Board::collisionCheck(ivec2 pos_bound, int orientation) -> bool {
    const auto& piece_rel_pos = piece_attributes[active_piece.type].states[orientation];
    return std::any_of(piece_rel_pos.begin(), piece_rel_pos.end(), [pos_bound, this](const ivec2& rel_pos) {
        ivec2 absolute_pos = rel_pos + pos_bound;
        bool within_bounds =
            0 <= absolute_pos.x && absolute_pos.x < num_cols && 0 <= absolute_pos.y && absolute_pos.y < num_rows;
        return !within_bounds || state[absolute_pos.y][absolute_pos.x].has_value();
    });
}

inline void Board::handleRotationTests(Orientation current_orientation, Rotation rotation) {
    const WallTests& wall_tests = active_piece.type == I ? wall_kick_tests_i : wall_kick_tests_not_i;
    for (const ivec2& wall_test : (wall_tests)[current_orientation][rotation]) {
        ivec2 new_position{active_piece.position.x + wall_test.x, active_piece.position.y - wall_test.y};
        Orientation new_orientation =
            rotation == Clockwise ? rotateClockwise(current_orientation) : rotateAntiClockwise(current_orientation);
        if (!collisionCheck(new_position, new_orientation)) {
            active_piece.orientation = new_orientation;
            active_piece.position = new_position;
            break;
        }
    }
}

inline void Board::updateRotation() {
    if (IsKeyPressed(KEY_J)) {
        lock_delay = false;
        handleRotationTests(active_piece.orientation, AntiClockwise);
    } else if (IsKeyPressed(KEY_K)) {
        lock_delay = false;
        handleRotationTests(active_piece.orientation, Clockwise);
    }
}

inline void Board::translate(ivec2 translation) {
    if (!start_slide || (start_slide && GetTime() - slide_time_start > slide_rate)) {
        start_slide = true;
        slide_time_start = GetTime();
        ivec2 new_position = active_piece.position + translation;
        if (!collisionCheck(new_position, active_piece.orientation)) {
            lock_delay = false;
            active_piece.position = new_position;
        }
    }
}

inline void Board::updateTranslation() {
    if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
        translate(ivec2{-1, 0});
    } else if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
        translate(ivec2{1, 0});
    } else {
        start_slide = false;
    }
    if (IsKeyDown(KEY_S)) {
        tick_rate = 0.05;
    } else {
        tick_rate = level_tick_rate;
    }
}

inline void Board::clearLine(int line) {
    assert(0 <= line && line < num_rows);
    for (int line_write = line; line_write > 0; line_write--) {
        state[line_write] = state[line_write - 1];
    }
}

inline void Board::clearLines(std::set<int>& lines) {
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

inline void Board::triggerLock() {
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

inline void Board::updateFall() {
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

inline void Board::drawCell(size_t row, size_t col) const {
    if (state[row][col].has_value()) {
        auto type = state[row][col].value();
        Rectangle r{.x = static_cast<float>(offset + col * cell_size),
                    .y = static_cast<float>(offset + row * cell_size),
                    .width = cell_size,
                    .height = cell_size};
        DrawRectangleRounded(r, 0.3, 6, piece_attributes[type].color);
    }
}

inline void Board::update() {
    if (!running) {
        return;
    }
    updateRotation();
    updateTranslation();
    if (tick(tick_rate)) {
        updateFall();
    }
}

inline void Board::draw() const {
    for (size_t row = 0; row < num_rows; row++) {
        for (size_t col = 0; col < num_cols; col++) {
            drawCell(row, col);
        }
    }

    active_piece.draw();
}

} // namespace tetris