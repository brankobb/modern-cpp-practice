// Rešenje zadatka z3_redosled_globalnih.

#include <iostream>

int ucitajBazu() { return 0x4000; }

// Ako je baza globalna promenljiva (nije dobro): registar može da se
// izračuna pre nje i pročita 0 -- bez greške, samo pogrešna adresa.
// Treba ovako: funkcija sa static lokalnom (Meyers singleton). Vrednost se
// izračuna pri prvom pozivu, ma ko prvi pozvao; od C++11 i thread-safe.
int& baza() {
    static int b = ucitajBazu();
    return b;
}

int registar = baza() + 0x10;

// Možeš i ovako, kad je vrednost poznata pri kompajliranju: constexpr je
// statička inicijalizacija, pa redosled ne postoji.
constexpr int bazaKonst() { return 0x4000; }
constexpr int registar2 = bazaKonst() + 0x10;

int main() {
    std::cout << std::hex << "registar = 0x" << registar << '\n';
    std::cout << "registar2 = 0x" << registar2 << '\n';
}
