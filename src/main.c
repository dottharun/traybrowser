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

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText(file_content, 100, 100, 30, WHITE);

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
