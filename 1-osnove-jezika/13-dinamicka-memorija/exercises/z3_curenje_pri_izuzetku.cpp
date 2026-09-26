// VRSTA: zašto
// DEMO-UB: NAIVNO detected memory leaks
//
// Zadatak 3 -- zašto ručni new curi čim nešto baci izuzetak (sekcija 7, R.11)
// Rešenje: exercises/solutions/z3_curenje_pri_izuzetku.cpp
//
// otvoriSve(n) otvara n kanala. Kanal 3 ne postoji i njegov konstruktor
// baci izuzetak.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-osnove-jezika/13-dinamicka-memorija/exercises/z3_curenje_pri_izuzetku.cpp -DNAIVNO
//   Izuzetak je uhvaćen i program se "uredno" završi, ali na izlazu
//   LeakSanitizer (deo ASan-a na Linux-u) prijavi "detected memory leaks":
//   niz pokazivača i kanali 0, 1, 2 nisu oslobođeni. Kad izuzetak izleti
//   iz otvoriSve, niko više nema pokazivač na njih. Brojač živih kanala
//   pokazuje isto: destruktori se nisu pozvali.
//   (Na Windows-u ASan ne prijavljuje curenje -- tamo gledaj brojač.
//   Ako izlaz preusmeriš u fajl ili pipe, red "greška: ..." može da
//   nestane: LeakSanitizer završi program pre nego što se isprazni bafer
//   std::cout-a. U terminalu se vidi.)
// Korak 2: u #else grani napiši otvoriSve tako da vraća
//   std::vector<std::unique_ptr<Kanal>>. Kad izuzetak izleti usred petlje,
//   vektor se uništi, a sa njim i svi već otvoreni kanali.
// Korak 3: probaj i bez pokazivača: std::vector<Kanal> sa reserve(n) i
//   emplace_back(i). Zašto reserve? (bez njega bi rast vektora
//   premeštao Kanal-e -- ovde radi, ali ne treba ti)

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

int zivih = 0;

struct Kanal {
    explicit Kanal(int kanalId) : id(kanalId) {
        if (kanalId == 3) throw std::runtime_error("kanal 3 ne postoji");
        ++zivih;
    }
    ~Kanal() { --zivih; }
    Kanal(const Kanal& o) : id(o.id) { ++zivih; }
    Kanal& operator=(const Kanal&) = default;
    int id;
};

#ifdef NAIVNO
Kanal** otvoriSve(int n) {
    Kanal** k = new Kanal*[n];
    for (int i = 0; i < n; ++i) k[i] = new Kanal(i);   // i == 3: baca
    return k;
}

int main() {
    try {
        Kanal** k = otvoriSve(5);
        (void)k;
    } catch (const std::exception& e) {
        std::cout << "greška: " << e.what() << ", živih kanala: " << zivih << '\n';
    }
}
#else
// TODO korak 2 i 3

int main() {
    // Korak 2 -- otkomentariši:
    // try {
    //     auto k = otvoriSve(5);
    // } catch (const std::exception& e) {
    //     std::cout << "greška: " << e.what() << ", živih kanala: " << zivih << '\n';
    // }
    // auto tri = otvoriSve(3);
    // std::cout << "otvoreno: " << tri.size() << ", živih: " << zivih << '\n';

    // Korak 3 -- otkomentariši:
    // try {
    //     auto k = otvoriSveVrednost(5);
    // } catch (const std::exception& e) {
    //     std::cout << "po vrednosti: " << e.what() << ", živih kanala: " << zivih - 3 << '\n';
    // }
}
#endif

/* OČEKIVANI IZLAZ
greška: kanal 3 ne postoji, živih kanala: 0
otvoreno: 3, živih: 3
po vrednosti: kanal 3 ne postoji, živih kanala: 0
*/
