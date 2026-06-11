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
    ToggleBorderlessWindowed();
    Vector2 ballPosition = {(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2};
    SetTargetFPS(60);

    // Load warrior sprites
    Texture2D warriorIdleTex   = LoadTexture("../pack 1/Units/Blue Units/Warrior/Warrior_Idle.png");
    Texture2D warriorAttackTex = LoadTexture("../pack 1/Units/Blue Units/Warrior/Warrior_Attack1.png");

    int idleFrameWidth   = warriorIdleTex.width / 8;   // 8 idle frames
    int attackFrameWidth = warriorAttackTex.width / 4;  // 4 attack frames
    int frameHeight      = warriorIdleTex.height;

    int currentFrame = 0;
    int frameTimer   = 0;
    int frameSpeed   = 8;

    // Slash state
    bool isSlashing      = false;
    int  slashFrame      = 0;
    int  slashTimer      = 0;
    int  slashFrameSpeed = 5;   // speed of attack animation
    float slashRange     = 100.0f; // radius around player that slash hits

    Bullet bullets[MAX_BULLETS] = {};
    Enemy  enemies[MAX_ENEMIES] = {};

    int shootTimer      = 0;
    int shootCooldown   = 15;
    int spawnTimer      = 0;
    int spawnCooldown   = 30;
    int maxHP           = 1;
    int playerHP        = maxHP;
    int invincibleTimer = 0;
    int invincibleCooldown = 60;
    int score           = 0;
    bool gameOver       = false;

    while (!WindowShouldClose()) {

        if (!gameOver) {
            // Movement
            if (IsKeyDown(KEY_D)) ballPosition.x += 4.0f;
            if (IsKeyDown(KEY_A)) ballPosition.x -= 4.0f;
            if (IsKeyDown(KEY_S)) ballPosition.y += 4.0f;
            if (IsKeyDown(KEY_W)) ballPosition.y -= 4.0f;

            // --- SLASH: auto-trigger when enemy enters melee range ---
            if (!isSlashing) {
                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (enemies[i].active) {
                        float dx   = enemies[i].position.x - ballPosition.x;
                        float dy   = enemies[i].position.y - ballPosition.y;
                        float dist = sqrt(dx * dx + dy * dy);
                        if (dist < slashRange) {
                            isSlashing = true;
                            slashFrame = 0;
                            slashTimer = 0;
                            break;
                        }
                    }
                }
            }

            // Advance slash animation
            if (isSlashing) {
                slashTimer++;
                if (slashTimer >= slashFrameSpeed) {
                    slashTimer = 0;
                    slashFrame++;

                    // On frame 2 (midpoint of swing) — deal damage to all enemies in range
                    if (slashFrame == 2) {
                        for (int i = 0; i < MAX_ENEMIES; i++) {
                            if (enemies[i].active) {
                                float dx   = enemies[i].position.x - ballPosition.x;
                                float dy   = enemies[i].position.y - ballPosition.y;
                                float dist = sqrt(dx * dx + dy * dy);
                                if (dist < slashRange) {
                                    enemies[i].active = false;
                                    score++;
                                }
                            }
                        }
                    }

                    // Animation finished
                    if (slashFrame >= 4) {
                        isSlashing = false;
                        slashFrame = 0;
                    }
                }

                // Reset idle animation frame counter while slashing
                currentFrame = 0;
                frameTimer   = 0;
            }

            // Idle animation (only when not slashing)
            if (!isSlashing) {
                frameTimer++;
                if (frameTimer >= frameSpeed) {
                    frameTimer = 0;
                    currentFrame = (currentFrame + 1) % 8;
                }
            }

            // Auto shoot toward nearest enemy (ranged only — outside slash range)
            shootTimer++;
            if (shootTimer >= shootCooldown) {
                int   nearestIndex = -1;
                float nearestDist  = 250.0f;

                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (enemies[i].active) {
                        float dx   = enemies[i].position.x - ballPosition.x;
                        float dy   = enemies[i].position.y - ballPosition.y;
                        float dist = sqrt(dx * dx + dy * dy);
                        if (dist < nearestDist && dist > slashRange) { // don't shoot melee-range enemies
                            nearestDist  = dist;
                            nearestIndex = i;
                        }
                    }
                }

                if (nearestIndex != -1) {
                    shootTimer = 0;
                    float dx     = enemies[nearestIndex].position.x - ballPosition.x;
                    float dy     = enemies[nearestIndex].position.y - ballPosition.y;
                    float length = sqrt(dx * dx + dy * dy);
                    float speed  = 12.0f;
                    Vector2 velocity = {(dx / length) * speed, (dy / length) * speed};

                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullets[i].active) {
                            bullets[i].position = ballPosition;
                            bullets[i].velocity = velocity;
                            bullets[i].active   = true;
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
                            float dx   = spawnPos.x - ballPosition.x;
                            float dy   = spawnPos.y - ballPosition.y;
                            float dist = sqrt(dx * dx + dy * dy);
                            if (dist > 150) break;
                        } while (true);
                        enemies[i].position = spawnPos;
                        enemies[i].active   = true;
                        break;
                    }
                }
            }

            // Invincible countdown
            if (invincibleTimer > 0) invincibleTimer--;

            // Update enemies
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].active) {
                    float dx     = ballPosition.x - enemies[i].position.x;
                    float dy     = ballPosition.y - enemies[i].position.y;
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
                                enemies[i].active  = false;
                                bullets[j].active  = false;
                                score++;
                                break;
                            }
                        }
                    }
                }
            }

            // Update bullets
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].active) {
                    bullets[i].position.x += bullets[i].velocity.x;
                    bullets[i].position.y += bullets[i].velocity.y;
                    if (bullets[i].position.y < -2000 || bullets[i].position.y > GetScreenHeight() + 2000 ||
                        bullets[i].position.x < -2000 || bullets[i].position.x > GetScreenWidth() + 2000) {
                        bullets[i].active = false;
                    }
                }
            }
        }

        // Restart
        if (gameOver) {
            Rectangle restartBtn = {(float)GetScreenWidth() / 2 - 80, (float)GetScreenHeight() / 2 + 60, 160, 45};
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(GetMousePosition(), restartBtn)) {
                ballPosition    = {(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2};
                playerHP        = maxHP;
                score           = 0;
                invincibleTimer = 0;
                spawnTimer      = 0;
                shootTimer      = 0;
                currentFrame    = 0;
                frameTimer      = 0;
                isSlashing      = false;
                slashFrame      = 0;
                gameOver        = false;
                for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active  = false;
                for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active  = false;
            }
        }

        // ---- DRAW ----
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!gameOver) {
            // Draw slash range circle (faint, for debug — remove later)
            DrawCircleLines((int)ballPosition.x, (int)ballPosition.y, slashRange, BLUE);

            // Draw player — attack or idle sprite
            Color tint = (invincibleTimer > 0 && (invincibleTimer / 6) % 2 == 0) ? RED : WHITE;

            if (isSlashing) {
                Rectangle srcRect  = {(float)(slashFrame * attackFrameWidth), 0, (float)attackFrameWidth, (float)frameHeight};
                Rectangle destRect = {ballPosition.x - 40, ballPosition.y - 40, 200, 200};
                DrawTexturePro(warriorAttackTex, srcRect, destRect, {0, 0}, 0.0f, tint);
            } else {
                Rectangle srcRect  = {(float)(currentFrame * idleFrameWidth), 0, (float)idleFrameWidth, (float)frameHeight};
                Rectangle destRect = {ballPosition.x - 40, ballPosition.y - 40, 200, 200};
                DrawTexturePro(warriorIdleTex, srcRect, destRect, {0, 0}, 0.0f, tint);
            }

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
            for (int i = 0; i < maxHP; i++) {
                Color hpColor = (i < playerHP) ? RED : LIGHTGRAY;
                DrawCircle(70 + i * 35, 22, 12, hpColor);
            }

            // Score
            DrawText(TextFormat("Score: %d", score), 10, 45, 24, DARKGRAY);
        }

        // Game over screen
        if (gameOver) {
            DrawText("GAME OVER", GetScreenWidth() / 2 - 130, GetScreenHeight() / 2 - 60, 50, RED);
            DrawText(TextFormat("Score: %d", score), GetScreenWidth() / 2 - 60, GetScreenHeight() / 2, 30, DARKGRAY);

            Rectangle restartBtn = {(float)GetScreenWidth() / 2 - 80, (float)GetScreenHeight() / 2 + 60, 160, 45};
            Color btnColor = CheckCollisionPointRec(GetMousePosition(), restartBtn) ? DARKGREEN : GRAY;
            DrawRectangleRec(restartBtn, btnColor);
            DrawText("RESTART", GetScreenWidth() / 2 - 52, GetScreenHeight() / 2 + 73, 20, WHITE);
        }

        EndDrawing();
    }

    UnloadTexture(warriorIdleTex);
    UnloadTexture(warriorAttackTex);
    CloseWindow();
    return 0;
}