# Projekt zaliczeniowy – Języki Obiektowe (Gra platformowa 2D w C++)

## 1. Opis architektury i funkcjonalności

Program realizuje dwuwymiarową grę platformową w czystym C++ z interfejsem graficznym opartym o **WinAPI / GDI** (podwójne buforowanie przez `BitBlt`) oraz diagnostycznym interfejsem konsolowym. Projekt został podzielony na moduły nagłówkowe (`.h`) oraz implementacyjne (`.cpp`):

* `grafika.h` / `grafika.cpp` – hierarchia platform, zarządzanie zasobami graficznymi, renderowanie i przesuwanie mapy.
* `ruch.h` / `ruch.cpp` – logika gracza, fizyka, grawitacja, kolizje AABB oraz system żyć.
* `punkty.h` / `punkty.cpp` – zliczanie unikalnych odwiedzonych platform.
* `main.cpp` – inicjalizacja okna WinAPI, główna pętla gry, obsługa wejścia oraz wyłapywanie wyjątków.

---

### Moduł `grafika.h` / `grafika.cpp`

#### Dołączone biblioteki (`#include`)
* `<windows.h>` – deklaracje funkcji API systemu Windows (WinAPI/GDI), struktur danych i stałych.
* `<vector>` – kontener `std::vector` do przechowywania wskaźników na platformy oraz szablonów graficznych.
* `<string>` – obsługa napisów (nazwy platform, komunikaty interfejsu).
* `<iostream>` – wypisywanie logów diagnostycznych w konsoli.
* `<stdexcept>` – standardowe klasy wyjątków (`std::invalid_argument`, `std::out_of_range`, `std::runtime_error`).

#### Struktura `Szab_graf`
Przechowuje szablon graficzny platformy:
* `HBITMAP bitmap` – uchwyt do wczytanej bitmapy `.bmp`.
* `float width`, `float height` – wymiary platformy (przyrostek `.f` oznacza literał typu `float` bez niejawnej konwersji z `double`).
* `COLORREF kol_zap` – zapasowy kolor wypełnienia, gdy bitmapa nie zostanie wczytana.

#### Klasa bazowa `Platforma`
* **Konstruktory i destruktor:**
  * `Platforma()` – konstruktor domyślny.
  * `Platforma(const std::string& nazwa, float x, float y, float width, float height, HBITMAP bmp, COLORREF kolor)` – konstruktor parametryczny z walidacją wymiarów.
  * `virtual ~Platforma()` – destruktor wirtualny, zapewniający poprawne zwalnianie pamięci obiektów klas pochodnych przez wskaźnik do klasy bazowej.
* **Metody dostępowe (gettery `const`):** `getNazwa()`, `getX()`, `getY()`, `getWidth()`, `getHeight()`.
* **Metody ustawiające (settery z walidacją):** `setNazwa()`, `setPozycja()`, `setWymiary()` – rzucają wyjątki przy niepoprawnych danych.
* **Przesuwanie kamery:** `przesunY(float dy)` – przesuwa platformę w dół podczas ruchu mapy w górę.
* **Metody wirtualne (polimorfizm):**
  * `virtual void rysuj(HDC memDC) const` – rysuje platformę na kontekście urządzenia w pamięci (`memDC`).
  * `virtual void aktualizuj(float dt)` – aktualizuje stan platformy na podstawie czasu klatki `dt` (domyślnie bez ruchu).
  * `virtual float pob_mno_Skok() const` – zwraca mnożnik siły skoku (domyślnie `1.0f`).
  * `virtual std::string pob_typ() const` – zwraca nazwę typu platformy.
* **Przeciążone operatory:**
  * `bool operator==(const Platforma& inne) const` – porównuje platformy po nazwie lub współrzędnych `(x, y)` (zapobiega duplikatom).
  * `bool operator<(const Platforma& inne) const` – porównuje platformy po współrzędnej `y` (używane do wyznaczania najwyższej platformy na mapie).
  * `friend std::ostream& operator<<(std::ostream& os, const Platforma& p)` – wypisuje pełny opis obiektu do strumienia wyjściowego.

#### Klasy pochodne
* **`Plat_zwykla`** (dziedziczy publicznie po `Platforma`) – statyczna platforma ze standardowym mnożnikiem skoku (`1.0f`). Przesłania (`override`) metody `rysuj()`, `pob_mno_Skok()` oraz `pob_typ()`.
* **`Plat_ruch`** (dziedziczy publicznie po `Platforma`) – ruchoma platforma poruszająca się poziomo w zakresie `[minX, maxX]` z prędkością `pred` oraz zwiększonym mnożnikiem skoku (`mno_skok`, domyślnie `1.35f`). Przesłania (`override`) `rysuj()` (dodaje złoty pasek wyróżniający), `aktualizuj()`, `pob_mno_Skok()` i `pob_typ()`.

#### Klasa zarządzająca `Main_graf`
Zarządza kontenerem polimorficznym `std::vector<Platforma*> platformy`, ładowaniem zasobów (`tlo.bmp`, `tlo2.bmp`, `postac.bmp`, `plat_1.bmp`–`plat_3.bmp`) oraz renderowaniem.
* **Operacje CRUD na obiektach:**
  * `dodaj_Plat(Platforma* plat)` – dodaje platformę (sprawdza duplikaty przez `operator==`).
  * `us_ost_Plat()` – usuwa ostatnią platformę (rzuca `std::out_of_range` przy próbie usunięcia platformy startowej).
  * `znajdz_Plat_nazw(const std::string& nazwa) const` – wyszukuje platformę po nazwie (rzuca `std::runtime_error`, gdy jej nie ma).
  * `mod_Plat(const std::string& nazwa, float x, float y)` – wyszukuje i przesuwa wybraną platformę.
* **Logika poziomu i renderowania:**
  * `gen_Poz()` – generuje początkowy układ platform i wyszukuje najwyższą z nich za pomocą `std::min_element` i `operator<`.
  * `przesun_Mape(float dy)` – przesuwa platformy w dół, usuwa te poza ekranem, generuje nowe u góry i płynnie przechodzi z `tlo.bmp` na `tlo2.bmp`.
  * `aktualizuj(float dt)` – polimorficznie aktualizuje wszystkie platformy.
  * `rysuj(...) const` – rysuje tło, platformy, postać gracza oraz interfejs (życia i punkty) z wykorzystaniem podwójnego buforowania.

---

### Moduł `ruch.h` / `ruch.cpp`

Zawiera klasę **`Gracz`**, odpowiadającą za fizykę i stan postaci:
* **Pola prywatne:** pozycja (`x`, `y`), wymiary (`width`, `height`), prędkość pozioma (`pred_ruch`) i pionowa (`pred_Y`), grawitacja, siła skoku, aktualny mnożnik skoku (`akt_mno_skok`), flaga `na_ziemi` oraz liczba żyć (`hp`).
* **Metody:**
  * `przecina(const Platforma& plat) const` – detekcja kolizji prostokątów (AABB).
  * `ustaw_na_start(const std::vector<Platforma*>& platformy)` – ustawia postać na środku pierwszej platformy.
  * `aktualizuj(float dt, const std::vector<Platforma*>& platformy, int wys_okna, Punkty& punkty)` – obsługuje ruch (`A`/`D`), skok (`W`/`Spacja`) z polimorficznym mnożnikiem `plat->pob_mno_Skok()`, grawitację, zaliczanie punktów oraz utratę życia po upadku poza mapę.
* **Przeciążone operatory:** `operator-=` (utrata żyć), `operator+=` (dodanie żyć) oraz `operator<<` (logowanie stanu gracza).

---

### Moduł `punkty.h` / `punkty.cpp`

Zawiera klasę **`Punkty`**, odpowiedzialną za system punktacji:
* Przechowuje aktualny `wynik` oraz zbiór `std::set<std::string> odwiedzone`, gwarantujący, że każda platforma może zostać zaliczona tylko raz.
* Metody: `getWynik()`, `oznacz_start()`, `zalicz_Plat()`, `reset()` oraz przeciążony `operator<<`.

---

### Moduł `main.cpp`

* Tworzy i rejestruje okno WinAPI (`WNDCLASSA`, `CreateWindowExA`) oraz obsługuje komunikaty systemowe (`WindowProc`).
* Uruchamia główną pętlę gry (~60 FPS), obsługuje przesuwanie kamery po przekroczeniu połowy wysokości ekranu oraz wyłapuje wyjątki w blokach `try-catch`.

---

## 2. Zastosowane mechanizmy programowania obiektowego

* **Hermetyzacja:** pola klas są `private` lub `protected`, dostęp odbywa się przez gettery `const` oraz settery z walidacją.
* **Dziedziczenie i polimorfizm:** hierarchia `Platforma` -> `Plat_zwykla`, `Plat_ruch`; przechowywanie wskaźników bazowych w `std::vector<Platforma*>` i wywoływanie metod wirtualnych (`rysuj`, `aktualizuj`, `pob_mno_Skok`, `pob_typ`).
* **Przeciążanie operatorów:** `==`, `<`, `<<` (w `Platforma`), `-=`, `+=`, `<<` (w `Gracz`), `<<` (w `Punkty`).
* **Obsługa wyjątków:** rzucanie i obsługa `std::invalid_argument`, `std::out_of_range` oraz `std::runtime_error`.

---

## 3. Kompilacja i sterowanie

### Kompilacja (z katalogu `projekt/gra`)
```powershell
g++ cialo/main.cpp cialo/grafika.cpp cialo/ruch.cpp cialo/punkty.cpp -o ../output/main.exe -lgdi32
../output/main.exe

Klawisz	Akcja
A / D	Ruch postaci w lewo / w prawo
W / Spacja	Skok
R	Reset poziomu i losowanie nowej mapy
1	Dodanie nowej platformy (operacja CRUD)
2	Usunięcie ostatniej platformy (operacja CRUD + test wyjątku)
3	Modyfikacja pozycji platformy Plat_2 (operacja CRUD)
```
---

## 4. Struktura katalogów projektu

```text
projekt/
├── output/
│   └── main.exe           # Skompilowany plik wykonywalny gry
└── gra/
    ├── README.md          # Dokumentacja projektu
    ├── cialo/             # Kod źródłowy C++
    │   ├── main.cpp       # Punkt wejścia, okno WinAPI, główna pętla gry
    │   ├── grafika.h      # Deklaracje klas: Platforma, Plat_zwykla, Plat_ruch, Main_graf
    │   ├── grafika.cpp    # Implementacja renderowania, CRUD i generowania mapy
    │   ├── ruch.h         # Deklaracja klasy Gracz
    │   ├── ruch.cpp       # Implementacja fizyki, kolizji AABB i systemu żyć
    │   ├── punkty.h       # Deklaracja klasy Punkty
    │   └── punkty.cpp     # Implementacja zliczania unikalnych platform
    └── obrazy/            # Zasoby graficzne (24-bitowe mapy bitowe .bmp)
        ├── tlo.bmp        # Tło początkowe
        ├── tlo2.bmp       # Tło docelowe po przesunięciu mapy
        ├── postac.bmp     # Grafika postaci gracza
        ├── plat_1.bmp     # Tekstura platformy (wariant 1)
        ├── plat_2.bmp     # Tekstura platformy (wariant 2)
        └── plat_3.bmp     # Tekstura platformy (wariant 3)
```

## 5. Diagram relacji między klasami

```text
                  ┌──────────────────────┐
                  │      Platforma       │ (klasa bazowa)
                  └──────────┬───────────┘
                             │ dziedziczenie publiczne
             ┌───────────────┴───────────────┐
             ▼                               ▼
  ┌─────────────────────┐         ┌─────────────────────┐
  │     Plat_zwykla     │         │      Plat_ruch      │
  └─────────────────────┘         └─────────────────────┘

  ┌─────────────────────┐  kompozycja (std::vector<Platforma*>)
  │      Main_graf      │ ───────────────────────────────────────► Platforma
  └─────────────────────┘  kompozycja (std::vector<Szab_graf>)
             │             ───────────────────────────────────────► Szab_graf
             │ używa w main.cpp
             ▼
  ┌─────────────────────┐  zależność (przekazywanie przez referencję)
  │        Gracz        │ ───────────────────────────────────────► Punkty
  └─────────────────────┘

```
## 6. Obsługa wyjątków  i walidacja danych

Wszystkie operacje podatne na błędne dane wejściowe lub naruszenie stanu aplikacji zostały zabezpieczone mechanizmem try-catch:

Typ wyjątku | Miejsce rzucenia | Warunek wywołania | Miejsce obsługi
std::invalid_argumentPlatforma::setNazwa(), setWymiary(), Plat_ruch::setMnoS(), Gracz::Gracz(), Gracz::setHp(), Main_graf::dodaj_Plat() | Pusta nazwa, niedodatnie wymiary/mnożnik skoku, ujemna liczba żyć lub pusty wskaźnik nullptr | main.cpp (główny blok try-catch oraz pętla CRUD)
std::out_of_rangePlatforma::setPozycja(), Main_graf::us_ost_Plat() | Próba ustawienia platformy poza granicami okna lub usunięcia ostatniej (startowej) platformy klawiszem 2 | main.cpp (pętla CRUD – komunikat w konsoli, gra działa dalej)
std::runtime_errorMain_graf::dodaj_Plat(), Main_graf::znajdz_Plat_nazw(), Gracz::ustaw_na_start() | Próba dodania duplikatu platformy (operator==), brak szukanej platformy (np. modyfikacja usuniętej Plat_2 klawiszem 3) lub pusta mapa | main.cpp / Main_graf::gen_Poz()

## 7. Zarządzanie pamięcią i zasobami systemowymi
Dynamiczne obiekty platform (Platforma*): wszystkie obiekty tworzone operatorem new w Main_graf są zwalniane przez delete w metodach czysc_Plat(), us_ost_Plat(), przesun_Mape() (dla platform poza ekranem) oraz w destruktorze ~Main_graf(). Wirtualny destruktor virtual ~Platforma() gwarantuje poprawne wywołanie destruktorów klas pochodnych.

Zasoby WinAPI / GDI: wszystkie uchwyty bitmap (HBITMAP), pędzle (HBRUSH) i konteksty pamięciowe (HDC) są zwalniane po użyciu za pomocą DeleteObject(), DeleteDC() oraz ReleaseDC(), co zapobiega wyciekom uchwytów GDI.

## 8. Wymagania środowiskowe
System operacyjny: Windows 10 / 11 (wymagane API Win32 / GDI).

Kompilator: MinGW / TDM-GCC (g++, standard C++11 lub nowszy).

Biblioteki zewnętrzne: brak – projekt korzysta wyłącznie ze standardowej biblioteki C++ oraz systemowej biblioteki gdi32 (linkowanej flagą -lgdi32).


## 9. Opis testów

### Pierwszy test:

*Uruchamianie za pomocą komendy:*
- "g++ cialo/main.cpp cialo/grafika.cpp cialo/ruch.cpp -o ../output/main.exe -lgdi32
../output/main.exe"
- Generowanie planszy działa tworzy się kilka platform, pierwsza platforma to platforma zwykła oraz mieszanka platform zwykłych i ruchomych.
- Model postaci się nie wczytuje tło się wczytuje. Generowane platformy mają różne wymiary, mnożnik skoków działa poprawnie. 
- System żyć działa system tworzenia kolejnych platform nie działa prawdopodbnie do usunięcia (1 - tworzy platforme, 2 - usuwa, 3 - podnosi pierwszą platformę). 
- Potenclajna zmiana systemu resetowania poziomu (Resetowanie będzie możliwe w każdym momencie a nie tylko w przypadku stracenia wszystkich punktów życia).

*Uruchamianie za pomocą pliku main.exe:*
- Generowanie platform działa właściwie, pierwsza platforma to platforma zwykła a dalsze to mieszanka zwykłych i ruchomych, model postaci i tło się nie wczytuje. 
- Generowane platformy mają różne wymiary, mnożnik skoków działa poprawnie. Zderzając się z ruchomą platformą jesteśmy wystrzeliwywani wyżej.

### Test drugi:
- Gra przestała działać błąd w pliku json


### Test trzeci:
- "g++ cialo/main.cpp cialo/grafika.cpp cialo/ruch.cpp cialo/punkty.cpp -o ../output/main.exe -lgdi32
../output/main.exe"
##### gra uruchamia się poprawnie:
        *skakanie | działa*
        *reset | działa*
        *zmiana tła | działa*
        *generowanie nowych platform | nie do końca działa - podczas generowania niektóre platformy się ze sobą przeplatają co powoduje błąd w ruchu. Odległości są w niektórych przypadkach za małe na postać co blokuje dostęp*
        *postać | nie do końca działa - usunięcie tła z modelu postaci powoduje nie ładowanie się go więc na szerokość i wysokość postaci jest szare tło*
        *przyciski do tworzenia, usuwania i modyfikacji | działa*
        *liczenie punktów | działa*

## 10. Wnioski i planowane usprawnienia
- Projekt spełnia założenia programowania obiektowego, łącząc logikę gry 2D z polimorfizmem, operacjami CRUD, przeciążaniem operatorów oraz obsługą wyjątków bez użycia gotowych silników graficznych. W ramach dalszego rozwoju aplikacji zaplanowano:

*Zwiększenie minimalnego odstępu pionowego oraz dodanie detekcji nakładania się w generatorze przesun_Mape(), aby wyeliminować zbyt bliskie generowanie platform.*

*Zastąpienie funkcji BitBlt funkcją TransparentBlt (biblioteka msimg32) dla rysowania postaci, aby uzyskać obsługę przezroczystego tła wokół modelu gracza.*

*Dodanie zapisu najwyższego wyniku (High Score) do pliku tekstowego.*