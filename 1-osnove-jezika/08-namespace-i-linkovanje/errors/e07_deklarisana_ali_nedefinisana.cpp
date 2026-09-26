// STD: c++17
// EXPECT-GCC: undefined reference to `missing(int)'
// EXPECT-CLANG: undefined reference to `missing(int)'
// LINK: support/empty.cpp
// POGREŠNO: funkcija je deklarisana i pozvana, ali nigde nije definisana.
// Zašto: deklaracija je obećanje kompajleru da definicija postoji negde. Zato
//   se svaki .cpp kompajlira bez greške. Linker traži simbol missing(int) u
//   svim objektnim fajlovima i ne nalazi ga. U praksi: zaboravljen .cpp u
//   build-u, ili definicija sa malo drugačijim potpisom (missing(long)).
// Ispravno: definiši funkciju u tačno jednom .cpp i dodaj ga u build.
int missing(int);

int main() {
    return missing(1);
}
