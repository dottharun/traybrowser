#include "file.c"
#include "html_tokenizer.c"

#include <raylib.h>

int main() {
    // opening html file
    // must be freed after use
    const char* path         = "./res/hello.html";
    char*       file_content = read_file_to_string(path);

    printf("file:\n%s", file_content);

    // tokenizer testing
    struct tzr_tokenizer_data tokenizer =
        tzr_tokenizer_data_create(file_content);
    printf("going to run tokenizer\n");

    tzr_run(&tokenizer);

    // rl window init
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
    const int factor = 80;
    InitWindow(factor * 16, factor * 9, "TRAYBROWSER");
    SetTargetFPS(30);

    // font init
    const char* font_path =
        "./res/ComicShannsMono/ComicShannsMonoNerdFont-Regular.otf";
    Font    scp_font     = LoadFontEx(font_path, 96, 0, 0);
    Vector2 fontPosition = {100.0f, 20.0f};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        // draw something
        DrawText("website: ", 500, 5, 30, WHITE);
        DrawTextEx(scp_font, file_content, fontPosition, 28, 1, WHITE);

        EndDrawing();
    }

    CloseWindow();

    free(file_content);
    return 0;
}
