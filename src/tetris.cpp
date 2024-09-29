#include "tetris.h"
#include "board.hpp"
#include "raylib.h"

auto main() -> int {
    InitWindow(2 * tetris::offset + tetris::cell_size * tetris::num_cols,
               2 * tetris::offset + tetris::cell_size * tetris::num_rows, "Tetris");
    SetTargetFPS(60);

    tetris::Board board{};

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