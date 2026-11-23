#include "punkty.h"

Punkty::Punkty() : wynik(0) {}

int Punkty::getWynik() const {
    return wynik;
}

void Punkty::oznacz_start(const Platforma* plat) {
    if (plat) {
        odwiedzone.insert(plat->getNazwa());
    }
}

bool Punkty::zalicz_Plat(const Platforma* plat) {
    if (!plat) return false;

    // insert().second zwraca true tylko wtedy, gdy nazwa nie istniała jeszcze w zbiorze
    if (odwiedzone.insert(plat->getNazwa()).second) {
        wynik++;
        std::cout << "[PUNKT!] Odwiedzono: " << plat->getNazwa() << " | Wynik: " << wynik << "\n";
        return true;
    }
    return false;
}

void Punkty::reset() {
    wynik = 0;
    odwiedzone.clear();
}

std::ostream& operator<<(std::ostream& os, const Punkty& p) {
    os << "[Punkty] Wynik: " << p.wynik;
    return os;
}