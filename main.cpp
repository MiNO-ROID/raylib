#include <raylib.h>
#include <cmath>
#define MAX_BULLETS 50

struct Bullet {
    Vector2 position;
    Vector2 velocity;
    bool active;
};

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Shoot");
    // ToggleFullscreen();
    Vector2 ballPosition = {(float) screenWidth / 2, (float) screenHeight / 2};
    SetTargetFPS(60);

    Bullet bullets[MAX_BULLETS] = {};
    while (!WindowShouldClose() ) {
        //Movement
        if (IsKeyDown(KEY_D)) ballPosition.x = ballPosition.x += 4.0f;
        if (IsKeyDown(KEY_A)) ballPosition.x = ballPosition.x -= 4.0f;
        if (IsKeyDown(KEY_S)) ballPosition.y = ballPosition.y += 4.0f;
        if (IsKeyDown(KEY_W)) ballPosition.y = ballPosition.y -= 4.0f;

        //Shoot ( MOUSE CLICK )
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();

            float dx = mouse.x - ballPosition.x;
            float dy = mouse.y - ballPosition.y;
            float length = sqrt(dx * dx + dy * dy);

            if (length > 0 ) {
                float speed = 8.0f;
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

        for (int i=0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                bullets[i].position.x += bullets[i].velocity.x;
                bullets[i].position.y += bullets[i].velocity.y;
                if (bullets[i].position.y < 0 || bullets[i].position.y > screenHeight ||
                    bullets[i].position.x < 0 || bullets[i].position.x > screenWidth) {
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