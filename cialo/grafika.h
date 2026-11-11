#ifndef GRAFIKA_H
#define GRAFIKA_H

#include <windows.h>
#include <vector> 
#include <string>
#include <iostream>
#include <stdexcept>

struct Szab_graf{ 
    HBITMAP bitmap = NULL; /// uchwyt do bitmapy
    float width = 120.f; /// szerokość bitmapy
    float height = 25.f; /// wysokość bitmapy
    COLORREF kol_zap = RGB(100, 180, 100); /// kolor wypełnienia (zielony)  
};

/// Klasa bazowa: Platforma
class Platforma {
    protected:
        std::string nazwa;
        float x, y; /// współrzędne platformy
        float width, height; /// wymiary platformy
        HBITMAP bitmap; /// uchwyt do bitmapy
        COLORREF kol_zap; /// kolor platformy
    
    public:
        /// Konstruktor domyślny
        Platforma();
        ///Konstruktor parametryczny
        Platforma(const std::string& nazwa, float x, float y, float width, float height, HBITMAP bmp = NULL, COLORREF kol_zap);
        virtual ~Platforma(); /// Destruktor wirtualny

        /// Metody dostępowe (gettery)
        std::string getNazwa() const;
        float getX() const;
        float getY() const;
        float getWidth() const; /// met
        float getHeight() const;

        /// Metody ustawiające (settery z walidacją i wyjątkami)
        void setNazwa(const std::string& nazwa); 
        void setPozycja(float noweX, float noweY); /// met
        void setWymiary(float now_width, float now_height);

        virtual void rysuj(HDC memDC) const;
        
        virtual void aktualizuj(float dt);

        virtual float pob_mno_Skok() const;
        virtual std::string pob_typ() const;

        // przeciążenie operatora strumieniowego 
        bool operator==(const Platforma& inne) const;
        /// przeciążenie operatora porównania == 
        bool operator<(const Platforma& inne) const;
        /// przeciążenie operatora porównania < 
        friend std::ostream& operator<<(std::ostream& os, const Platforma& p);
        /// przeciążenie operatora strumieniowego << 

    };

    

#endif