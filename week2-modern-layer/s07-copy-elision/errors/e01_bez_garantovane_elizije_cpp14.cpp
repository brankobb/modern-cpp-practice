// STD: c++14
// EXPECT-GCC: use of deleted function 'Guarded::Guarded(Guarded&&)'
// EXPECT-CLANG: call to implicitly-deleted copy constructor of 'Guarded'
// POGREŠNO (u C++14): vraćanje po vrednosti tipa koji nema ni kopiju ni move.
// Zašto: do C++14 "return Guarded{};" je formalno pravio privremeni objekat
//   i kopirao/pomerao ga u rezultat. Kompajler je smeo da izostavi kopiju,
//   ali je konstruktor MORAO da postoji. C++17 (P0135) menja pravila:
//   prvalue nije objekat dok ga ne materijalizuješ, pa kopije nema ni
//   formalno. Isti fajl se u C++17 kompajlira (main.cpp, sekcija 3).
// Ispravno: C++17 ili novije; u C++14 vrati std::unique_ptr<Guarded>.
#include <mutex>

struct Guarded {
    std::mutex m;
    int value = 0;
};

Guarded make() { return Guarded{}; }

int main() {
    Guarded g = make();
    return g.value;
}
