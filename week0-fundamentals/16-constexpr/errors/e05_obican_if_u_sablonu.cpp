// STD: c++17
// EXPECT-GCC: request for member 'size' in 'value', which is of non-class type 'const int'
// EXPECT-CLANG: member reference base type 'const int' is not a structure or union
// POGREŠNO: običan if umesto if constexpr u šablonu.
// Zašto: kod običnog if-a OBE grane moraju da se kompajliraju za svaki T,
//   iako se jedna nikad ne izvrši. Za T = int grana value.size() nije
//   ispravan kod. if constexpr odbaci granu koja ne važi za dati T, pa se
//   ona ni ne kompajlira.
// Ispravno: if constexpr (std::is_same_v<T, std::string>) (main.cpp, sekcija 6).
#include <string>
#include <type_traits>

template <typename T>
std::size_t length(const T& value) {
    if (std::is_same_v<T, std::string>)
        return value.size();
    else
        return 1;
}

int main() {
    return static_cast<int>(length(std::string("abc")) + length(42));
}
