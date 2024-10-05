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

    int level = 0;
    int current_level_lines_cleared = 0;
    double tick_rate = level_tick_rates[level];

    SlideState slide_state{SlideState::Inactive};
    double slide_timer = 0;

    // 7 bag randomization
    std::random_device rd;
    std::mt19937 g{rd()};
    std::array<Tetromino, Tetromino::NUM_TETROMINOS> random_bag_0 = {I, J, L, O, S, T, Z};
    std::array<Tetromino, Tetromino::NUM_TETROMINOS> random_bag_1 = {I, J, L, O, S, T, Z};
    std::array<Tetromino, num_next_pieces + 1> curr_and_next_pieces{};
    std::array<Tetromino, Tetromino::NUM_TETROMINOS>::iterator bag_pointer;
    bool bag_0 = true;

    Board() { reset(); }

    void reset();
    auto getNextTetromino() -> Tetromino;
    auto tick(double interval) -> bool;
    [[nodiscard]] auto collisionCheck(Tetromino type, ivec2 pos_bound, Orientation orientation) const -> bool;
    void handleRotationTests(Orientation const& current_orientation, bool clockwise);
    void updateRotation();
    void updateHorizontalTranslation();
    void updateVerticalTranslation();
    void clearLine(int line);
    void clearLines(std::set<int>& lines);
    void translate(ivec2 translation);
    void triggerLock();
    void updateSpawn();
    void updateFall();
    void updateSlideState(ivec2 translation);
    void update();
    void updateGhostPiece();
    void updateHoldPiece();
    void drawCell(int row, int col) const;
    static void drawGrid();
    void draw() const;
};

inline void Board::reset() {
    state = {};
    std::shuffle(random_bag_0.begin(), random_bag_0.end(), g);
    std::shuffle(random_bag_1.begin(), random_bag_1.end(), g);
    bag_pointer = random_bag_0.begin();
    active_piece.type = *bag_pointer;
    for (size_t i = 0; i < num_next_pieces; i++) {
        curr_and_next_pieces[i] = *bag_pointer++;
    }
    curr_and_next_pieces[num_next_pieces] = *bag_pointer++;
    bag_pointer = random_bag_1.begin();
    bag_0 = false;
}

inline auto Board::getNextTetromino() -> Tetromino {
    for (size_t i = 0; i < num_next_pieces; i++) {
        curr_and_next_pieces[i] = curr_and_next_pieces[i + 1];
    }
    curr_and_next_pieces[num_next_pieces] = *bag_pointer++;
    if (bag_0 && bag_pointer == random_bag_0.end()) {
        std::shuffle(random_bag_0.begin(), random_bag_0.end(), g);
        bag_pointer = random_bag_1.begin();
        bag_0 = false;
    } else if (!bag_0 && bag_pointer == random_bag_1.end()) {
        std::shuffle(random_bag_0.begin(), random_bag_0.end(), g);
        bag_pointer = random_bag_0.begin();
        bag_0 = true;
    }
    return curr_and_next_pieces[0];
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

inline void Board::updateHorizontalTranslation() {
    if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT)) {
        updateSlideState(ivec2{-1, 0});
    } else if (IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT)) {
        updateSlideState(ivec2{1, 0});
    } else {
        slide_state = SlideState::Inactive;
    }
}

inline void Board::updateVerticalTranslation() {
    if (IsKeyPressed(KEY_SPACE)) {
        active_piece.position = ghost_piece.position;
        triggerLock();
        return;
    }
    if (IsKeyDown(KEY_DOWN)) {
        tick_rate = level_down_tick_rates[level];
    } else {
        tick_rate = level_tick_rates[level];
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

    // This _can_ be optimized, but it doesn't _need_ to be.
    for (auto line : lines) {
        clearLine(line);
        current_level_lines_cleared++;
    }
    if (level + 1 < num_levels && current_level_lines_cleared >= 10) {
        level++;
        current_level_lines_cleared = 0;
    }
}

inline void Board::updateSpawn() {
    if (!collisionCheck(active_piece.type, active_piece.position, active_piece.orientation)) {
        return;
    }
    if (!collisionCheck(active_piece.type, active_piece.position - ivec2{0, 1}, active_piece.orientation)) {
        active_piece.position -= ivec2{0, 1};
        return;
    }
    if (!collisionCheck(active_piece.type, active_piece.position - ivec2{0, 2}, active_piece.orientation)) {
        active_piece.position -= ivec2{0, 2};
        return;
    }
    std::cout << "Game Over" << std::endl;
    running = false;
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
    if (std::all_of(clear_lines.begin(), clear_lines.end(), [](int line) -> bool { return line < 2; })) {
        std::cout << "Game Over" << std::endl;
        running = false;
    }
    clearLines(clear_lines);
    active_piece.reset(getNextTetromino());
    just_swapped_hold = false;
    last_update_time = GetTime();
    updateSpawn();
}

inline void Board::updateFall() {
    ivec2 new_position = active_piece.position + ivec2{0, 1};
    if (!collisionCheck(active_piece.type, new_position, active_piece.orientation)) {
        lock_delay = false;
        active_piece.position = new_position;
    } else if (!lock_delay) {
        lock_delay = true;
        lock_delay_start_time = GetTime();
    } else if (GetTime() - lock_delay_start_time >= lock_delay_period) {
        lock_delay = false;
        triggerLock();
    }
}

inline void Board::drawCell(int row, int col) const {
    if (state[row][col].has_value()) {
        auto type = state[row][col].value();
        Rectangle r{.x = static_cast<float>(hold_width + offset + static_cast<float>(col) * cell_size),
                    .y = static_cast<float>(offset + static_cast<float>(row - 2) * cell_size),
                    .width = cell_size,
                    .height = cell_size};
        DrawRectangleRounded(r, 0.4, 6, piece_attributes[type].color);
    }
}

inline void Board::updateHoldPiece() {
    if (IsKeyPressed(KEY_LEFT_SHIFT) && !just_swapped_hold) {
        just_swapped_hold = true;
        Tetromino temp = active_piece.type;
        if (hold_piece.has_value()) {
            active_piece.reset(hold_piece.value());
        } else {
            active_piece.reset(getNextTetromino());
        }
        updateSpawn();
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
    updateVerticalTranslation();
    updateHorizontalTranslation();
    if (tick(tick_rate)) {
        updateFall();
    }
    updateGhostPiece();
}

inline void Board::drawGrid() {
    for (int row = 0; row < num_rows - 1; row++) {
        DrawLine(hold_width + offset, row * cell_size + offset, hold_width + offset + num_cols * cell_size,
                 row * cell_size + offset, DARKGRAY);
    }
    for (int col = 0; col <= num_cols; col++) {
        DrawLine(hold_width + offset + col * cell_size, offset, hold_width + offset + col * cell_size,
                 offset + (num_rows - 2) * cell_size, DARKGRAY);
    }
}

inline void Board::draw() const {
    for (int row = 2; row < num_rows; row++) {
        for (int col = 0; col < num_cols; col++) {
            drawCell(row, col);
        }
    }
    drawGrid();
    if (hold_piece.has_value()) {
        Piece::drawTetromino(hold_piece.value(), hold_position, tiny_piece_size);
    }

    for (int i = 0; i < num_next_pieces; i++) {
        Piece::drawTetromino(curr_and_next_pieces[i + 1], next_position + i * ivec2{0, next_position_spacing},
                             tiny_piece_size);
    }

    if (!running) {
        return;
    }
    active_piece.draw();
    ghost_piece.drawGhost();

    DrawText(TextFormat("Hold"), 45, 30, 20, WHITE);
    DrawText(TextFormat("Next"), 485, 30, 20, WHITE);
    DrawText(TextFormat("Level: %i", level + 1), 30, 500, 20, WHITE);
}

} // namespace tetris