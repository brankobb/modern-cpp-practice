// STD: c++20
// EXPECT-GCC: 'argc' is not a constant expression
// EXPECT-CLANG: call to consteval function 'square' is not a constant expression
// POGREŠNO: consteval funkcija pozvana sa vrednošću poznatom tek pri izvršavanju.
// Zašto: consteval ("immediate function") MORA da se izračuna pri
//   kompajliranju -- za razliku od constexpr, koja SME. argc se zna tek kad
//   se program pokrene.
// Ispravno: constexpr umesto consteval ako funkcija treba i u runtime-u;
//   consteval kad je poziv sa runtime vrednošću greška u dizajnu (npr.
//   provera formata stringa pri kompajliranju).
consteval int square(int x) { return x * x; }

int main(int argc, char**) {
    return square(argc);
}
