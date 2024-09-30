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
    Piece ghost_piece{Tetromino{}};
    std::optional<Tetromino> hold_piece;
    bool just_swapped_hold = false;

    double last_update_time = GetTime();
    bool lock_delay = false;
    double lock_delay_start_time = 0;
    bool running = true;
    double tick_rate = level_tick_rate;

    SlideState slide_state{SlideState::Inactive};
    double slide_timer = 0;

    // 7 bag randomization
    std::random_device rd;
    std::mt19937 g{rd()};
    std::array<Tetromino, Tetromino::NUM_TETROMINOS> random_bag_0 = {I, J, L, O, S, T, Z};
    std::array<Tetromino, Tetromino::NUM_TETROMINOS> random_bag_1 = {I, J, L, O, S, T, Z};
    std::array<Tetromino, 2> next_pieces{};
    std::array<Tetromino, Tetromino::NUM_TETROMINOS>::iterator bag_pointer;
    bool bag_0 = true;

    Board() { reset(); }

    void reset();
    auto getNextTetromino() -> Tetromino;
    auto tick(double interval) -> bool;
    [[nodiscard]] auto collisionCheck(Tetromino type, ivec2 pos_bound, Orientation orientation) const -> bool;
    void handleRotationTests(Orientation const& current_orientation, bool clockwise);
    void updateRotation();
    void updateTranslation();
    void clearLine(int line);
    void clearLines(std::set<int>& lines);
    void translate(ivec2 translation);
    void triggerLock();
    void updateFall();
    void updateSlideState(ivec2 translation);
    void drawCell(size_t row, size_t col) const;
    void update();
    void updateGhostPiece();
    void updateHoldPiece();
    void draw() const;
};

inline void Board::reset() {
    state = {};
    std::shuffle(random_bag_0.begin(), random_bag_0.end(), g);
    std::shuffle(random_bag_1.begin(), random_bag_1.end(), g);
    bag_pointer = random_bag_0.begin();
    active_piece.type = *bag_pointer;
    next_pieces[0] = *bag_pointer++;
    next_pieces[1] = *bag_pointer++;
}

inline auto Board::getNextTetromino() -> Tetromino {
    next_pieces[0] = next_pieces[1];
    next_pieces[1] = *bag_pointer;
    bag_pointer++;
    if (bag_0 && bag_pointer == random_bag_0.end()) {
        std::shuffle(random_bag_0.begin(), random_bag_0.end(), g);
        bag_pointer = random_bag_1.begin();
        bag_0 = false;
    } else if (!bag_0 && bag_pointer == random_bag_1.end()) {
        std::shuffle(random_bag_0.begin(), random_bag_0.end(), g);
        bag_pointer = random_bag_0.begin();
        bag_0 = true;
    }
    return next_pieces[0];
}

inline auto Board::tick(double interval) -> bool {
    double current_time = GetTime();
    if (current_time - last_update_time >= interval) {
        last_update_time = current_time;
        return true;
    }
    return false;
}

inline auto Board::collisionCheck(Tetromino type, ivec2 pos_bound, Orientation orientation) const -> bool {
    auto const& piece_rel_pos = piece_attributes[type].states[orientation];
    return std::any_of(piece_rel_pos.begin(), piece_rel_pos.end(), [pos_bound, this](ivec2 const& rel_pos) {
        ivec2 absolute_pos = rel_pos + pos_bound;
        bool within_bounds =
            0 <= absolute_pos.x && absolute_pos.x < num_cols && absolute_pos.y < num_rows && 0 <= absolute_pos.y;
        return !within_bounds || state[absolute_pos.y][absolute_pos.x].has_value();
    });
}

inline void Board::handleRotationTests(Orientation const& current_orientation, bool clockwise) {
    WallTests const& wall_tests = active_piece.type == I ? wall_kick_tests_i : wall_kick_tests_not_i;
    Orientation new_orientation = clockwise ? current_orientation++ : current_orientation--;
    for (ivec2 const& wall_test : wall_tests[current_orientation][static_cast<size_t>(clockwise)]) {
        ivec2 new_position{active_piece.position.x + wall_test.x, active_piece.position.y - wall_test.y};
        if (!collisionCheck(active_piece.type, new_position, new_orientation)) {
            lock_delay = false;
            active_piece.orientation = new_orientation;
            active_piece.position = new_position;
            break;
        }
    }
}

inline void Board::updateRotation() {
    if (active_piece.type == Tetromino::O) {
        return;
    }
    if (IsKeyPressed(KEY_Z)) {
        handleRotationTests(active_piece.orientation, false);
    } else if (IsKeyPressed(KEY_X)) {
        handleRotationTests(active_piece.orientation, true);
    }
}

inline void Board::translate(ivec2 translation) {
    ivec2 new_position = active_piece.position + translation;
    if (!collisionCheck(active_piece.type, new_position, active_piece.orientation)) {
        lock_delay = false;
        active_piece.position = new_position;
    }
}

inline void Board::updateSlideState(ivec2 translation) {
    if (slide_state == SlideState::Inactive) {
        slide_state = SlideState::StartDelay;
        slide_timer = GetTime();
        translate(translation);
    } else if (slide_state == SlideState::StartDelay && GetTime() - slide_timer > slide_delay_period) {
        slide_state = SlideState::Slide;
        slide_timer = GetTime();
    } else if (slide_state == SlideState::Slide && GetTime() - slide_timer > slide_rate) {
        slide_timer = GetTime();
        translate(translation);
    }
}

inline void Board::updateTranslation() {
    if (IsKeyPressed(KEY_SPACE)) {
        active_piece.position = ghost_piece.position;
        triggerLock();
        return;
    }

    if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT)) {
        updateSlideState(ivec2{-1, 0});
    } else if (IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT)) {
        updateSlideState(ivec2{1, 0});
    } else {
        slide_state = SlideState::Inactive;
    }

    if (IsKeyDown(KEY_DOWN)) {
        tick_rate = down_tick_rate;
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
    auto const& piece_rel_pos = piece_attributes[active_piece.type].states[active_piece.orientation];
    std::set<int> clear_lines{};
    for (auto const& pos_rel : piece_rel_pos) {
        ivec2 absolute_pos = active_piece.position + pos_rel;
        state[absolute_pos.y][absolute_pos.x] = active_piece.type;
        clear_lines.insert(absolute_pos.y);
    }
    clearLines(clear_lines);
    active_piece.reset(getNextTetromino());
    if (collisionCheck(active_piece.type, active_piece.position, active_piece.orientation)) {
        std::cout << "Game Over" << std::endl;
        running = false;
    }
}

inline void Board::updateFall() {
    ivec2 new_position = active_piece.position + ivec2{0, 1};
    if (collisionCheck(active_piece.type, new_position, active_piece.orientation)) {
        if (!lock_delay) {
            lock_delay = true;
            lock_delay_start_time = GetTime();
        } else if (GetTime() - lock_delay_start_time >= lock_delay_period) {
            lock_delay = false;
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
                    .y = static_cast<float>(offset + (row - 2) * cell_size),
                    .width = cell_size,
                    .height = cell_size};
        DrawRectangleRounded(r, 0.3, 6, piece_attributes[type].color);
    }
}

inline void Board::updateHoldPiece() {
    if (IsKeyPressed(KEY_LEFT_SHIFT)) {
        Tetromino temp = active_piece.type;
        if (hold_piece.has_value()) {
            active_piece.reset(hold_piece.value());
        } else {
            active_piece.reset(getNextTetromino());
        }
        hold_piece = temp;
    }
}

inline void Board::updateGhostPiece() {
    ivec2 target_position = active_piece.position;
    while (!collisionCheck(active_piece.type, target_position + ivec2{0, 1}, active_piece.orientation)) {
        target_position.y++;
    }
    ghost_piece.position = target_position;
    ghost_piece.orientation = active_piece.orientation;
    ghost_piece.type = active_piece.type;
}

inline void Board::update() {
    if (!running) {
        return;
    }
    updateHoldPiece();
    updateRotation();
    updateTranslation();
    if (tick(tick_rate)) {
        updateFall();
    }
    updateGhostPiece();
}

inline void Board::draw() const {
    for (size_t row = 2; row < num_rows; row++) {
        for (size_t col = 0; col < num_cols; col++) {
            drawCell(row, col);
        }
    }
    active_piece.draw();
    ghost_piece.drawGhost();
}

} // namespace tetris