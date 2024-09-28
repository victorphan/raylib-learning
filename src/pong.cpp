#include "pong.h"
#include "glm/glm.hpp"
#include "raylib.h"

auto main() -> int {
    Ball ball{};
    Paddle player{};
    CpuPaddle cpu_player{};

    int player_score = 0;
    int cpu_score = 0;

    InitWindow(screen_width, screen_height, "pong");

    SetTargetFPS(60);

    ball.reset();

    // Game Loop
    while (!WindowShouldClose()) {
        // 1. Event handling

        // 2. Updating Positions
        ball.update(player_score, cpu_score);
        cpu_player.update(ball.position[1]);
        player.update();

        // Checking for collisions
        if (CheckCollisionCircleRec(
                toVector2(ball.position), ball.radius,
                Rectangle{player.position[0], player.position[1], player.dimension[0], player.dimension[1]})) {
            ball.velocity[0] = -ball.velocity[0];
        }
        if (CheckCollisionCircleRec(toVector2(ball.position), ball.radius,
                                    Rectangle{cpu_player.position[0], cpu_player.position[1], cpu_player.dimension[0],
                                              cpu_player.dimension[1]})) {
            ball.velocity[0] = -ball.velocity[0];
        }

        // 3. Drawing
        BeginDrawing();
        ClearBackground(BLACK);
        DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
        ball.draw();
        cpu_player.draw();
        player.draw();
        DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
        DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}