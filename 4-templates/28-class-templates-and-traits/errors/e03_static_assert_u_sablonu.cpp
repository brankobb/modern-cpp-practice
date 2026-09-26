// EXPECT-GCC: static assertion failed: Merenje<T>: T mora biti brojčani tip
// EXPECT-CLANG: static assertion failed due to requirement 'std::is_arithmetic_v<const char *>': Merenje<T>: T mora biti brojčani tip
// NAMERNA GREŠKA (kurs 150): static_assert u klasnom šablonu odbije tip
// za koji šablon nije napisan -- sa porukom koju si sam napisao, na
// početku izveštaja. Bez njega bi greška nastala tek duboko u nekoj
// metodi, ili (gore) šablon bi se kompajlirao i radio besmisleno
// (Merenje<const char*> bi poredio adrese).
// Ispravno: Merenje<double>, Merenje<int>... (main.cpp, sekcija 8).
#include <type_traits>
template <typename T>
class Merenje {
    static_assert(std::is_arithmetic_v<T>, "Merenje<T>: T mora biti brojčani tip");

public:
    explicit Merenje(T v) : v_(v) {}

private:
    T v_;
};
int main() {
    Merenje<const char*> m("21.5");
    (void)m;
}
