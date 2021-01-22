#include <raylib.h>
#include <raymath.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_FPS 60

#define BACKGROUND_COLOR BLACK

#define PROJS_CAP 1000
#define PROJ_WIDTH 0.1f
#define PROJ_HEIGHT 0.1f
#define PROJ_LEN 0.1f
#define PROJ_COLOR WHITE
#define PROJ_VEL 5.0f
#define PROJ_LIFETIME 5.0f

#define PLAYER_GUN_LEN 0.5f
#define PLAYER_HEIGHT 1.0f

Color colors[] = {
    YELLOW,
    GOLD,
    ORANGE,
    PINK,
    RED,
    MAROON,
    GREEN,
    LIME,
    DARKGREEN,
    SKYBLUE,
    BLUE,
    DARKBLUE,
    PURPLE,
    VIOLET,
    DARKPURPLE,
    BEIGE,
    BROWN,
    DARKBROWN,
};

Color random_color(void)
{
    return colors[rand() % (sizeof(colors) / sizeof(colors[0]))];
}

typedef float Seconds;

typedef struct {
    Seconds lifetime;
    Vector3 pos;
    Vector3 dir;
} Proj;

Proj projs[PROJS_CAP] = {0};

void draw_projs(void)
{
    for (size_t i = 0; i < PROJS_CAP; ++i) {
        if (projs[i].lifetime > 0.0f) {
            DrawCube(projs[i].pos,
                     PROJ_WIDTH,
                     PROJ_HEIGHT,
                     PROJ_LEN,
                     PROJ_COLOR);
        }
    }
}

void update_projs(void)
{
    const Seconds dt = GetFrameTime();
    for (size_t i = 0; i < PROJS_CAP; ++i) {
        if (projs[i].lifetime > 0.0f) {
            projs[i].lifetime -= dt;
            projs[i].pos = Vector3Add(
                projs[i].pos,
                Vector3Scale(projs[i].dir, dt));
        }
    }
}

void spawn_proj(Vector3 pos, Vector3 dir)
{
    for (size_t i = 0; i < PROJS_CAP; ++i) {
        if (projs[i].lifetime <= 0.0f) {
            projs[i].lifetime = PROJ_LIFETIME;
            projs[i].pos = pos;
            projs[i].dir = dir;
            break;
        }
    }
}

typedef struct {
    Vector2 position;
    float height;
    Color color;
} Pillar;

#define PILLARS_GRID_WIDTH 3
#define PILLARS_GRID_HEIGHT 23
#define PILLARS_GRID_SPACING_X 2.0f
#define PILLARS_GRID_SPACING_Y 2.0f
#define PILLARS_CAP (PILLARS_GRID_WIDTH * PILLARS_GRID_HEIGHT)
#define PILLAR_COLOR LIME
#define PILLAR_SIZE_X 1.0f
#define PILLAR_SIZE_Z 1.0f

Pillar pillars[PILLARS_CAP] = {0};

void draw_pillars(void)
{
    for (size_t i = 0; i < PILLARS_CAP; ++i) {
        DrawCubeV(
            (Vector3) {
                pillars[i].position.x,
                pillars[i].height / 2.0,
                pillars[i].position.y
            },
            (Vector3) {
                PILLAR_SIZE_X,
                pillars[i].height,
                PILLAR_SIZE_Z
            },
            pillars[i].color);
    }
}

void init_pillars(void)
{
    for (size_t x = 0; x < PILLARS_GRID_WIDTH; ++x) {
        for (size_t y = 0; y < PILLARS_GRID_HEIGHT; ++y) {
            size_t i = y * PILLARS_GRID_WIDTH + x;
            pillars[i].height = 5.0f;
            pillars[i].color = random_color();
            pillars[i].position = (Vector2) {
                x * PILLARS_GRID_SPACING_X,
                y * PILLARS_GRID_SPACING_Y
            };
        }
    }
}

Vector3 camera_direction(Camera *camera)
{
    return Vector3Normalize(Vector3Subtract(camera->target, camera->position));
}

#define V3_Fmt "{%f, %f, %f}"
#define V3_Arg(v3) v3.x, v3.y, v3.z

char hud_buffer[1024];


int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "UwU");

    Shader shader = LoadShader(0, TextFormat("./light.fs", 330));

    Camera camera = {0};
    camera.position = (Vector3) {1.0f, PLAYER_HEIGHT, 1.0f};
    camera.target = (Vector3) {0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.type = CAMERA_PERSPECTIVE;

    Model model = LoadModel("guy/guy.iqm");
    Texture2D texture = LoadTexture("./guy/guytex.png");
    SetMaterialTexture(&model.materials[0], MAP_DIFFUSE, texture);

    SetCameraMode(camera, CAMERA_FIRST_PERSON);

    SetTargetFPS(SCREEN_FPS);

    init_pillars();

    while (!WindowShouldClose()) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector3 dir = camera_direction(&camera);
            spawn_proj(
                Vector3Add(camera.position, Vector3Scale(dir, PLAYER_GUN_LEN)),
                Vector3Scale(dir, PROJ_VEL));
        }

        update_projs();
        UpdateCamera(&camera);

        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            camera.target = (Vector3) {0.0f, 0.0f, 0.0f};
        }

        BeginDrawing();
        {
            ClearBackground(BACKGROUND_COLOR);
            BeginShaderMode(shader);
            {
                BeginMode3D(camera);
                {
                    // DrawModelEx(model, (Vector3){0.0f, 0.0f, 0.0f}, (Vector3){ 1.0f, 0.0f, 0.0f }, -90.0f, (Vector3){ 1.0f, 1.0f, 1.0f }, WHITE);
                    draw_projs();
                    draw_pillars();
                    DrawGrid(10, 1.0f);         // Draw a grid
                }
                EndMode3D();
            }
            EndShaderMode();

            snprintf(hud_buffer, sizeof(hud_buffer),
                     "Target: "V3_Fmt, V3_Arg(camera.target));
            DrawText(hud_buffer, 10, 10, 20, MAROON);
        }
        EndDrawing();
    }

    return 0;
}
