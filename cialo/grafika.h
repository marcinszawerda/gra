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
    /// .f oznacza, że liczba jest typu float 
    /// bez konwersji z double na float, co może prowadzić do utraty precyzji.
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
        /*
        Konstruktor parametryczny
        */
        Platforma(const std::string& nazwa, float x, float y, float width, float height, HBITMAP bmp = NULL, COLORREF kol_zap);
        virtual ~Platforma(); /// Destruktor wirtualny
        /// destruktor wirtualny umożliwia poprawne usuwanie obiektów pochodnych przez wskaźnik do klasy bazowej.

        /// Metody dostępowe (gettery)
        std::string getNazwa() const;
        float getX() const;
        float getY() const;
        float getWidth() const;
        float getHeight() const;

        /// Metody ustawiające (settery z walidacją i wyjątkami)
        void setNazwa(const std::string& nazwa);
        void setPozycja(float noweX, float noweY);
        void setWymiary(float now_width, float now_height);

        /// metody wirtualne do rysowania i aktualizacji platformy
        virtual void rysuj(HDC memDC) const;
        /* HDC to uchwyt do kontekstu urządzenia, który reprezentuje powierzchnię rysowania.
            memDC to kontekst urządzenia pamięci, który jest używany do rysowania w pamięci, 
            a następnie kopiowania obrazu na ekran.
        */
        virtual void aktualizuj(float dt);
        /*
            aktualizuj(float dt) jest metodą czysto wirtualną, 
            co oznacza, że klasa Platforma jest klasą abstrakcyjną i nie można tworzyć jej obiektów bezpośrednio.
            dt to czas, który upłynął od ostatniej aktualizacji, 
            co pozwala na płynne animacje i ruchy obiektów w grze.
        */

        virtual float pob_mno_Skok() const; /// metoda wirtualna do pobierania mnożnika skoku
        virtual std::string pob_typ() const; /// metoda wirtualna do pobierania typu platformy

        // przeciążenie operatora strumieniowego do wyświetlania informacji o platformie
        bool operator==(const Platforma& inne) const;
        /// przeciążenie operatora porównania == do porównywania platform po nazwie i współrzędnych
        bool operator<(const Platforma& inne) const;
        /// przeciążenie operatora porównania < do porównywania platform po współrzędnych x i y
        friend std::ostream& operator<<(std::ostream& os, const Platforma& p);
        /// przeciążenie operatora strumieniowego << do wyświetlania informacji o platformie

    };



#endif