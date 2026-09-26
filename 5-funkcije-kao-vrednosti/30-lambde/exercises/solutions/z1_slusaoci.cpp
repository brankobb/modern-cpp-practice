// Rešenje zadatka z1_slusaoci.

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

// Korak 2a: obična funkcija -- bez stanja.
void loguj(int v) { std::cout << "log: " << v << '\n'; }

// Korak 2b: funkcijski objekat -- stanje (prag) u članu.
struct Alarm {
    int prag;
    void operator()(int v) const {
        if (v > prag) std::cout << "ALARM: " << v << " > " << prag << '\n';
    }
};

// Korak 1: std::function<void(int)> prima SVE što se može pozvati sa int:
// funkciju, objekat sa operator(), lambdu -- zato slušaoci mogu biti
// različitih vrsta u istom vektoru.
class Dogadjaj {
public:
    void pretplati(std::function<void(int)> f) { slusaoci_.push_back(std::move(f)); }
    void objavi(int v) const {
        for (const auto& f : slusaoci_) f(v);
    }

private:
    std::vector<std::function<void(int)>> slusaoci_;
};

int main() {
    Dogadjaj temp;
    int brojObjava = 0;
    std::vector<int> istorija;
    temp.pretplati(loguj);
    temp.pretplati(Alarm{10});
    // Korak 2c: po REFERENCI, jer lambda treba da menja promenljive
    // pozivaoca. Sa [brojObjava] bi menjala svoju kopiju (i tražila
    // mutable), a brojObjava u main-u bi ostao 0. Referenca je ovde
    // bezbedna, jer se objavi() poziva samo dok su brojObjava i istorija
    // žive. (Da temp nadživi te promenljive, reference bi visile -- z3.)
    temp.pretplati([&brojObjava](int) { ++brojObjava; });
    temp.pretplati([&istorija](int v) { istorija.push_back(v); });
    temp.objavi(5);
    temp.objavi(12);
    std::cout << "objava: " << brojObjava << ", istorija:";
    for (int v : istorija) std::cout << ' ' << v;
    std::cout << '\n';

    // Korak 3: prag po vrednosti -- lambda ga samo čita.
    int prag = 6;
    auto it = std::find_if(istorija.begin(), istorija.end(), [prag](int v) { return v > prag; });
    std::cout << "prvi iznad " << prag << ": " << (it != istorija.end() ? *it : -1) << '\n';
    std::cout << "parnih: " << std::count_if(istorija.begin(), istorija.end(), [](int v) { return v % 2 == 0; })
              << '\n';
}
