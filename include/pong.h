#include "glm/glm.hpp"
#include "raylib.h"

constexpr int screen_width = 1280;
constexpr int screen_height = 800;
constexpr float ball_radius = 15;
constexpr int rectangle_height = 120;
constexpr int rectangle_width = 25;
constexpr int padding = 10;

inline auto toVector2(const glm::vec2& v) -> Vector2 { return Vector2{v[0], v[1]}; }

struct Paddle {
    glm::vec2 position{screen_width - rectangle_width - padding, (screen_height - rectangle_height) / 2};
    glm::vec2 dimension{rectangle_width, rectangle_height};
    float speed{10};

    void draw() {
        DrawRectangle(static_cast<int>(position[0]), static_cast<int>(position[1]), static_cast<int>(dimension[0]),
                      static_cast<int>(dimension[1]), WHITE);
    }

    void update() {
        if (IsKeyDown(KEY_UP)) {
            position[1] -= speed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            position[1] += speed;
        }
        if (position[1] < 0) {
            position[1] = 0;
        }
        if (position[1] + rectangle_height > screen_height) {
            position[1] = screen_height - rectangle_height;
        }
    }
};

struct CpuPaddle {
    glm::vec2 position{padding, (screen_height - rectangle_height) / 2};
    glm::vec2 dimension{rectangle_width, rectangle_height};
    float speed{6};

    void draw() {
        DrawRectangle(static_cast<int>(position[0]), static_cast<int>(position[1]), static_cast<int>(dimension[0]),
                      static_cast<int>(dimension[1]), WHITE);
    }

    void update(float ball_y) {
        if ((position[1] + (static_cast<float>(rectangle_height) / 2)) > ball_y) {
            position[1] -= speed;
        } else {
            position[1] += speed;
        }
        if (position[1] < 0) {
            position[1] = 0;
        }
        if (position[1] + rectangle_height > screen_height) {
            position[1] = screen_height - rectangle_height;
        }
    }
};

struct Ball {
    glm::vec2 position{screen_width / 2, screen_height / 2};
    glm::vec2 velocity{7, 7};
    float radius{ball_radius};

    [[nodiscard]] auto onVerticalEdge() const -> bool {
        return position[1] <= radius || position[1] >= screen_height - radius;
    }

    void reset() {
        position = {screen_width / 2, screen_height / 2};
        glm::vec2 speed_choices = {-1, 1};
        velocity[0] *= speed_choices[GetRandomValue(0, 1)];
        velocity[1] *= speed_choices[GetRandomValue(0, 1)];
    }
    void draw() const { DrawCircleV(toVector2(position), radius, WHITE); }
    void updateEdge(int& player_score, int& cpu_score) {
        if (position[0] <= radius) {
            player_score++;
            reset();
        } else if (position[0] >= screen_width - radius) {
            cpu_score++;
            reset();
        }
        if (onVerticalEdge()) {
            velocity[1] = -velocity[1];
        }
    }
    void update(int& player_score, int& cpu_score) {
        updateEdge(player_score, cpu_score);
        position += velocity;
    }
};
