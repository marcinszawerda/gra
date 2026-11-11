#Projekt zaliczeniowy na przedmiot Języki obiektowe

1. ## Opis architektury i funkcjonalności
Program realizuje dwuwymiarową grę platformową z interfejsem graficznym (WinAPI GDI) oraz diagnostycznym interfejsem konsolowym. Projekt został podzielony na moduły nagłówkowe (.h) oraz implementacyjne (.cpp):

- **grafika.h**:

*include*:
'windows.h' - windows.h to nagłówek zawierający deklaracje funkcji API systemu Windows, struktur danych i stałych używanych w programowaniu aplikacji Windows.
'vector' - vektor do przechowywania platform
'string' - ciąg znaków
'iostream' - iostream do wyświetlania informacji o platformie
'stdexcept' - stdexeption do obsługi wyjątków

*struktura szablonu grafiki*

'szab_graf' - struktura przechowująca informacje o grafice platformy, takie jak uchwyt do bitmapy, wymiary i kolor wypełnienia.

'HBITMAP bitmap' - uchwyt do bitmapy
'float width' - szerokość bitmapy
'float height' - wysokość bitmapy
'.f' oznacza, że liczba jest typu float bez konwersji z double na float, co może prowadzić do utraty precyzji.
'COLORREF kol_zap' - kolor wypełnienia 
    

*Konstruktor domyślny*
'Platforma()'
'HBITMAP bmp' - bmp to uchwyt do bitmapy, który jest używany do rysowania platformy.
'virtual ~Platforma()' - destruktor wirtualny umożliwia poprawne usuwanie obiektów pochodnych przez wskaźnik do klasy bazowej.

*Metody dostępowe (gettery)*
'string getNazwa()' - metoda zwracająca nazwę
'float getX()' - metoda pobierająca współżędne x platformy
'float getY()' - metoda pobierająca współżędne y platformy
'float getWidth()' - metoda pobierająca szerokość
'float getHeight()' - metoda pobierająca wysokość

*Metody ustawiające*
'setNazwa' - metoda ustawiająca nazwę platformy;
'setPozycja' - metoda ustawiająca pozycję
'setWymiary' - metoda ustawiająca wymiary

*metody wirtualne do rysowania i aktualizacji platformy*
'rysuj(HDC memDC)' - metoda wirtualna do rysowania platformy na kontekście urządzenia pamięci
'HDC' - uchwyt do kontekstu urządzenia, który reprezentuje powierzchnię rysowania.
'memDC' - kontekst urządzenia pamięci, który jest używany do rysowania w pamięci, a następnie kopiowania obrazu na ekran.

'aktualizuj(float dt)' - metoda wirtualna do aktualizacji platformy. Jest metodą czysto wirtualną, co oznacza, że klasa Platforma jest klasą abstrakcyjną i nie można tworzyć jej obiektów bezpośrednio.
'dt' to czas, który upłynął od ostatniej aktualizacji, co pozwala na płynne animacje i ruchy obiektów w grze.

*metody wirtualne do pobierania mnożnika skoku i typu platformy*

'float pob_mno_Skok()' - metoda wirtualna do pobierania mnożnika skoku
'string pob_typ()' - metoda wirtualna do pobierania typu platformy

*Przeciążenia operatora*
przeciążenie operatora strumieniowego do wyświetlania informacji o platformie

'bool operator==' - przeciążenie operatora porównania == do porównywania platform po nazwie i współrzędnych
'bool operator<' - przeciążenie operatora porównania < do porównywania platform po współrzędnych x i y
'std::ostream& operator<<' - przeciążenie operatora strumieniowego << do wyświetlania informacji o platformie

