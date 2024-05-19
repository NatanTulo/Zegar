#include "raylib.h"

int main()
{
    // Inicjalizacja
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60); // Ustawienie, aby gra działała w 60 klatkach na sekundę

    // Główna pętla gry
    while (!WindowShouldClose()) // Wykrywanie zamknięcia okna lub naciśnięcia klawisza ESC
    {
        // Rozpoczęcie rysowania
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Gratulacje! Utworzyłeś swoje pierwsze okno!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    // De-inicjalizacja
    CloseWindow(); // Zamknięcie okna i kontekstu OpenGL

    return 0;
}