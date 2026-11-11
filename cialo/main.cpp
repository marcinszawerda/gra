#include <windows.h>
#include <iostream>
#include "grafika.h"
#include "ruch.h"

bool running = true;

/* LRESULT to wartość całkowita, którą program powraca do systemu Windows. 
Zawiera on odpowiedź programu na określony komunikat. 
Znaczenie tej wartości zależy od kodu komunikatu. 
Callback jest konwencją wywoływania funkcji.
Odpowiada za obsługę komunikatów wysyłanych do okna aplikacji.
zmienna hwnd to uchwyt do okna, uMsg to kod komunikatu, wParam i lParam to dodatkowe informacje o komunikacie.
argumenty wParam i lParam są używane do przekazywania dodatkowych informacji o komunikacie, 
takich jak identyfikatory przycisków myszy lub klawiszy, współrzędne kursora itp.
zmiana flag running na false powoduje zakończenie pętli głównej programu i wyjście z aplikacji.
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

    /// Hinstance to uchwyt do instancji aplikacji, który jest używany do identyfikacji aplikacji.
    HINSTANCE hInstance = GetModuleHandle(NULL);

    /* WNDCLASSW to struktura, która zawiera informacje o klasie okna, takie jak nazwa klasy, 
    wskaźnik do funkcji obsługi komunikatów, uchwyt do instancji aplikacji itp.
    */
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"Okno_Gry";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    RECT wr = {0, 0, 800, 600}; // Ustawienia rozmiaru okna
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    /*
    CreateWindowEx to funkcja, która tworzy okno aplikacji.
    Parametry funkcji to:
    - dwExStyle: rozszerzone style okna (0 oznacza brak dodatkowych stylów)
    - lpClassName: nazwa klasy okna (wc.lpszClassName)
    - lpWindowName: tytuł okna (L"Gra Platformowa 2D")
    - dwStyle: styl okna (WS_OVERLAPPEDWINDOW oznacza standardowe ok
    - no z paskiem tytułu, przyciskami minimalizacji i maksymalizacji)
    - x, y: współrzędne lewego górnego rogu okna (CW_USEDEFAULT oznacza domyślne położenie)
    - nWidth, nHeight: szerokość i wysokość okna (wr.right
    - wr.left i wr.bottom - wr.top oznaczają szerokość i wysokość okna po uwzględnieniu ramek)
    */
    HWND hwnd = CreateWindowEx(
        0,
        L"Okno_Gry",
        L"Gra Platformowa 2D",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    return 0;
}