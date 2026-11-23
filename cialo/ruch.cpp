#include "ruch.h"
#include <windows.h>

// Konstruktor domyślny (deleguje do parametrycznego)
Gracz::Gracz() : Gracz(40.f, 60.f, 3) {}

// Konstruktor parametryczny z walidacją danych
Gracz::Gracz(float width, float height, int start_hp)
    : x(0.f), y(0.f), width(width), height(height),
      pred_ruch(280.f), pred_Y(0.f), grawitacja(1200.f),
      sila_skoku(-540.f), akt_mno_skok(1.0f),
      na_ziemi(false), hp(start_hp) {
    if (width <= 0.f || height <= 0.f || start_hp <= 0) {
        throw std::invalid_argument("Wymiary gracza oraz poczatkowe zycia musza byc dodatnie!");
    }
}

// Gettery (oznaczone const)
float Gracz::getX() const { return x; }
float Gracz::getY() const { return y; }
float Gracz::getWidth() const { return width; }
float Gracz::getHeight() const { return height; }
int Gracz::getHp() const { return hp; }

// Settery
void Gracz::setHp(int noweHp) {
    if (noweHp < 0) {
        throw std::invalid_argument("Liczba zyc gracza nie moze byc ujemna!");
    }
    hp = noweHp;
}

void Gracz::setPozycja(float noweX, float noweY) {
    x = noweX;
    y = noweY;
}

// Sprawdzanie kolizji prostokąta gracza z prostokątem platformy (AABB)
bool Gracz::przecina(const Platforma& plat) const {
    return (x < plat.getX() + plat.getWidth() &&
            x + width > plat.getX() &&
            y < plat.getY() + plat.getHeight() &&
            y + height > plat.getY());
}

// Ustawia postać na środku pierwszej platformy z wektora
void Gracz::ustaw_na_start(const std::vector<Platforma*>& platformy) {
    if (platformy.empty()) {
        throw std::runtime_error("Brak platform na mapie - nie mozna ustawic gracza na starcie!");
    }
    const Platforma* pierwsza = platformy[0];
    x = pierwsza->getX() + (pierwsza->getWidth() - width) / 2.f;
    y = pierwsza->getY() - height;
    pred_Y = 0.f;
    na_ziemi = true;
}

// Główna aktualizacja ruchu, skoku, kolizji i upadku poza mapę
void Gracz::aktualizuj(float dt, const std::vector<Platforma*>& platformy, int wys_okna) {
    if (hp <= 0) return; // Gdy brak żyć, postać nie reaguje na sterowanie

    // 1. Ruch poziomy (klawisze A i D)
    float dx = 0.f;
    if (GetAsyncKeyState('A') & 0x8000) dx -= pred_ruch * dt;
    if (GetAsyncKeyState('D') & 0x8000) dx += pred_ruch * dt;
    x += dx;

    // Kolizje poziome ze ścianami platform
    for (const Platforma* plat : platformy) {
        if (przecina(*plat)) {
            if (dx > 0) x = plat->getX() - width;
            else if (dx < 0) x = plat->getX() + plat->getWidth();
        }
    }

    // 2. Skok (W lub Spacja) z uwzględnieniem polimorficznego mnożnika platformy
    if (((GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState(VK_SPACE) & 0x8000)) && na_ziemi) {
        pred_Y = sila_skoku * akt_mno_skok;
        na_ziemi = false;
    }

    // Grawitacja i ruch pionowy
    pred_Y += grawitacja * dt;
    y += pred_Y * dt;
    na_ziemi = false;
    akt_mno_skok = 1.0f;

    // 3. Kolizje pionowe (stanie na platformie lub uderzenie głową od dołu)
    for (const Platforma* plat : platformy) {
        if (przecina(*plat)) {
            if (pred_Y > 0) {
                y = plat->getY() - height; // Postaw gracza na górze platformy
                pred_Y = 0.f;
                na_ziemi = true;
                akt_mno_skok = plat->pob_mno_Skok(); // Polimorficzne pobranie mnożnika skoku!
            } else if (pred_Y < 0) {
                y = plat->getY() + plat->getHeight(); // Uderzenie głową w spód platformy
                pred_Y = 0.f;
            }
        }
    }

    // 4. Sprawdzenie upadku poza dolną krawędź okna (utrata 1 życia przez operator -=)
    if (y > (float)wys_okna) {
        *this -= 1;
        std::cout << "[UPADEK POZA MAPE] " << *this << "\n";
        if (hp > 0) {
            ustaw_na_start(platformy); // Odrodzenie na pierwszej platformie
        }
    }
}

// Przeciążone operatory
Gracz& Gracz::operator-=(int obrazenia) {
    hp -= obrazenia;
    if (hp < 0) hp = 0;
    return *this;
}

Gracz& Gracz::operator+=(int leczenie) {
    hp += leczenie;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Gracz& g) {
    os << "[Gracz] Poz: (" << g.x << ", " << g.y << "), Zycia: " << g.hp;
    return os;
}