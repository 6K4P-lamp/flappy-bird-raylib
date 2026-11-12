// Compile: clang -lraylib main.c -o flappy_bird

#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define BIRD_X 180
#define BIRD_RADIUS 16

#define PIPE_WIDTH 70
#define PIPE_GAP 170
#define PIPE_SPEED 220
#define PIPE_COUNT 3

typedef enum {
    MENU,
    PLAYING,
    GAME_OVER
} GameState;

typedef struct {
    float y;
    float velocity;
} Bird;

typedef struct {
    float x;
    float gapY;
    bool scored;
} Pipe;

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy Bird - raylib");
    SetTargetFPS(60);

    srand((unsigned int)time(NULL));

    GameState state = MENU;

    Bird bird = { SCREEN_HEIGHT / 2.0f, 0 };

    Pipe pipes[PIPE_COUNT];

    int score = 0;
    int bestScore = 0;

    const float gravity = 900.0f;
    const float jumpForce = -350.0f;

    for (int i = 0; i < PIPE_COUNT; i++) {
        pipes[i].x = SCREEN_WIDTH + i * 300;
        pipes[i].gapY = 200 + rand() % 200;
        pipes[i].scored = false;
    }

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Input
        if (state == MENU) {
            if (IsKeyPressed(KEY_SPACE) ||
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                state = PLAYING;
                bird.y = SCREEN_HEIGHT / 2.0f;
                bird.velocity = jumpForce;
            }
        }

        else if (state == PLAYING) {

            if (IsKeyPressed(KEY_SPACE) ||
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                bird.velocity = jumpForce;
            }

            // Bird physics
            bird.velocity += gravity * dt;
            bird.y += bird.velocity * dt;

            // Screen collision
            if (bird.y - BIRD_RADIUS < 0 ||
                bird.y + BIRD_RADIUS > SCREEN_HEIGHT) {
                state = GAME_OVER;
            }

            // Pipe movement
            for (int i = 0; i < PIPE_COUNT; i++) {

                pipes[i].x -= PIPE_SPEED * dt;

                // Pipe collision
                Rectangle topPipe = {
                    pipes[i].x,
                    0,
                    PIPE_WIDTH,
                    pipes[i].gapY - PIPE_GAP / 2
                };

                Rectangle bottomPipe = {
                    pipes[i].x,
                    pipes[i].gapY + PIPE_GAP / 2,
                    PIPE_WIDTH,
                    SCREEN_HEIGHT
                };

                Rectangle birdRect = {
                    BIRD_X - BIRD_RADIUS,
                    bird.y - BIRD_RADIUS,
                    BIRD_RADIUS * 2,
                    BIRD_RADIUS * 2
                };

                if (CheckCollisionCircleRec(
                    (Vector2){BIRD_X, bird.y},
                    BIRD_RADIUS,
                    topPipe) ||
                    CheckCollisionCircleRec(
                    (Vector2){BIRD_X, bird.y},
                    BIRD_RADIUS,
                    bottomPipe)) {
                    state = GAME_OVER;
                }

                // Score
                if (!pipes[i].scored &&
                    pipes[i].x + PIPE_WIDTH < BIRD_X) {
                    score++;
                    pipes[i].scored = true;
                }

                // Reset pipe
                if (pipes[i].x + PIPE_WIDTH < 0) {
                    pipes[i].x = SCREEN_WIDTH + 100;
                    pipes[i].gapY = 180 + rand() % 240;
                    pipes[i].scored = false;
                }
            }

            if (score > bestScore)
                bestScore = score;
        }

        else if (state == GAME_OVER) {
            if (IsKeyPressed(KEY_SPACE) ||
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

                score = 0;

                bird.y = SCREEN_HEIGHT / 2.0f;
                bird.velocity = 0;

                for (int i = 0; i < PIPE_COUNT; i++) {
                    pipes[i].x = SCREEN_WIDTH + i * 300;
                    pipes[i].gapY = 200 + rand() % 200;
                    pipes[i].scored = false;
                }

                state = PLAYING;
            }
        }

        // Drawing
        BeginDrawing();

        ClearBackground(SKYBLUE);

        // Background
        DrawRectangle(0, 500, SCREEN_WIDTH, 100, BEIGE);
        DrawLine(0, 500, SCREEN_WIDTH, 500, DARKGREEN);

        // Pipes
        for (int i = 0; i < PIPE_COUNT; i++) {

            int topHeight = (int)(pipes[i].gapY - PIPE_GAP / 2);
            int bottomY = (int)(pipes[i].gapY + PIPE_GAP / 2);

            DrawRectangle(
                (int)pipes[i].x,
                0,
                PIPE_WIDTH,
                topHeight,
                GREEN
            );

            DrawRectangle(
                (int)pipes[i].x - 5,
                topHeight - 20,
                PIPE_WIDTH + 10,
                20,
                DARKGREEN
            );

            DrawRectangle(
                (int)pipes[i].x,
                bottomY,
                PIPE_WIDTH,
                SCREEN_HEIGHT - bottomY,
                GREEN
            );

            DrawRectangle(
                (int)pipes[i].x - 5,
                bottomY,
                PIPE_WIDTH + 10,
                20,
                DARKGREEN
            );
        }

        // Bird
        DrawCircle(BIRD_X, (int)bird.y, BIRD_RADIUS, YELLOW);
        DrawCircle(BIRD_X + 6, (int)bird.y - 5, 4, WHITE);
        DrawCircle(BIRD_X + 7, (int)bird.y - 5, 2, BLACK);
        DrawEllipse(BIRD_X - 7, (int)bird.y + 5, 9, 5, ORANGE);

        // Score
        if (state == PLAYING || state == GAME_OVER) {
            char scoreText[32];
            sprintf(scoreText, "%d", score);

            DrawText(scoreText, SCREEN_WIDTH / 2 - 15, 40,
                     40, WHITE);
        }

        // Menu
        if (state == MENU) {
            DrawRectangle(180, 180, 440, 200, Fade(BLACK, 0.5f));

            DrawText("FLAPPY BIRD", 255, 210, 40, YELLOW);
            DrawText("Press SPACE or CLICK to fly",
                     220, 280, 20, WHITE);
            DrawText("Avoid the pipes!",
                     300, 320, 20, WHITE);
        }

        // Game over
        if (state == GAME_OVER) {
            DrawRectangle(180, 180, 440, 220, Fade(BLACK, 0.7f));

            DrawText("GAME OVER", 280, 205, 40, RED);

            char scoreText[64];
            sprintf(scoreText, "Score: %d", score);
            DrawText(scoreText, 320, 270, 25, WHITE);

            char bestText[64];
            sprintf(bestText, "Best: %d", bestScore);
            DrawText(bestText, 320, 305, 25, YELLOW);

            DrawText("Press SPACE to restart",
                     250, 355, 20, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
