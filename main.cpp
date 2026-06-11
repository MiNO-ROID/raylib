#include <raylib.h>
#include <cmath>
#define MAX_BULLETS 200

struct Bullet {
    Vector2 position;
    Vector2 velocity;
    bool active;
};

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Shoot");
    ToggleBorderlessWindowed();
    Vector2 ballPosition = {(float)screenWidth / 2, (float)screenHeight / 2};
    SetTargetFPS(60);

    Bullet bullets[MAX_BULLETS] = {};
    int shootTimer = 0;
    int shootCooldown = 10; // shoots every 10 frames

    while (!WindowShouldClose()) {
        // Movement
        if (IsKeyDown(KEY_D)) ballPosition.x += 4.0f;
        if (IsKeyDown(KEY_A)) ballPosition.x -= 4.0f;
        if (IsKeyDown(KEY_S)) ballPosition.y += 4.0f;
        if (IsKeyDown(KEY_W)) ballPosition.y -= 4.0f;

        // Auto shoot toward cursor
        shootTimer++;
        if (shootTimer >= shootCooldown) {
            shootTimer = 0;

            Vector2 mouse = GetMousePosition();
            float dx = mouse.x - ballPosition.x;
            float dy = mouse.y - ballPosition.y;
            float length = sqrt(dx * dx + dy * dy);

            if (length > 0) {
                float speed = 12.0f;
                Vector2 velocity = {(dx / length) * speed, (dy / length) * speed};

                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (!bullets[i].active) {
                        bullets[i].position = ballPosition;
                        bullets[i].velocity = velocity;
                        bullets[i].active = true;
                        break;
                    }
                }
            }
        }
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                bullets[i].position.x += bullets[i].velocity.x;
                bullets[i].position.y += bullets[i].velocity.y;

                if (bullets[i].position.y < -2000 || bullets[i].position.y > screenHeight + 2000 ||
                    bullets[i].position.x < -2000 || bullets[i].position.x > screenWidth + 2000) {
                    bullets[i].active = false;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(ballPosition, 40, BLUE);

        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                DrawCircleV(bullets[i].position, 8, RED);
            }
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}