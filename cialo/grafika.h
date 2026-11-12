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
        Platforma(const std::string& nazwa, float x, float y, float width, float height, HBITMAP bmp = NULL, COLORREF kol_zap = RGB(100, 180, 100));
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

    /// Klasy pochodne

    class Plat_zwykla : public Platforma {
    public:
        Plat_zwykla(); /// Konstruktor domyślny
        Plat_zwykla(const std::string& nazwa, float x, float y, float width, float height, HBITMAP bmp = NULL, COLORREF kol_zap = RGB(100, 180, 100));
        void rysuj(HDC memDC) const override; // Przesłonięcie metody rysuj
        float pob_mno_Skok() const override; // Przesłonięcie metody pob_mno_Skok
        std::string pob_typ() const override; // Przesłonięcie metody pob_typ
        /// przesłonięcie oznacza nadpisanie metody w klasie pochodnej
    };

    class plat_ruch : public Platforma {
        private:
        float mno_skok; /// mnożnik skoku platformy ruchomej
        float pred; /// prędkość platformy ruchomej
        float minX, maxX; /// granice ruchu platformy

    public:
        plat_ruch(); /// Konstruktor domyślny
        plat_ruch(const std::string& nazwa, float x, float y, float width, float height, float mno_skok, float pred, float minX, float maxX, HBITMAP bmp = NULL, COLORREF kol_zap = RGB(100, 180, 100));
        float getMnoS() const; /// getter mnożnika skoku
        void setMnoS(float noweMnoS); /// setter mnożnika skoku
        void rysuj(HDC memDC) const override; // Przesłonięcie metody rysuj
        void aktualizuj(float dt) override; // Przesłonięcie metody aktualizuj
        float pob_mno_Skok() const override; // Przesłonięcie metody pob_mno_Skok
        std::string pob_typ() const override; // Przesłonięcie metody pob_typ

    };

    // klasa zarządzająca 

    class Main_graf {
        private:
            int szer_okna; /// szerokość okna
            int wys_okna; /// wysokość okna
            HBITMAP tBitmap; /// uchwyt do bitmapy
            HBITMAP posBitmap; /// uchwyt do starej bitmapy
            std::vector<Szab_graf> grafika; /// wektor przechowujący obiekty typu Szab_graf
            std::vector<Platforma*> platformy; /// wektor przechowujący wskaźniki do obiektów typu Platforma

            void czysc_Plat();
            void dodaj_szab_Plat(const char* plat_1, float width, float height, COLORREF kol_zap);

        public:
            Main_graf(); /// Konstruktor domyślny
            Main_graf(int szer, int wys); /// Konstruktor parametryczny
            ~Main_graf(); /// Destruktor

            void dodaj_Plat(Platforma* plat); /// dodanie platformy do wektora
            void us_ost_Plat(); /// usunięcie ostatniej platformy z wektora
            Platforma* znajdz_Plat_nazw(const std::string& nazwa, float X, float Y); /// wyszukiwanie platformy po nazwie i współrzędnych

            void gen_Poz(); /// generowanie pozycji platform
            void aktualizuj(float dt); /// aktualizacja pozycji platform
            void rysuj(HDC hdc, float graczX, float graczY, float graczW, float graczH, int hp) const; /// rysowanie platform i gracza
        
            const std::vector<Platforma*>& getPlatformy() const; /// getter wektora platform
            int getWysOkna() const; /// getter wysokości okna
        };

#endif