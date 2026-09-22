#include <cstring>
#include <iostream>

// Vežba: klasa koja poseduje resurs (npr. char* na heap-u) sa NAIVNIM
// kompajlerski-generisanim kopiranjem (ne piši copy ctor/assignment).
// Napravi dva objekta, kopiraj jedan u drugi, izmeni jedan i pokaži da je
// drugi takođe promenjen (shallow copy bug). Pokreni pod ASan-om — očekuj
// double-free ili use-after-free kad oba destruktora oslobode isti pokazivač.
//
// Zatim popravi: napiši copy ctor i copy assignment koji rade DEEP copy,
// uključujući self-assignment proveru u operator=.
//
// Ako ostaviš klasu BEZ copy ctor/assignment kad poseduje resurs (NIJE
// DOBRO) jer kompajler GENERIŠE default verzije koje rade SHALLOW copy
// (kopiraju pokazivač, ne ono na šta pokazuje) -- oba objekta onda misle
// da poseduju ISTI resurs, i oba destruktora će pokušati da ga oslobode
// (double-free, tačno ono što vidiš ispod pod ASan-om).
// Treba da napišeš SOPSTVENI copy ctor/assignment koji rade DEEP copy
// (alociraju NOVU memoriju i kopiraju SADRŽAJ) kad god klasa poseduje
// resurs preko sirovog pokazivača.
// Možeš i izbeći problem u korenu -- koristi std::string umesto char* --
// već ima ispravno deep-copy ponašanje ugrađeno, ne moraš ti da ga pišeš.

class NaiveString {
public:
    explicit NaiveString(const char* s) {
        data_ = new char[std::strlen(s) + 1];
        std::strcpy(data_, s);
    }
    ~NaiveString() { delete[] data_; }
    // Namerno bez copy ctor / copy assignment -- posmatraj šta se dešava.

private:
    char* data_;
};

int main() {
    // unitbuf -- auto-flush posle svake cout operacije, da ispis ne
    // ostane zaglavljen u baferu ako program pukne pre nego što se
    // isprazni (bitno kad je stdout preusmeren u fajl, ne terminal).
    std::cout.setf(std::ios::unitbuf);

    std::cout << "-- shallow copy (namerni bag, ASan treba da uhvati double-free) --\n";
    NaiveString a("hello");
    NaiveString b = a; // shallow copy
    (void)b;
}
