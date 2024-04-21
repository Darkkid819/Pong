#include "raylib.h"
#include "raymath.h"
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define PADDLE_HEIGHT 100
#define PADDLE_WIDTH 20
#define BALL_SIZE 20
#define WIN_SCORE 10

typedef enum GameScreen {
    LOGO,
    TITLE,
    GAMEPLAY,
    ENDING
} GameScreen;

typedef struct Player {
    Vector2 position;
    Vector2 speed;
    Rectangle bounds;
    int score;
} Player;

typedef struct Ball {
    Vector2 position;
    Vector2 speed;
    float speedCoefficient;
    Rectangle bounds;
} Ball;

static Texture2D texLogo;

static Font font;

static Sound fxHit;
static Sound fxWall;
static Sound fxScore;

static Player player, cpu;
static Ball ball;
static GameScreen screen;
static int framesCounter;

static void InitGame(void);
static void UpdateGame(void);
static void DrawGame(void);
static void DeInitGame(void);

int main(void)
{

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PONG");
    SetTargetFPS(60);

    InitAudioDevice();
    InitGame();

    while (!WindowShouldClose())
    {
        UpdateGame();
        BeginDrawing();
            DrawGame();
        EndDrawing();
    }

    DeInitGame();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

static void InitGame(void) {
    #ifndef RESOURCES
    #define RESOURCES
        texLogo = LoadTexture("resources/raylib_logo.png");
        font = LoadFont("resources/setback.png");
        fxHit = LoadSound("resources/hit.wav");
        fxWall = LoadSound("resources/wall.wav");
        fxScore = LoadSound("resources/score.wav");
    #endif

    screen = LOGO;
    framesCounter = 0;

    player.position = (Vector2){SCREEN_WIDTH - PADDLE_WIDTH * 2, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2};
    player.speed = (Vector2){0.0f, 400.0f};
    player.bounds = (Rectangle){player.position.x, player.position.y, PADDLE_WIDTH, PADDLE_HEIGHT};
    player.score = 0;

    cpu.position = (Vector2){PADDLE_WIDTH, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2};
    cpu.speed = (Vector2){0.0f, 400.0f};
    cpu.bounds = (Rectangle){cpu.position.x, cpu.position.y, PADDLE_WIDTH, PADDLE_HEIGHT};
    cpu.score = 0;

    ball.position = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    ball.speed = (Vector2){200.0f, 0.0f};
    ball.speedCoefficient = 1.0f;
    ball.bounds = (Rectangle){ball.position.x - BALL_SIZE / 2, ball.position.y - BALL_SIZE / 2, BALL_SIZE, BALL_SIZE};
}

static void UpdateGame(void) {
    switch(screen) {
        case LOGO: {
            framesCounter++;

            if (framesCounter > 180) {
                screen = TITLE;
                framesCounter = 0;
            }
        } break;
        case TITLE: {
            framesCounter++;

            if (IsKeyPressed(KEY_ENTER)){
                screen = GAMEPLAY;
            }
        } break;
        case GAMEPLAY: {
            if (IsKeyDown(KEY_UP)) {
                player.position.y -= player.speed.y * GetFrameTime();
            }
            if (IsKeyDown(KEY_DOWN)) {
                player.position.y += player.speed.y * GetFrameTime();
            }
            player.position.y = Clamp(player.position.y, 0, SCREEN_HEIGHT - PADDLE_HEIGHT);
            player.bounds.y = player.position.y;

            float centerCpuPaddle = cpu.position.y + PADDLE_HEIGHT / 2;
            float centerBall = ball.position.y + BALL_SIZE / 2;
            float distance = centerBall - centerCpuPaddle;
            float cpuErrorFactor = (GetFrameTime() > 0.1) ? GetRandomValue(-20, 20) : 0;
            cpu.position.y += (cpu.speed.y * 0.5f + cpuErrorFactor) * GetFrameTime() * (distance > 0 ? 1 : -1);
            cpu.position.y = Clamp(cpu.position.y, 0, SCREEN_HEIGHT - PADDLE_HEIGHT);
            cpu.bounds.y = cpu.position.y;

            ball.position.x += ball.speed.x * GetFrameTime();
            ball.position.y += ball.speed.y * GetFrameTime();

            if (ball.position.x + BALL_SIZE/2 >= SCREEN_WIDTH) {
                cpu.score++;  
                ball.position = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
                ball.speed = (Vector2){200.0f, 0.0f};
                ball.speedCoefficient = 1.0f; 
                player.position.y = cpu.position.y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
                player.bounds.y = player.position.y;
                cpu.bounds.y = cpu.position.y;
                
                PlaySound(fxScore);
            } else if (ball.position.x - BALL_SIZE/2 <= 0) {
                player.score++;
                ball.position = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
                ball.speed = (Vector2){-200.0f, 0.0f};
                ball.speedCoefficient = 1.0f;
                player.position.y = cpu.position.y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
                player.bounds.y = player.position.y;
                cpu.bounds.y = cpu.position.y;
                
                PlaySound(fxScore);
            }

            if (ball.position.y + BALL_SIZE >= SCREEN_HEIGHT || ball.position.y - BALL_SIZE <= 0) {
                ball.speed.y *= -1;

                PlaySound(fxWall);
            }
            ball.bounds.x = ball.position.x;
            ball.bounds.y = ball.position.y;

            if (CheckCollisionRecs(player.bounds, ball.bounds) || CheckCollisionRecs(cpu.bounds, ball.bounds)) {
                if (ball.speedCoefficient < 1.5f) {
                    ball.speedCoefficient += 0.1f; 
                }
                ball.speed.x = -ball.speed.x * ball.speedCoefficient;
                ball.speed.x = Clamp(ball.speed.x, -400.0f, 400.0f);

                float paddleY = CheckCollisionRecs(player.bounds, ball.bounds) ? player.position.y : cpu.position.y;
                float impact = ball.position.y - (paddleY + PADDLE_HEIGHT / 2);
                float normalizedImpact = impact / (PADDLE_HEIGHT / 2);
                ball.speed.y = normalizedImpact * 400.0f; 
                ball.speed.y = Clamp(ball.speed.y, -400.0f, 400.0f); 

                PlaySound(fxHit);
            }

            if (player.score >= WIN_SCORE || cpu.score >= WIN_SCORE){
                screen = ENDING;
            }
        } break;
        case ENDING: {
            framesCounter++;

            if(IsKeyPressed(KEY_ENTER)) {
                InitGame();
                screen = TITLE;
            }
        } break;
        default: break;
    }
}

static void DrawGame(void) {
    ClearBackground(RAYWHITE);

    switch (screen) {
        case LOGO: {
            DrawTexture(texLogo, SCREEN_WIDTH / 2 - texLogo.width / 2, SCREEN_HEIGHT / 2 - texLogo.height / 2, WHITE);
        } break;
        case TITLE: {
            DrawTextEx(font, "PONG", (Vector2){SCREEN_WIDTH / 2 - MeasureText("PONG", 80) / 2, 80}, 80, 10, MAROON);
            
            if((framesCounter / 30) % 2 == 0) {
                DrawText("PRESS [ENTER] TO START", 
                    SCREEN_WIDTH / 2 - MeasureText("PRESS [ENTER] TO START", 20) / 2,
                    SCREEN_HEIGHT / 2 + 60,
                    20,
                    DARKGRAY);
            }
        } break;
        case GAMEPLAY: {
            ClearBackground(BLACK);

            DrawText(TextFormat("%d", player.score), SCREEN_WIDTH * 2 / 3, 20, 60, WHITE);
            DrawText(TextFormat("%d", cpu.score), SCREEN_WIDTH / 3 - BALL_SIZE, 20, 60, WHITE);

            for (int y = 0; y < SCREEN_HEIGHT; y += 20) {
                DrawRectangle(SCREEN_WIDTH / 2 - BALL_SIZE / 4, y, BALL_SIZE / 2, 10, WHITE);
            }

            DrawRectangle(player.bounds.x, player.bounds.y, player.bounds.width, player.bounds.height, WHITE);
            DrawRectangle(cpu.bounds.x, cpu.bounds.y, cpu.bounds.width, cpu.bounds.height, WHITE);
            DrawRectangle(ball.bounds.x, ball.bounds.y, ball.bounds.width, ball.bounds.height, WHITE);
        } break;
        case ENDING: {
            if (player.score >= WIN_SCORE) {
                DrawTextEx(font, "PLAYER WIN", (Vector2){SCREEN_WIDTH / 2 - MeasureText("PLAYER WIN", 80) / 2, 100}, 80, 6, MAROON);
            } else {
                DrawTextEx(font, "CPU WIN", (Vector2){SCREEN_WIDTH / 2 - MeasureText("CPU WIN", 80) / 2, 100}, 80, 6, MAROON);
            }
                        
            if ((framesCounter) % 2 == 0) {
                DrawText("PRESS [ENTER] TO PLAY AGAIN", 
                    GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
                    GetScreenHeight() / 2 + 80,
                    20,
                    GRAY);
            }
        } break;
        default: break;
    }
}

static void DeInitGame(void) {
    UnloadTexture(texLogo);

    UnloadFont(font);

    UnloadSound(fxHit);
    UnloadSound(fxScore);
    UnloadSound(fxWall);
}