#ifndef RUCH_H
#define RUCH_H

#include "punkty.h"
#include "grafika.h"
#include <vector>
#include <iostream>
#include <stdexcept>

/// Klasa reprezentująca postać gracza, jego fizykę, kolizje oraz system żyć
class Gracz {
    private:
        float x, y; /// współrzędne pozycji gracza
        float width, height; /// wymiary gracza
        float pred_ruch; /// prędkość poruszania się w poziomie (A / D)
        float pred_Y; /// aktualna prędkość pionowa (skok / spadanie)
        float grawitacja; /// siła grawitacji ciągnąca w dół
        float sila_skoku; /// bazowa siła wybicia przy skoku
        float akt_mno_skok; /// mnożnik skoku pobrany z platformy, na której stoi gracz
        bool na_ziemi; /// flaga określająca, czy gracz stoi na platformie
        int hp; /// aktualna liczba żyć gracza

    public:
        /// Konstruktor domyślny
        Gracz();
        /// Konstruktor parametryczny
        Gracz(float width, float height, int start_hp);

        /// Metody dostępowe (gettery)
        float getX() const;
        float getY() const;
        float getWidth() const;
        float getHeight() const;
        int getHp() const;

        /// Metody ustawiające (settery z walidacją i wyjątkami)
        void setHp(int noweHp);
        void setPozycja(float noweX, float noweY);

        /// Metody logiki gry, kolizji i fizyki
        bool przecina(const Platforma& plat) const; /// wykrywanie kolizji AABB z platformą
        void ustaw_na_start(const std::vector<Platforma*>& platformy); /// ustawienie gracza na pierwszej platformie
        void aktualizuj(float dt, const std::vector<Platforma*>& platformy, int wys_okna, Punkty& punkty); /// ruch, skok, grawitacja i utrata żyć

        /// Przeciążone operatory (wymaganie na próg 10.0 pkt)
        Gracz& operator-=(int obrazenia); /// odejmowanie żyć
        Gracz& operator+=(int leczenie); /// dodawanie żyć
        friend std::ostream& operator<<(std::ostream& os, const Gracz& g); /// wypisywanie stanu gracza do konsoli
};

#endif