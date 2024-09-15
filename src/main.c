#include "file.c"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    // opening html file
    const char* path         = "./res/cern.html";
    char*       file_content = read_file_to_string(path);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);

    const int factor = 80;
    InitWindow(factor * 16, factor * 9, "TRAYBROWSER");

    // font init
    Font scp_font = LoadFontEx(
        "./res/ComicShannsMono/ComicShannsMonoNerdFont-Regular.otf",
        96,
        0,
        0
    );
    Vector2 fontPosition = {100.0f, 20.0f};

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText("website", 500, 5, 30, WHITE);
        DrawTextEx(scp_font, file_content, fontPosition, 28, 1, WHITE);

        EndDrawing();
    }

    CloseWindow();

    // freeing file_content
    if (file_content) {
        printf("File content:\n%s\n", file_content);
        free(file_content); // Remember to free the allocated memory
    }

    return 0;
}
