// EXPECT-GCC: static assertion failed: Measurement<T>: T must be an arithmetic type
// EXPECT-CLANG: static assertion failed due to requirement 'std::is_arithmetic_v<const char *>': Measurement<T>: T must be an arithmetic type
// NAMERNA GREŠKA (kurs 150): static_assert u klasnom šablonu odbije tip
// za koji šablon nije napisan -- sa porukom koju si sam napisao, na
// početku izveštaja. Bez njega bi greška nastala tek duboko u nekoj
// metodi, ili (gore) šablon bi se kompajlirao i radio besmisleno
// (Measurement<const char*> bi poredio adrese).
// Ispravno: Measurement<double>, Measurement<int>... (main.cpp, sekcija 8).
#include <type_traits>
template <typename T>
class Measurement {
    static_assert(std::is_arithmetic_v<T>, "Measurement<T>: T must be an arithmetic type");

public:
    explicit Measurement(T v) : v_(v) {}

private:
    T v_;
};
int main() {
    Measurement<const char*> m("21.5");
    (void)m;
}
