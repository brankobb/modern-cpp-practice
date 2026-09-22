// STD: c++17
// EXPECT-GCC: should have been declared inside 'util'
// EXPECT-CLANG: out-of-line definition of 'b' does not match any declaration in namespace 'util'
// POGREŠNO: definicija util::b() van namespace-a, a b() nije deklarisan u util.
// Zašto: kvalifikovana definicija (void util::b() {...}) sme samo da DEFINIŠE
//   ime koje je već deklarisano u tom namespace-u. Ne može da ga uvede. Ovo
//   je zaštita od greške u kucanju: da je pravilo drugačije, "util::ad" umesto
//   "util::add" bi tiho napravio novu funkciju.
// Ispravno: deklaracija u namespace-u (u header-u), pa definicija, ili
//   definicija unutar "namespace util { ... }".
namespace util { void a(); }

void util::b() {}

int main() {}
