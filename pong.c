#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define PADDLE_HEIGHT 100
#define PADDLE_WIDTH 20
#define BALL_SIZE 20

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
    Rectangle bounds;
} Ball;

static Texture2D texLogo;

static Font font;

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

    InitGame();

    while (!WindowShouldClose())
    {
        UpdateGame();
        BeginDrawing();
            DrawGame();
        EndDrawing();
    }

    DeInitGame();

    CloseWindow();

    return 0;
}

static void InitGame(void) {
    #ifndef RESOURCES
    #define RESOURCES
        texLogo = LoadTexture("resources/raylib_logo.png");
        font = LoadFont("resources/setback.png");
    #endif

    screen = LOGO;
    framesCounter = 0;

    player.position = (Vector2){SCREEN_WIDTH - PADDLE_WIDTH * 2, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2};
    player.speed = (Vector2){0.0f, 8.0f};
    player.bounds = (Rectangle){player.position.x, player.position.y, PADDLE_WIDTH, PADDLE_HEIGHT};
    player.score = 0;

    cpu.position = (Vector2){PADDLE_WIDTH, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2};
    cpu.speed = (Vector2){0.0f, 8.0f};
    cpu.bounds = (Rectangle){cpu.position.x, cpu.position.y, PADDLE_WIDTH, PADDLE_HEIGHT};
    cpu.score = 0;

    ball.position = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    ball.speed = (Vector2){3.0f, 3.0f};
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
            if (IsKeyPressed(KEY_ENTER)){
                screen = TITLE;
            }
        } break;
        case ENDING: {
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
        case ENDING: break;
        default: break;
    }
}

static void DeInitGame(void) {
    UnloadTexture(texLogo);

    UnloadFont(font);
}