#include "grafika.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

Platforma::Platforma() : nazwa(""), x(0.f), y(0.f), width(100.f), height(25.f), bitmap(NULL), kol_zap(RGB(100, 180, 100)) {}

Platforma::Platforma(const std:: string& nazwa, float x, float y, float width, float height, HBITMAP bmp, COLORREF kolor) 
: nazwa(nazwa), x(x), y(y), width(width), height(height), bitmap(NULL), kol_zap(kolor) {
setWymiary(width, height); // Walidacja wymiarów platformy przez setter
}

Platforma::~Platforma() {}

std::string Platforma::getNazwa() const { return nazwa; }
float Platforma::getX() const { return x; }
float Platforma::getY() const { return y; }
float Platforma::getWidth() const { return width; }
float Platforma::getHeight() const { return height; }

void Platforma::setNazwa(const std::string& nowaNazwa){
    if (nowaNazwa.empty()){
        throw std::invalid_argument("Nazwa platformy nie może być pusta");
    }
    nazwa = nowaNazwa;
}

void Platforma::setPozycja(float noweX, float noweY){
    if (noweX < 0.f || noweX > 800.f || noweY < 0.f || noweY > 600.f){
        throw std::out_of_range("Pozycja platformy przekracza poza granice mapy");
    }
    x = noweX;
    y = noweY;
}

void Platforma::setWymiary(float now_width, float now_height){
    if (now_width <= 0.f || now_height <= 0.f){
        throw std::invalid_argument("Wymiary platformy muszą być dodatnie");
    }
    width = now_width;
    height = now_height;
}
// Implementacja metody rysującej platformę
void Platforma::rysuj(HDC memDC) const {
    if (bitmap) {
        HDC platDC = CreateCompatibleDC(memDC); // Tworzenie kontekstów urządzenia dla bitmapy
        HGDIOBJ oldPlat = SelectObject(platDC, bitmap); // Wybieranie bitmapy do kontekstu urządzenia
        BitBlt(memDC, (int)x, (int)y, (int)width, (int)height, platDC, 0, 0, SRCCOPY); // Kopiowanie bitmapy na kontekst urządzenia
        SelectObject(platDC, oldPlat); // Przywracanie poprzedniego obiektu do kontekstu urządzenia
        DeleteDC(platDC); // Usuwanie kontekstu urządzenia dla bitmapy
    } else {
        HBRUSH platbrush = CreateSolidBrush(kol_zap); // Tworzenie pędzla do wypełnienia prostokąta
        RECT rect = { (int)x, (int)y, (int)(x + width), (int)(y + height) }; // Definiowanie prostokąta platformy
        FillRect(memDC, &rect, platbrush); // Wypełnianie prostokąta kolorem platformy
        DeleteObject(platbrush);
    }
}

void Platforma::aktualizuj(float dt){
    (void)dt; // Domyślna implementacja nie zmienia pozycji platformy
}

float Platforma::pob_mno_Skok() const {
    return 1.0f; // Domyślny mnożnik skoku dla zwykłej platformy
}
std::string Platforma::pob_typ() const {
    return "Plat_zwykla"; // Domyślny typ platformy
}

bool Platforma::operator==(const Platforma& inne) const {
    return (this -> nazwa == inne.nazwa) || (this -> x == inne.x && this -> y == inne.y);
}

bool Platforma::operator<(const Platforma& inne) const {
    return this ->y < inne.y;
}

std::ostream& operator<<(std::ostream& os, const Platforma& p){
    os << "[" << p.pob_typ() << "] Nazwa: " << p.nazwa << ", Poz: (" << p.x << ", " << p.y << "), Wym: (" << p.width << " x " << p.height << " Mnożnik skoku: x" << p.pob_mno_Skok() << ")";
    return os;
}

// Implementacja klasy Plat_zwykla

Plat_zwykla::Plat_zwykla() : Platforma() {}

Plat_zwykla::Plat_zwykla(const std::string& nazwa, float x, float y, float width, float height, HBITMAP bmp, COLORREF kolor) 
: Platforma(nazwa, x, y, width, height, bmp, kolor) {}

void Plat_zwykla::rysuj(HDC memDC) const {
    Platforma::rysuj(memDC); // Wywołanie metody rysującej z klasy bazowej
}

float Plat_zwykla::pob_mno_Skok() const {
    return 1.0f; // Zwykła platforma nie zmienia mnożnika skoku
}
std::string Plat_zwykla::pob_typ() const {
    return "Plat_zwykla"; // Typ platformy zwykłej
}

// Implementacja klasy Plat_ruch

Plat_ruch::Plat_ruch() : Platforma(), mno_skok(1.35f), pred(60.0f), minX(50.0f), maxX(650.0f) {}
Plat_ruch::Plat_ruch(const std::string& nazwa, float x, float y, float width, float height, float mno_skok, float pred, float minX, float maxX, HBITMAP bmp, COLORREF kolor)
: Platforma(nazwa, x, y, width, height, bmp, kolor), mno_skok(mno_skok), pred(pred), minX(minX), maxX(maxX) {
    minX =(x - 60.f > 10.f) ? x - 60.f : 10.f; // Ustawienie granicy minimalnej ruchu platformy
    maxX =(x + 60.f < 750.f) ? x + 60.f : 750.f; // Ustawienie granicy maksymalnej ruchu platformy
    // ? oznacza operator warunkowy, który sprawdza warunek i zwraca jedną z dwóch wartości w zależności od wyniku tego warunku.
}

float Plat_ruch::getMnoS() const {
    return mno_skok; // Zwraca wartość mnożnika skoku platformy ruchomej
}

void Plat_ruch::setMnoS(float noweMnoS) {
    if (noweMnoS <= 0.f) {
        throw std::invalid_argument("Mnożnik skoku platformy musi być dodatni");
    }
    mno_skok = noweMnoS; // Ustawia nową wartość mnożnika skoku platformy ruchomej
}

void Plat_ruch::rysuj(HDC memDC) const {
    Platforma::rysuj(memDC); // Wywołanie metody rysującej z klasy bazowej
    HBRUSH pasbrush = CreateSolidBrush(RGB(255, 215, 0)); // Tworzenie pędzla do rysowania pasów platformy ruchomej
    RECT r = { (int)x, (int)y, (int)(x + width), (int)(y + 6) }; // Definiowanie prostokąta dla pierwszego pasa
    FillRect(memDC, &r, pasbrush); // Wypełnianie prostokąta pierwszego pasa kolorem
    DeleteObject(pasbrush); // Usuwanie pędzla po użyciu
}

void Plat_ruch::aktualizuj(float dt) {
    x += pred * dt; // Aktualizacja pozycji platformy ruchomej na podstawie prędkości i czasu
    if (x < minX) {
        x = minX; // Ograniczenie pozycji do granicy minimalnej
        pred = -pred; // Zmiana kierunku ruchu
    } else if (x > maxX) {
        x = maxX; // Ograniczenie pozycji do granicy maksymalnej
        pred = -pred; // Zmiana kierunku ruchu
    }
}

float Plat_ruch::pob_mno_Skok() const {
    return mno_skok; // Zwraca wartość mnożnika skoku platformy ruchomej
}
std::string Plat_ruch::pob_typ() const {
    return "Plat_ruch"; // Typ platformy ruchomej
}

Main_graf::Main_graf() : Main_graf(800, 600) {} // Wywołanie konstruktora parametrycznego z domyślnymi wymiarami okna

Main_graf::Main_graf(int szer, int wys) 
: szer_okna(szer), wys_okna(wys), tBitmap(NULL), posBitmap(NULL) {
    srand((unsigned int)  time(NULL)); // Inicjalizacja generatora liczb losowych

    tBitmap = (HBITMAP)LoadImageA(NULL, "obrazy/tlo.bmp", IMAGE_BITMAP, szer_okna, wys_okna, LR_LOADFROMFILE); // Ładowanie bitmapy tła z pliku
    posBitmap = (HBITMAP)LoadImageA(NULL, "obrazy/postac.bmp", IMAGE_BITMAP, 40, 60, LR_LOADFROMFILE); // Ładowanie bitmapy postaci z pliku);

    dodaj_szab_Plat("obrazy/plat_1.bmp", 170.f, 25.f, RGB(90, 190, 90));
    dodaj_szab_Plat("obrazy/plat_2.bmp", 125.f, 25.f, RGB(70, 160, 140));
    dodaj_szab_Plat("obrzy/plat_3.bmp", 90.f, 25.f, RGB(140, 170, 70));

    gen_Poz(); // Generowanie pozycji platform
}

Main_graf::~Main_graf() {
    czysc_Plat(); // Czyszczenie wektora platform
    if (tBitmap) DeleteObject(tBitmap); // Usuwanie bitmapy tła
    if (posBitmap) DeleteObject(posBitmap); // Usuwanie bitmapy postaci
    for (auto& szab : grafika) {
        if (szab.bitmap) DeleteObject(szab.bitmap); // Usuwanie bitmap szablonów platform
    }
}

void Main_graf::czysc_Plat() {
    for (Platforma* p : platformy){
        delete p;
    }
    platformy.clear();
}

void Main_graf::dodaj_szab_Plat(const char* plat_1, float width, float height, COLORREF kolor) {
    Szab_graf sz;
    sz.bitmap = (HBITMAP)LoadImageA(NULL, plat_1, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    sz.kol_zap = kolor;

    if (sz.bitmap) {
        BITMAP bm;
        GetObject(sz.bitmap, sizeof(BITMAP), &bm);
        sz.width = (float)bm.bmWidth;
        sz.height = (float)bm.bmHeight;
    }
    else {
        sz.width = width;
        sz.height = height;
    }
    grafika.push_back(sz);
}

// 1. OPERACJA DODAWANIA OBIEKTU (z użyciem przeciążonego operatora == oraz wyjątków)
void Main_graf::dodaj_Plat(Platforma* nowaPlatforma) {
    if (!nowaPlatforma) {
        throw std::invalid_argument("Proba dodania pustego wskaznika platformy!");
    }
    for (const Platforma* istniejaca : platformy) {
        if (*istniejaca == *nowaPlatforma) {
            delete nowaPlatforma;
            throw std::runtime_error("Platforma o tej samej nazwie lub pozycji juz istnieje!");
        }
    }
    platformy.push_back(nowaPlatforma);
    std::cout << "[DODANO OBIEKT] " << *nowaPlatforma << "\n";
}

// 2. OPERACJA USUWANIA OBIEKTU (z rzucaniem wyjątku przy próbie usunięcia platformy startowej)
void Main_graf::us_ost_Plat() {
    if (platformy.size() <= 1) {
        throw std::out_of_range("Nie mozna usunac pierwszej platformy startowej!");
    }
    Platforma* ostatnia = platformy.back();
    std::cout << "[USUNIETO OBIEKT] " << *ostatnia << "\n";
    delete ostatnia;
    platformy.pop_back();
}

// 3. OPERACJA WYSZUKIWANIA OBIEKTU
Platforma* Main_graf::znajdz_Plat_nazw(const std::string& szu_nazwa) const {
    for (Platforma* p : platformy) {
        if (p->getNazwa() == szu_nazwa) {
            return p;
        }
    }
    throw std::runtime_error("Nie znaleziono platformy o nazwie: " + szu_nazwa);
}

// 4. OPERACJA MODYFIKACJI OBIEKTU
void Main_graf::mod_Plat(const std::string& nazwa, float X, float Y) {
    Platforma* p = znajdz_Plat_nazw(nazwa);
    p->setPozycja(p->getX() + X, p->getY() + Y);
    std::cout << "[ZMODYFIKOWANO OBIEKT] " << *p << "\n";
}

void Main_graf::gen_Poz() {
    czysc_Plat();
    std::cout << "\n--- GENEROWANIE NOWEGO POZIOMU ---\n";

    if (grafika.empty()) {
            std::cerr << "[BLAD GENEROWANIA] Brak szablonow platform w wektorze grafika!\n";
            return;
    }

    float bazaX[5] = { 40.f, 200.f, 360.f, 520.f, 650.f };
    float bazaY[5] = { 490.f, 410.f, 330.f, 240.f, 160.f };

    for (int i = 0; i < 5; ++i) {
        int losowaWersja = rand() % grafika.size();
        const Szab_graf& sz = grafika[losowaWersja];
        float y = (i == 0) ? 0.f : (float)((rand() % 40) - 20);
        std::string nazwa = "Plat_" + std::to_string(i + 1);

        try {
            // Pierwsza platforma jest zawsze zwykła, kolejne losowo mogą być sprężystymi trampolinami
            if (i == 0 || (rand() % 2 == 0)) {
                dodaj_Plat(new Plat_zwykla(
                    nazwa, bazaX[i], bazaY[i] + y, sz.width, sz.height, sz.bitmap, sz.kol_zap
                ));
            } else {
                    float startX = bazaX[i];
                float gran_Min = (startX - 60.f > 10.f) ? startX - 60.f : 10.f;
                float gran_Max = (startX + 60.f < (float)szer_okna - 10.f - sz.width) ? startX + 60.f
                    : (float)szer_okna - 10.f - sz.width;

                dodaj_Plat(new Plat_ruch(
                    nazwa, bazaX[i], bazaY[i] + y, sz.width, sz.height, 1.35f, 55.f, gran_Min, gran_Max, sz.bitmap, sz.kol_zap));
            }
        } catch (const std::exception& e) {
            std::cerr << "[BLAD GENEROWANIA] " << e.what() << "\n";
        }
    }

    // Praktyczne użycie przeciążonego operatora < (wyszukanie najwyżej położonej platformy)
    if (!platformy.empty()) {
        auto najwyzsza = std::min_element(platformy.begin(), platformy.end(),
            [](const Platforma* a, const Platforma* b) { return *a < *b; });
        std::cout << "[INFO] Najwyzsza platforma na mapie to: " << (*najwyzsza)->getNazwa() << "\n";
    }
}

void Main_graf::aktualizuj(float dt) {
    // Polimorficzne wywołanie aktualizacji dla wszystkich platform
    for (Platforma* plat : platformy) {
        plat->aktualizuj(dt);
    }
}

void Main_graf::rysuj(HDC hdc, float graczX, float graczY, float graczW, float graczH, int zycia) const {
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, szer_okna, wys_okna);
    HGDIOBJ oldBmp = SelectObject(memDC, memBitmap);

    // 1. STAŁE TŁO
    if (tBitmap) {
        HDC bgDC = CreateCompatibleDC(memDC);
        HGDIOBJ oldBg = SelectObject(bgDC, tBitmap);
        BitBlt(memDC, 0, 0, szer_okna, wys_okna, bgDC, 0, 0, SRCCOPY);
        SelectObject(bgDC, oldBg);
        DeleteDC(bgDC);
    } else {
        RECT bgRect = {0, 0, szer_okna, wys_okna};
        HBRUSH bgBrush = CreateSolidBrush(RGB(35, 40, 55));
        FillRect(memDC, &bgRect, bgBrush);
        DeleteObject(bgBrush);
    }

    // 2. POLIMORFICZNE RYSOWANIE PLATFORM
    for (const Platforma* plat : platformy) {
        plat->rysuj(memDC);
    }

    // 3. POSTAĆ GRACZA
    if (zycia > 0) {
        if (posBitmap) {
            HDC pDC = CreateCompatibleDC(memDC);
            HGDIOBJ oldP = SelectObject(pDC, posBitmap);
            BitBlt(memDC, (int)graczX, (int)graczY, (int)graczW, (int)graczH, pDC, 0, 0, SRCCOPY);
            SelectObject(pDC, oldP);
            DeleteDC(pDC);
        } else {
            HBRUSH playerBrush = CreateSolidBrush(RGB(230, 80, 80));
            RECT pr = {(int)graczX, (int)graczY, (int)(graczX + graczW), (int)(graczY + graczH)};
            FillRect(memDC, &pr, playerBrush);
            DeleteObject(playerBrush);
        }
    }

    // 4. INTERFEJS UŻYTKOWNIKA
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 255, 255));
    std::string tekstZyc = "Zycia: " + std::to_string(zycia) + " | Klawisze: [1] Dodaj plat. [2] Usun [3] Modyfikuj Plat_2";
    TextOutA(memDC, 20, 15, tekstZyc.c_str(), (int)tekstZyc.length());

    HBRUSH lifeBrush = CreateSolidBrush(RGB(240, 60, 60));
    for (int i = 0; i < zycia; ++i) {
        RECT lr = { 20 + i * 25, 40, 38 + i * 25, 58 };
        FillRect(memDC, &lr, lifeBrush);
    }
    DeleteObject(lifeBrush);

    if (zycia <= 0) {
        std::string koniec = "KONIEC GRY! Wcisnij [R], aby wylosowac mape i zagrac od nowa.";
        TextOutA(memDC, 190, 280, koniec.c_str(), (int)koniec.length());
    }

    BitBlt(hdc, 0, 0, szer_okna, wys_okna, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBmp);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
}

const std::vector<Platforma*>& Main_graf::getPlatformy() const { return platformy; }
int Main_graf::getWysOkna() const { return wys_okna; }

