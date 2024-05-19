#include "raylib.h"
#include <string>
#include <cstring>
#include <cstdlib>

int main(void)
{
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Zegar");
    InitAudioDevice();
    SetTargetFPS(60);

    Sound tick = LoadSound("assets/clock.wav");
    double tickTime = 0;
	double time = 0;

    while (!WindowShouldClose())
    {
        if (GetTime() - tickTime >= 1) {
			tickTime = GetTime();
            PlaySound(tick);
        }

        time = GetTime();
        std::string temp = "Aktualny czas: " + std::to_string(time);
        char* timeStr= (char*)malloc(temp.length() + 16);
        if (timeStr!= nullptr) strcpy(timeStr, temp.c_str());

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText(timeStr, 20, 20, 20, LIGHTGRAY);
        free(timeStr);
        EndDrawing();
    }

	// Deinicjalizacja
    UnloadSound(tick);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}