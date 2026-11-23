#ifndef PUNKTY_H
#define PUNKTY_H

#include "grafika.h"
#include <set>
#include <string>
#include <iostream>

/// Klasa zliczająca unikalne platformy, na które wskoczył gracz
class Punkty {
    private:
        int wynik; /// aktualna liczba punktów
        std::set<std::string> odwiedzone; /// zbiór nazw już zaliczonych platform

    public:
        Punkty(); /// Konstruktor domyślny

        int getWynik() const; /// getter wyniku
        void oznacz_start(const Platforma* plat); /// oznacza platformę startową bez dodawania punktu
        bool zalicz_Plat(const Platforma* plat); /// dodaje punkt, jeśli platforma nie była odwiedzona
        void reset(); /// zeruje punkty i historię odwiedzonych platform

        /// Przeciążony operator strumieniowy
        friend std::ostream& operator<<(std::ostream& os, const Punkty& p);
};

#endif