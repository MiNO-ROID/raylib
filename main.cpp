#include <raylib.h>
#include <cmath>
#define MAX_BULLETS 200
#define MAX_ENEMIES 20

struct Bullet {
    Vector2 position;
    Vector2 velocity;
    bool active;
};

struct Enemy {
    Vector2 position;
    bool active;
};

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Shoot");
    Vector2 ballPosition = {(float)screenWidth / 2, (float)screenHeight / 2};
    SetTargetFPS(60);

    Bullet bullets[MAX_BULLETS] = {};
    Enemy enemies[MAX_ENEMIES] = {};

    int shootTimer = 0;
    int shootCooldown = 15;
    int spawnTimer = 0;
    int spawnCooldown = 30;
    int playerHP = 5;
    int invincibleTimer = 0;
    int invincibleCooldown = 60;
    int score = 0;
    bool gameOver = false;

    while (!WindowShouldClose()) {

        if (!gameOver) {
            // Movement
            if (IsKeyDown(KEY_D)) ballPosition.x += 4.0f;
            if (IsKeyDown(KEY_A)) ballPosition.x -= 4.0f;
            if (IsKeyDown(KEY_S)) ballPosition.y += 4.0f;
            if (IsKeyDown(KEY_W)) ballPosition.y -= 4.0f;

            // Shoot
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

            // Spawn enemy
            spawnTimer++;
            if (spawnTimer >= spawnCooldown) {
                spawnTimer = 0;
                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (!enemies[i].active) {
                        Vector2 spawnPos;
                        do {
                            spawnPos = {
                                (float)GetRandomValue(0, GetScreenWidth()),
                                (float)GetRandomValue(0, GetScreenHeight())
                            };
                            float dx = spawnPos.x - ballPosition.x;
                            float dy = spawnPos.y - ballPosition.y;
                            float dist = sqrt(dx * dx + dy * dy);
                            if (dist > 150) break;
                        } while (true);
                        enemies[i].position = spawnPos;
                        enemies[i].active = true;
                        break;
                    }
                }
            }

            // Invincible countdown
            if (invincibleTimer > 0) invincibleTimer--;

            // Update enemies
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].active) {
                    float dx = ballPosition.x - enemies[i].position.x;
                    float dy = ballPosition.y - enemies[i].position.y;
                    float length = sqrt(dx * dx + dy * dy);
                    if (length > 0) {
                        enemies[i].position.x += (dx / length) * 2.0f;
                        enemies[i].position.y += (dy / length) * 2.0f;
                    }

                    // Enemy touches player
                    if (invincibleTimer == 0) {
                        if (CheckCollisionCircles(enemies[i].position, 20, ballPosition, 40)) {
                            playerHP--;
                            invincibleTimer = invincibleCooldown;
                            enemies[i].active = false;
                            if (playerHP <= 0) gameOver = true;
                        }
                    }

                    // Bullet hits enemy
                    for (int j = 0; j < MAX_BULLETS; j++) {
                        if (bullets[j].active) {
                            if (CheckCollisionCircles(enemies[i].position, 20, bullets[j].position, 8)) {
                                enemies[i].active = false;
                                bullets[j].active = false;
                                score++;
                                break;
                            }
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
        }

        // Restart
        if (gameOver) {
            Rectangle restartBtn = {(float)screenWidth / 2 - 80, (float)screenHeight / 2 + 60, 160, 45};
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(GetMousePosition(), restartBtn)) {
                ballPosition = {(float)screenWidth / 2, (float)screenHeight / 2};
                playerHP = 5;
                score = 0;
                invincibleTimer = 0;
                spawnTimer = 0;
                shootTimer = 0;
                gameOver = false;
                for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
                for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!gameOver) {
            // Player
            Color playerColor = (invincibleTimer > 0 && (invincibleTimer / 6) % 2 == 0) ? RED : BLUE;
            DrawCircleV(ballPosition, 40, playerColor);

            // Enemies
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].active) DrawCircleV(enemies[i].position, 20, RED);
            }

            // Bullets
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].active) DrawCircleV(bullets[i].position, 8, ORANGE);
            }

            // HP bar
            DrawText("HP:", 10, 10, 24, DARKGRAY);
            for (int i = 0; i < 5; i++) {
                Color hpColor = (i < playerHP) ? RED : LIGHTGRAY;
                DrawCircle(70 + i * 35, 22, 12, hpColor);
            }

            // Score
            DrawText(TextFormat("Score: %d", score), 10, 45, 24, DARKGRAY);
        }

        // Game over screen
        if (gameOver) {
            DrawText("GAME OVER", screenWidth / 2 - 130, screenHeight / 2 - 60, 50, RED);
            DrawText(TextFormat("Score: %d", score), screenWidth / 2 - 60, screenHeight / 2, 30, DARKGRAY);

            Rectangle restartBtn = {(float)screenWidth / 2 - 80, (float)screenHeight / 2 + 60, 160, 45};
            Color btnColor = CheckCollisionPointRec(GetMousePosition(), restartBtn) ? DARKGREEN : GRAY;
            DrawRectangleRec(restartBtn, btnColor);
            DrawText("RESTART", screenWidth / 2 - 52, screenHeight / 2 + 73, 20, WHITE);
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}