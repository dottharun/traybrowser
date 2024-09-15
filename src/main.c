#include <raylib.h>

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);

    const int factor = 80;
    InitWindow(factor * 16, factor * 9, "TRAYBROWSER");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Hello world", 100, 100, 30, DARKBLUE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
