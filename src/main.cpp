#include "raylib.h"

auto main() -> int {
    Vector2 centre = {400, 400};
    float radius = 20;
    Color green = {20, 160, 133, 255};

    InitWindow(800, 800, "Game");

    SetTargetFPS(60);

    // Game Loop
    while (!WindowShouldClose()) {
        // 1. Event handling
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            centre.x += 3;
        } else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            centre.x -= 3;
        } else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
            centre.y += 3;
        } else if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
            centre.y -= 3;
        }

        // 2. Updating Positions

        // 3. Drawing
        BeginDrawing();
        ClearBackground(green);
        DrawCircleV(centre, radius, RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}