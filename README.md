# Wirtualny Zegar z Wahadłem

Stworzono wirtualny zegar z wahadłem, który zwizualizowano przy użyciu technologii OpenGL. W projekcie zastosowano bibliotekę [raylib](https://www.raylib.com). 

Zegar jest klasycznym zegarem analogowym z okrągłą tarczą oraz trzema wskazówkami: sekundową, minutową i godzinową. Wskazówka godzinowa wykonuje obrót w trakcie 12 godzin. Pod tarczą umieszczono wahadło – obecnie nieruchome. 

Dźwięk tykania odtwarzany jest w chwili zmiany sekundy. W tym celu czas między kolejnymi iteracjami pętli głównej jest sumowany i porównywany do 1, co pozwala synchronizować tyk zegara z rzeczywistym czasem.

Do projektu użyto biblioteki ctime, aby do czasu obliczanego przez program dodawać czas systemowy od momentu uruchomienia, dzięki czemu zegar wskazuje aktualny czas.

### Sterowanie:
- Włączanie/wyłączanie źródeł światła.
- Przyspieszanie/spowalnianie upływu czasu.
- Zmiana kolorów zegara i podłogi.
- Reset widoku kamery do ustawień początkowych.
- Ustawianie czasu wskazywanego przez zegar (zmiana trybu ustawiania czasu, regulacja godzin, minut i sekund).

Pozostałe elementy, takie jak obrót wskazówek, ruch wahadła oraz animacja zębatek mechanizmu zegara, zostaną dodane w kolejnych etapach. Dodatkowo planowana jest interaktywna funkcjonalność umożliwiająca użytkownikowi wybór godziny wskazywanej przez zegar lub ustawienie aktualnego czasu.
