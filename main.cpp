#include <raylib.h>
#include <cmath>
#define MAX_ENEMIES 20

struct Enemy {
    Vector2 position;
    bool active;
};

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Slash");
    ToggleBorderlessWindowed();
    Vector2 ballPosition = {(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2};
    SetTargetFPS(60);

    // Load warrior sprites
    Texture2D warriorIdleTex   = LoadTexture("../pack 1/Units/Blue Units/Warrior/Warrior_Idle.png");
    Texture2D warriorRunTex    = LoadTexture("../pack 1/Units/Blue Units/Warrior/Warrior_Run.png");
    Texture2D warriorAttackTex = LoadTexture("../pack 1/Units/Blue Units/Warrior/Warrior_Attack1.png");

    int idleFrameWidth   = warriorIdleTex.width / 8;   // 8 frames
    int runFrameWidth    = warriorRunTex.width  / 6;   // 6 frames
    int attackFrameWidth = warriorAttackTex.width / 4; // 4 frames
    int frameHeight      = warriorIdleTex.height;
    int drawSize         = 200;
    int drawOffset       = drawSize / 2;

    int currentFrame = 0;
    int frameTimer   = 0;
    int frameSpeed   = 8;

    // Slash state
    bool isSlashing      = false;
    int  slashFrame      = 0;
    int  slashTimer      = 0;
    int  slashFrameSpeed = 5;
    float slashRange     = 80.0f;

    Enemy enemies[MAX_ENEMIES] = {};

    int spawnTimer         = 0;
    int spawnCooldown      = 30;
    int maxHP              = 3;
    int playerHP           = maxHP;
    int invincibleTimer    = 0;
    int invincibleCooldown = 60;
    int score              = 0;
    bool gameOver          = false;

    while (!WindowShouldClose()) {

        if (!gameOver) {
            bool isMoving = false;

            // Movement
            if (IsKeyDown(KEY_D)) { ballPosition.x += 4.0f; isMoving = true; }
            if (IsKeyDown(KEY_A)) { ballPosition.x -= 4.0f; isMoving = true; }
            if (IsKeyDown(KEY_S)) { ballPosition.y += 4.0f; isMoving = true; }
            if (IsKeyDown(KEY_W)) { ballPosition.y -= 4.0f; isMoving = true; }

            // SLASH: trigger when any enemy enters melee range
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

                    if (slashFrame >= 4) {
                        isSlashing = false;
                        slashFrame = 0;
                    }
                }
                currentFrame = 0;
                frameTimer   = 0;
            }

            // Idle / Run animation (only when not slashing)
            if (!isSlashing) {
                int maxFrames = isMoving ? 6 : 8;
                frameTimer++;
                if (frameTimer >= frameSpeed) {
                    frameTimer = 0;
                    currentFrame = (currentFrame + 1) % maxFrames;
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

                    if (!isSlashing && invincibleTimer == 0) {
                        if (CheckCollisionCircles(enemies[i].position, 20, ballPosition, 30)) {
                            playerHP--;
                            invincibleTimer = invincibleCooldown;
                            enemies[i].active = false;
                            if (playerHP <= 0) gameOver = true;
                        }
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
                currentFrame    = 0;
                frameTimer      = 0;
                isSlashing      = false;
                slashFrame      = 0;
                gameOver        = false;
                for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
            }
        }

        // ---- DRAW ----
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!gameOver) {
            DrawCircleLines((int)ballPosition.x, (int)ballPosition.y, slashRange, BLUE);

            Color tint = (invincibleTimer > 0 && (invincibleTimer / 6) % 2 == 0) ? RED : WHITE;
            bool isMovingDraw = IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D);

            if (isSlashing) {
                Rectangle srcRect  = {(float)(slashFrame * attackFrameWidth), 0, (float)attackFrameWidth, (float)frameHeight};
                Rectangle destRect = {ballPosition.x - drawOffset, ballPosition.y - drawOffset, (float)drawSize, (float)drawSize};
                DrawTexturePro(warriorAttackTex, srcRect, destRect, {0, 0}, 0.0f, tint);
            } else if (isMovingDraw) {
                Rectangle srcRect  = {(float)(currentFrame * runFrameWidth), 0, (float)runFrameWidth, (float)frameHeight};
                Rectangle destRect = {ballPosition.x - drawOffset, ballPosition.y - drawOffset, (float)drawSize, (float)drawSize};
                DrawTexturePro(warriorRunTex, srcRect, destRect, {0, 0}, 0.0f, tint);
            } else {
                Rectangle srcRect  = {(float)(currentFrame * idleFrameWidth), 0, (float)idleFrameWidth, (float)frameHeight};
                Rectangle destRect = {ballPosition.x - drawOffset, ballPosition.y - drawOffset, (float)drawSize, (float)drawSize};
                DrawTexturePro(warriorIdleTex, srcRect, destRect, {0, 0}, 0.0f, tint);
            }

            // Debug state label
            const char* stateLabel = isSlashing ? "SLASHING" : (isMovingDraw ? "RUNNING" : "IDLE");
            Color stateColor = isSlashing ? GREEN : (isMovingDraw ? ORANGE : DARKGRAY);
            DrawText(stateLabel, 10, 75, 20, stateColor);

            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].active) DrawCircleV(enemies[i].position, 20, RED);
            }

            DrawText("HP:", 10, 10, 24, DARKGRAY);
            for (int i = 0; i < maxHP; i++) {
                Color hpColor = (i < playerHP) ? RED : LIGHTGRAY;
                DrawCircle(70 + i * 35, 22, 12, hpColor);
            }
            DrawText(TextFormat("Score: %d", score), 10, 45, 24, DARKGRAY);
        }

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
    UnloadTexture(warriorRunTex);
    UnloadTexture(warriorAttackTex);
    CloseWindow();
    return 0;
}