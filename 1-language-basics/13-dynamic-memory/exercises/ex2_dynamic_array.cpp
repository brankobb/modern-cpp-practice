// KIND: usage
//
// Zadatak 2 -- sopstveni rastući niz: šta std::vector radi za tebe (sekcija 5)
//   ./build.sh 1-language-basics/13-dynamic-memory/exercises/ex2_dynamic_array.cpp
// Rešenje: exercises/solutions/ex2_dynamic_array.cpp
//
// class DynamicArray čuva int-ove u bloku sa new[]: pokazivač data_,
// broj elemenata size_ i kapacitet cap_.
// Korak 1: destruktor (delete[]), i zabrani kopiranje (= delete za copy
//   konstruktor i copy dodelu) -- kopija bi delila isti blok, pa bi ga oba
//   destruktora oslobodila (lekcija 14, ub/u02).
// Korak 2: void add(int x): ako je size_ == cap_, NOVI kapacitet je
//   cap_ ? 2 * cap_ : 1; alociraj novi blok, prepiši elemente, oslobodi
//   stari, pa upiši x. Redosled je bitan: stari blok se briše tek POSLE
//   kopiranja. Ispiši "grow: <stari> -> <novi>" pri svakom rastu.
// Korak 3: int at(std::size_t i) const (baca std::out_of_range van
//   opsega), std::size_t size() const, std::size_t capacity() const.

#include <cstddef>
#include <iostream>
#include <stdexcept>

class DynamicArray {
public:
    // TODO korak 1, 2, 3
};

int main() {
    // Korak 1-3 -- otkomentariši:
    // DynamicArray a;
    // for (int i = 1; i <= 10; ++i) a.add(i);
    // int sum = 0;
    // for (std::size_t i = 0; i < a.size(); ++i) sum += a.at(i);
    // std::cout << "size: " << a.size() << ", capacity: " << a.capacity()
    //           << ", sum: " << sum << '\n';
    // try {
    //     a.at(10);
    // } catch (const std::out_of_range&) {
    //     std::cout << "at(10): out_of_range\n";
    // }
}

/* EXPECTED OUTPUT
grow: 0 -> 1
grow: 1 -> 2
grow: 2 -> 4
grow: 4 -> 8
grow: 8 -> 16
size: 10, capacity: 16, sum: 55
at(10): out_of_range
*/
