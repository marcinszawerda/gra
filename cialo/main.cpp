#include <windows.h>
#include <iostream>
#include "grafika.h"
#include "ruch.h"

bool running = true;

/* LRESULT to wartość całkowita, którą program zwraca do systemu Windows. 
Zawiera on odpowiedź programu na określony komunikat. 
Znaczenie tej wartości zależy od kodu komunikatu. 
CALLBACK jest konwencją wywoływania funkcji.
Odpowiada za obsługę komunikatów wysyłanych do okna aplikacji.
zmienna hwnd to uchwyt do okna, uMsg to kod komunikatu, wParam i lParam to dodatkowe informacje o komunikacie.
argumenty wParam i lParam są używane do przekazywania dodatkowych informacji o komunikacie, 
takich jak identyfikatory przycisków myszy lub klawiszy, współrzędne kursora itp.
zmiana flagi running na false powoduje zakończenie pętli głównej programu i wyjście z aplikacji.
PostQuitMessage(0) wysyła komunikat WM_QUIT do kolejki komunikatów, co powoduje zakończenie aplikacji.
*/
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DESTROY) {
        running = false;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    std::cout << "=== PROJEKT ZALICZENIOWY W C++: GRA PLATFORMOWA 2D ===\n";

    /// HINSTANCE to uchwyt do instancji aplikacji, który jest używany do identyfikacji aplikacji.
    HINSTANCE hInstance = GetModuleHandle(NULL);

    /* WNDCLASSA to struktura, która zawiera informacje o klasie okna, takie jak nazwa klasy, 
    wskaźnik do funkcji obsługi komunikatów, uchwyt do instancji aplikacji itp.
    */
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "Okno_Gry";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    RECT wr = {0, 0, 800, 600}; // Ustawienia rozmiaru obszaru roboczego okna
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    /*
    CreateWindowExA to funkcja, która tworzy okno aplikacji.
    Parametry funkcji to:
    - dwExStyle: rozszerzone style okna (0 oznacza brak dodatkowych stylów)
    - lpClassName: nazwa klasy okna (wc.lpszClassName)
    - lpWindowName: tytuł okna ("Gra Platformowa 2D")
    - dwStyle: styl okna (WS_OVERLAPPEDWINDOW | WS_VISIBLE oznacza standardowe okno, od razu widoczne)
    - x, y: współrzędne lewego górnego rogu okna (CW_USEDEFAULT oznacza domyślne położenie)
    - nWidth, nHeight: szerokość i wysokość okna po uwzględnieniu ramek
    */
    HWND hwnd = CreateWindowExA(
        0,
        "Okno_Gry",
        "Gra Platformowa 2D",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd) {
        std::cerr << "[BLAD] Nie udalo sie utworzyc okna aplikacji!\n";
        return -1;
    }

    // Główny blok try-catch obsługujący wyjątki podczas inicjalizacji i działania gry
    try {
        Main_graf grafika(800, 600);
        Gracz gracz(40.f, 60.f, 3);

        // Ustawienie postaci na pierwszej platformie na starcie gry
        gracz.ustaw_na_start(grafika.getPlatformy());
        std::cout << gracz << "\n";

        HDC hdc = GetDC(hwnd);
        int licznikDodanych = 6;

        // --- GŁÓWNA PĘTLA GRY ---
        while (running) {
            // Obsługa komunikatów systemu Windows (np. kliknięcie X, przesuwanie okna)
            MSG msg;
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            // Obsługa operacji na obiektach (CRUD) z wyłapywaniem wyjątków
            try {
                // [1] Dodanie nowej platformy
                if (GetAsyncKeyState('1') & 0x0001) {
                    std::string nazwa = "Plat_" + std::to_string(licznikDodanych++);
                    float losX = (float)(100 + rand() % 550);
                    float losY = (float)(120 + rand() % 350);
                    grafika.dodaj_Plat(new Plat_zwykla(nazwa, losX, losY, 110.f, 22.f, NULL, RGB(120, 180, 220)));
                }
                // [2] Usunięcie ostatniej platformy
                if (GetAsyncKeyState('2') & 0x0001) {
                    grafika.us_ost_Plat();
                }
                // [3] Wyszukanie i modyfikacja pozycji platformy "Plat_2"
                if (GetAsyncKeyState('3') & 0x0001) {
                    grafika.mod_Plat("Plat_2", 0.f, -15.f);
                }
            } catch (const std::exception& e) {
                std::cerr << "[WYJATEK OPERACJI NA OBIEKCIE] " << e.what() << "\n";
            }

            // Restart gry i losowanie nowej mapy klawiszem [R] w dowolnym momencie
            if (GetAsyncKeyState('R') & 0x0001) {
                gracz.setHp(3);
                grafika.gen_Poz();
                gracz.ustaw_na_start(grafika.getPlatformy());
            }

            // Aktualizacja logiki świata i gracza oraz rysowanie klatki
            grafika.aktualizuj(0.016f);
            gracz.aktualizuj(0.016f, grafika.getPlatformy(), grafika.getWysOkna());
            grafika.rysuj(hdc, gracz.getX(), gracz.getY(), gracz.getWidth(), gracz.getHeight(), gracz.getHp());

            Sleep(16); // Ograniczenie do ~60 FPS
        }

        ReleaseDC(hwnd, hdc);

    } catch (const std::exception& e) {
        std::cerr << "[KRYTYCZNY BLAD PROGRAMU] " << e.what() << "\n";
        return -1;
    }

    return 0;
}