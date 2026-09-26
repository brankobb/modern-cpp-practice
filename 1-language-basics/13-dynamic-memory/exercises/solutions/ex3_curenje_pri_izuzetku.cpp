// Rešenje zadatka ex3_curenje_pri_izuzetku.

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

// Ako radiš "Kanal** k = new Kanal*[n]; k[i] = new Kanal(i);" (nije
// dobro): izuzetak u sredini petlje ostavi niz i sve već napravljene
// kanale bez vlasnika -- curenje, a destruktori se ne pozovu (ako Kanal
// drži hardverski resurs, i on ostane zauzet).
// Treba ovako: svaki resurs odmah dobije vlasnika (unique_ptr), a vlasnici
// su u vektoru. Izuzetak -> vektor se uništi -> sve se oslobodi.
std::vector<std::unique_ptr<Kanal>> otvoriSve(int n) {
    std::vector<std::unique_ptr<Kanal>> k;
    for (int i = 0; i < n; ++i) k.push_back(std::make_unique<Kanal>(i));
    return k;
}

// Možeš i ovako, kad objekti ne moraju imati stabilne adrese: sami
// objekti u vektoru. reserve -> nema premeštanja pri rastu.
std::vector<Kanal> otvoriSveVrednost(int n) {
    std::vector<Kanal> k;
    k.reserve(static_cast<std::size_t>(n));
    for (int i = 0; i < n; ++i) k.emplace_back(i);
    return k;
}

int main() {
    try {
        auto k = otvoriSve(5);
    } catch (const std::exception& e) {
        std::cout << "greška: " << e.what() << ", živih kanala: " << zivih << '\n';
    }
    auto tri = otvoriSve(3);
    std::cout << "otvoreno: " << tri.size() << ", živih: " << zivih << '\n';

    try {
        auto k = otvoriSveVrednost(5);
    } catch (const std::exception& e) {
        std::cout << "po vrednosti: " << e.what() << ", živih kanala: " << zivih - 3 << '\n';
    }
}
