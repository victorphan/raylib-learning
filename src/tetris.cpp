#include "tetris.hpp"
#include "board.hpp"
#include "raylib.h"

auto main() -> int {
    InitWindow(tetris::screen_width, tetris::screen_height, "Tetris");
    SetTargetFPS(240);

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