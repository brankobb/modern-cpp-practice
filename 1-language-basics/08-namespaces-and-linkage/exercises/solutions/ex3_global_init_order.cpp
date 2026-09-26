// Rešenje zadatka ex3_global_init_order.

#include <iostream>

int loadBase() { return 0x4000; }

// Ako je baza globalna promenljiva (nije dobro): registar može da se
// izračuna pre nje i pročita 0 -- bez greške, samo pogrešna adresa.
// Treba ovako: funkcija sa static lokalnom (Meyers singleton). Vrednost se
// izračuna pri prvom pozivu, ma ko prvi pozvao; od C++11 i thread-safe.
int& base() {
    static int b = loadBase();
    return b;
}

int regAddr = base() + 0x10;

// Možeš i ovako, kad je vrednost poznata pri kompajliranju: constexpr je
// statička inicijalizacija, pa redosled ne postoji.
constexpr int baseConst() { return 0x4000; }
constexpr int regAddr2 = baseConst() + 0x10;

int main() {
    std::cout << std::hex << "regAddr = 0x" << regAddr << '\n';
    std::cout << "regAddr2 = 0x" << regAddr2 << '\n';
}
