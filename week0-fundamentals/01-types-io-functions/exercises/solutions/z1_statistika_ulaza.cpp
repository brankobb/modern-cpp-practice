// Rešenje zadatka z1_statistika_ulaza.

#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

// Korak 1: više rezultata -> struct (F.20), ne gomila izlaznih parametara.
struct Statistika {
    int ispravnih = 0;
    int gresaka = 0;
    long long zbir = 0;   // 2000000000 + 2000000000 ne staje u int (UB!)
    int min = 0;
    int max = 0;
};

// Korak 2: proveri svako čitanje. Neuspeh: failbit -> clear() -> preskoči
// loš token. Kraj ulaza: eof -> izlaz iz petlje.
Statistika saberi(std::istream& in) {
    Statistika s;
    int x = 0;
    while (true) {
        if (in >> x) {
            if (s.ispravnih == 0 || x < s.min) s.min = x;
            if (s.ispravnih == 0 || x > s.max) s.max = x;
            s.zbir += x;
            ++s.ispravnih;
        } else if (in.eof()) {
            break;
        } else {
            ++s.gresaka;
            in.clear();
            in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
        }
    }
    return s;
}

// Korak 3: setw važi samo za SLEDEĆI ispis, a left/right, fixed i
// setprecision ostaju -- zato ih na kraju vraćamo. setw broji bajtove, ne
// slova: "grešaka" (8 bajtova u UTF-8) bi dobilo jedan razmak manje.
void ispisi(const Statistika& s) {
    auto red = [](const char* naziv) -> std::ostream& {
        return std::cout << std::left << std::setw(12) << naziv << std::right << std::setw(12);
    };
    red("ispravnih") << s.ispravnih << '\n';
    red("neispravnih") << s.gresaka << '\n';
    red("zbir") << s.zbir << '\n';
    red("min") << s.min << '\n';
    red("max") << s.max << '\n';
    double prosek = s.ispravnih ? static_cast<double>(s.zbir) / s.ispravnih : 0.0;
    red("prosek") << std::fixed << std::setprecision(2) << prosek << '\n';
    std::cout.unsetf(std::ios::floatfield | std::ios::adjustfield);
    std::cout << std::setprecision(6);
}

int main() {
    std::istringstream ulaz("12 7 x -3 99999999999 2000000000 2000000000 abc 5");
    Statistika s = saberi(ulaz);
    ispisi(s);
    std::cout << 42 << " (format posle ispisa je vraćen)\n";
}
