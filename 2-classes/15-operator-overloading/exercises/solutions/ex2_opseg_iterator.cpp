// Rešenje zadatka ex2_opseg_iterator.

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

class Opseg {
public:
    Opseg(int pocetak, int kraj) : pocetak_(pocetak), kraj_(kraj) {}

    // Korak 1: minimalni iterator za range-for.
    class Iterator {
    public:
        explicit Iterator(int i) : i_(i) {}
        int operator*() const { return i_; }
        Iterator& operator++() {        // prefiks: uveća, vrati sebe
            ++i_;
            return *this;
        }
        Iterator operator++(int) {      // postfiks: kopija stare vrednosti, pa prefiks
            Iterator stari = *this;
            ++*this;
            return stari;
        }
        friend bool operator==(const Iterator& a, const Iterator& b) { return a.i_ == b.i_; }
        friend bool operator!=(const Iterator& a, const Iterator& b) { return !(a == b); }

    private:
        int i_;
    };

    Iterator begin() const { return Iterator(pocetak_); }
    Iterator end() const { return Iterator(kraj_); }

    // Korak 2: samo const verzija -- elementi se računaju, ne čuvaju, pa
    // nema šta da se menja preko reference.
    int operator[](int k) const {
        if (k < 0 || k >= kraj_ - pocetak_) throw std::out_of_range("Opseg::operator[]");
        return pocetak_ + k;
    }

private:
    int pocetak_;
    int kraj_;
};

// Korak 3: objekat koji se poziva kao funkcija, a nosi stanje (faktor).
class Skaliraj {
public:
    explicit Skaliraj(int faktor) : faktor_(faktor) {}
    int operator()(int x) const { return x * faktor_; }

private:
    int faktor_;
};

int main() {
    const char* sep = "";
    for (int x : Opseg(1, 5)) {
        std::cout << sep << x;
        sep = " ";
    }
    std::cout << '\n';
    Opseg o(1, 5);
    auto it = o.begin();
    int stara = *it++;
    std::cout << "it++ vraća staru: " << stara << ", sada: " << *it << '\n';

    std::cout << "Opseg(10, 20)[3] = " << Opseg(10, 20)[3] << '\n';
    try {
        Opseg(10, 20)[10];
    } catch (const std::out_of_range&) {
        std::cout << "[10]: out_of_range\n";
    }

    std::vector<int> v;
    for (int x : Opseg(1, 5)) v.push_back(x);
    std::transform(v.begin(), v.end(), v.begin(), Skaliraj{3});
    std::cout << "skalirano:";
    for (int x : v) std::cout << ' ' << x;
    std::cout << '\n';
}
