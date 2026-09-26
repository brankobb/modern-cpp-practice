// EXPECT-GCC: request for member 'size' in 't', which is of non-class type 'const int'
// EXPECT-CLANG: member reference base type 'const int' is not a structure or union
// POGREŠNO tek pri INSTANCIJACIJI: telo šablona se za izraze koji zavise
// od T proverava tek kad se šablon pozove sa konkretnim tipom (two-phase
// lookup). length() je ispravan šablon -- za std::string radi. Za int
// "t.size()" nema smisla, i greška se prijavi unutar šablona, a ne na
// mestu poziva (kod velikih šablona to su poruke od stotinu redova).
// Ispravno: pozovi samo sa tipom koji ima size(), ili ograniči šablon
// (static_assert, enable_if; C++20 concepts) da greška bude na mestu poziva.
#include <cstddef>
template <typename T>
std::size_t length(const T& t) { return t.size(); }
int main() { return static_cast<int>(length(5)); }
