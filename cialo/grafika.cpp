#include "grafika.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

Platforma::Platforma() : nazwa(""), x(0.f), y(0.f), width(100.f), height(25.f), bitmap(NULL), kol_zap(RGB(100, 180, 100)) {}

Platforma::Platforma(const std:: string& nazwa, float x, float y, float width, float height,HBITMAP bmp, COLORREF kol_zap) 
: nazwa(nazwa), x(x), y(y), width(width), height(height), bitmap(NULL), kol_zap(kol_zap) {
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

Plat_zwykla::Plat_zwykla(const std::string& nazwa, float x, float y, float width, float height, HBITMAP bmp, COLORREF kol_zap) 
: Platforma(nazwa, x, y, width, height, bmp, kol_zap) {}

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
Plat_ruch::Plat_ruch(const std::string& nazwa, float x, float y, float width, float height, float mno_skok, float pred, float minX, float maxX, HBITMAP bmp, COLORREF kol_zap)
: Platforma(nazwa, x, y, width, height, bmp, kol_zap), mno_skok(mno_skok), pred(pred), minX(minX), maxX(maxX) {
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

