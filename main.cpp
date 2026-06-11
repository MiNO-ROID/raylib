#include <raylib.h>

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "raylib");
    ToggleFullscreen();
    Vector2 ballPosition = {(float) screenWidth / 2, (float) screenHeight / 2};
    SetTargetFPS(60);
    while (!WindowShouldClose() ) {
        if (IsKeyDown(KEY_D)) ballPosition.x = ballPosition.x += 4.0f;
        if (IsKeyDown(KEY_A)) ballPosition.x = ballPosition.x -= 4.0f;
        if (IsKeyDown(KEY_S)) ballPosition.y = ballPosition.y += 4.0f;
        if (IsKeyDown(KEY_W)) ballPosition.y = ballPosition.y -= 4.0f;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(ballPosition, 40, MAROON);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}