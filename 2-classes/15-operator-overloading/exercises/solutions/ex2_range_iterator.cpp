// Rešenje zadatka ex2_range_iterator.

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

class Range {
public:
    Range(int first, int last) : first_(first), last_(last) {}

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
            Iterator old = *this;
            ++*this;
            return old;
        }
        friend bool operator==(const Iterator& a, const Iterator& b) { return a.i_ == b.i_; }
        friend bool operator!=(const Iterator& a, const Iterator& b) { return !(a == b); }

    private:
        int i_;
    };

    Iterator begin() const { return Iterator(first_); }
    Iterator end() const { return Iterator(last_); }

    // Korak 2: samo const verzija -- elementi se računaju, ne čuvaju, pa
    // nema šta da se menja preko reference.
    int operator[](int k) const {
        if (k < 0 || k >= last_ - first_) throw std::out_of_range("Range::operator[]");
        return first_ + k;
    }

private:
    int first_;
    int last_;
};

// Korak 3: objekat koji se poziva kao funkcija, a nosi stanje (faktor).
class Scale {
public:
    explicit Scale(int factor) : factor_(factor) {}
    int operator()(int x) const { return x * factor_; }

private:
    int factor_;
};

int main() {
    const char* sep = "";
    for (int x : Range(1, 5)) {
        std::cout << sep << x;
        sep = " ";
    }
    std::cout << '\n';
    Range r(1, 5);
    auto it = r.begin();
    int old = *it++;
    std::cout << "it++ returns the old value: " << old << ", now: " << *it << '\n';

    std::cout << "Range(10, 20)[3] = " << Range(10, 20)[3] << '\n';
    try {
        Range(10, 20)[10];
    } catch (const std::out_of_range&) {
        std::cout << "[10]: out_of_range\n";
    }

    std::vector<int> v;
    for (int x : Range(1, 5)) v.push_back(x);
    std::transform(v.begin(), v.end(), v.begin(), Scale{3});
    std::cout << "scaled:";
    for (int x : v) std::cout << ' ' << x;
    std::cout << '\n';
}
