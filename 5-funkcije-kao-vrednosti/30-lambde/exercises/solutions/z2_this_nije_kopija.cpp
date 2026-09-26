// Rešenje zadatka z2_this_nije_kopija.

#include <iostream>

class Termostat {
public:
    explicit Termostat(int prag) : prag_(prag) {}
    void postaviPrag(int p) { prag_ = p; }
    int prag() const { return prag_; }

    // Ako u metodi pišeš [=] i koristiš član (nije dobro): zarobljen je
    // this, pa lambda čita TRENUTNI član -- i visi ako objekat nestane.
    // Treba ovako: kopiraj tačno ono što treba, init capture-om.
    auto napraviProveru() const {
        return [prag = prag_](int t) { return t < prag; };
    }
    // Možeš i ovako (C++17): [*this] -- kopija celog objekta u lambdi.

private:
    int prag_;
};

int main() {
    std::cout << std::boolalpha;
    Termostat ts(20);
    auto provera = ts.napraviProveru();
    std::cout << "provera napravljena sa pragom " << ts.prag() << '\n';
    ts.postaviPrag(10);
    std::cout << "prag sada " << ts.prag() << ", 15 ispod praga: " << provera(15) << '\n';
}
