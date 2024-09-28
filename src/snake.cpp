#include "raylib.h"
#include <algorithm>
#include <deque>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

constexpr int screen_width = 750;
constexpr int screen_height = 750;

constexpr Color green = {173, 204, 96, 255};
constexpr Color dark_green = {43, 51, 24, 255};

constexpr int cell_size = 30;
constexpr int cell_count = 25;
constexpr int offset = 75;

using namespace glm;

auto elementInDeque(ivec2 el, std::deque<ivec2> deq) -> bool {
    return std::any_of(deq.begin(), deq.end(), [el](ivec2 x) { return el == x; });
};

struct Snake {
    std::deque<ivec2> body = {ivec2{6, 9}, ivec2{5, 9}, ivec2{4, 9}};
    ivec2 direction = {1, 0};
    bool add_segment = false;

    void reset() {
        body = {ivec2{6, 9}, ivec2{5, 9}, ivec2{4, 9}};
        direction = {1, 0};
        add_segment = false;
    }

    void draw() {
        for (auto& cell : body) {
            Rectangle r{.x = static_cast<float>(offset + cell[0] * cell_size),
                        .y = static_cast<float>(offset + cell[1] * cell_size),
                        .width = cell_size,
                        .height = cell_size};
            DrawRectangleRounded(r, 0.5, 6, dark_green);
        }
    }

    void update() {
        body.push_front(body[0] + direction);
        if (add_segment) {
            add_segment = false;
        } else {
            body.pop_back();
        }
    }
};

struct Food {
    ivec2 position{};
    Texture2D texture{};

    explicit Food(const std::deque<ivec2>& snake_body) {
        Image image = LoadImage("assets/food.png");
        texture = LoadTextureFromImage(image);
        position = generateRandomPos(snake_body);
        UnloadImage(image);
    }
    ~Food() { UnloadTexture(texture); }
    Food(const Food&) = default;
    Food(Food&&) = delete;
    Food& operator=(const Food&) = default;
    Food& operator=(Food&&) = delete;

    void draw() { DrawTexture(texture, offset + position[0] * cell_size, offset + position[1] * cell_size, WHITE); }
    static auto generateRandomPos(const std::deque<ivec2>& snake_body) -> ivec2 {
        auto generateRandomCell = []() -> ivec2 {
            int x = GetRandomValue(0, cell_count - 1);
            int y = GetRandomValue(0, cell_count - 1);
            return {x, y};
        };

        ivec2 position = generateRandomCell();
        while (elementInDeque(position, snake_body)) {
            position = generateRandomCell();
        }

        return position;
    }
};

struct Game {
    Snake snake{};
    Food food{snake.body};
    bool running = true;
    int score = 0;
    Sound eat_sound{};
    Sound wall_sound{};

    Game() {
        InitAudioDevice();
        eat_sound = LoadSound("assets/eat.mp3");
        wall_sound = LoadSound("assets/wall.mp3");
    }

    Game(const Game&) = default;
    Game(Game&&) = delete;
    Game& operator=(const Game&) = default;
    Game& operator=(Game&&) = delete;
    ~Game() {
        UnloadSound(eat_sound);
        UnloadSound(wall_sound);
        CloseAudioDevice();
    }

    void draw() {
        food.draw();
        snake.draw();
    }

    void update() {
        if (running) {
            snake.update();
            checkCollisionWithFood();
            checkCollisionWithEdge();
            checkCollisionWithTail();
        }
    }

    void checkCollisionWithFood() {
        if (snake.body[0] == food.position) {
            snake.add_segment = true;
            food.position = Food::generateRandomPos(snake.body);
            score++;
            PlaySound(eat_sound);
        }
    }

    void checkCollisionWithEdge() {
        if (snake.body[0][0] == cell_count || snake.body[0][0] == -1 || snake.body[0][1] == cell_count ||
            snake.body[0][1] == -1) {
            gameOver();
        }
    }

    void checkCollisionWithTail() {
        std::deque<ivec2> headless_body = snake.body;
        headless_body.pop_front();
        if (elementInDeque(snake.body[0], headless_body)) {
            gameOver();
        }
    }

    void gameOver() {
        snake.reset();
        food.position = Food::generateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wall_sound);
    }
};

auto event_triggered(double interval) -> bool {
    static double last_update_time = 0;
    double current_time = GetTime();
    if (current_time - last_update_time >= interval) {
        last_update_time = current_time;
        return true;
    }
    return false;
}

auto main() -> int {
    InitWindow(2 * offset + cell_size * cell_count, 2 * offset + cell_size * cell_count, "snake");
    SetTargetFPS(60);

    Game game{};

    while (!WindowShouldClose()) {
        BeginDrawing();

        if (event_triggered(0.2)) {
            // std::cout << GetTime() << std::endl;
            game.update();
        }

        if (IsKeyPressed(KEY_UP) && game.snake.direction[1] != 1) {
            game.snake.direction = {0, -1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction[1] != -1) {
            game.snake.direction = {0, 1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction[0] != 1) {
            game.snake.direction = {-1, 0};
            game.running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction[0] != -1) {
            game.snake.direction = {1, 0};
            game.running = true;
        }

        // Drawing
        ClearBackground(green);
        DrawRectangleLinesEx(
            Rectangle{offset - 5, offset - 5, cell_size * cell_count + 10, cell_size * cell_count + 10}, 5, dark_green);
        DrawText("Retro Snake", offset - 5, 20, 40, dark_green);
        DrawText(TextFormat("%i", game.score), offset - 5, offset + cell_size * cell_count + 10, 40, dark_green);
        game.draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}