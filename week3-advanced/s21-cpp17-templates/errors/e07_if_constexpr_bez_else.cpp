// EXPECT-GCC: request for member 'size' in 'x', which is of non-class type 'const int'
// EXPECT-CLANG: member reference base type 'const int' is not a structure or union
// POGREŠNO: if constexpr odbacuje samo SVOJU granu. Kod POSLE if-a (bez
// else) nije deo grane i instancira se uvek -- i za int, gde x.size()
// ne postoji. Pri izvršavanju bi se za int ta linija ionako preskočila
// (return je pre nje), ali kompajler mora da je prevede.
// Ispravno: else { return x.size(); }
#include <string>
#include <type_traits>
template <typename T>
std::size_t velicina(const T& x) {
    if constexpr (std::is_integral_v<T>) return sizeof(x);
    return x.size();
}
int main() { return static_cast<int>(velicina(5) + velicina(std::string("ab"))); }
