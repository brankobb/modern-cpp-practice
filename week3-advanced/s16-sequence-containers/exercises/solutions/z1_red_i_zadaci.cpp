// Rešenje zadatka z1_red_i_zadaci.

#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <utility>

// Korak 1: deque -- push_front i pop_front su O(1). Vector bi za svaku
// hitnu poruku i svaku obrađenu pomerao ceo niz (vector nema ni
// push_front ni pop_front, errors/e04).
class RedPoruka {
public:
    void primi(std::string p) { red_.push_back(std::move(p)); }
    void primiHitno(std::string p) { red_.push_front(std::move(p)); }
    bool obradi(std::string& izlaz) {
        if (red_.empty()) return false;
        izlaz = std::move(red_.front());
        red_.pop_front();
        return true;
    }

private:
    std::deque<std::string> red_;
};

// Korak 2: splice samo prevezuje čvor -- string se ne kopira ni ne pomera.
void naPocetak(std::list<std::string>& l, const std::string& ime) {
    auto it = std::find(l.begin(), l.end(), ime);
    if (it != l.end()) l.splice(l.begin(), l, it);
}

int main() {
    RedPoruka red;
    red.primi("temp 21");
    red.primi("temp 22");
    red.primiHitno("ALARM pritisak");
    std::string p;
    while (red.obradi(p)) std::cout << "obrađeno: " << p << '\n';

    std::list<std::string> zadaci{"kalibracija", "log", "backup", "update"};
    naPocetak(zadaci, "backup");
    std::cout << "zadaci:";
    for (const auto& z : zadaci) std::cout << ' ' << z;
    std::cout << '\n';

    // Korak 3: max_element vraća iterator na PRVI najveći; distance daje indeks.
    std::array<int, 7> poDanu{12, 30, 7, 30, 45, 3, 0};
    auto it = std::max_element(poDanu.begin(), poDanu.end());
    std::cout << "najprometniji dan: " << std::distance(poDanu.begin(), it) << " (" << *it << " poruka)\n";
}
