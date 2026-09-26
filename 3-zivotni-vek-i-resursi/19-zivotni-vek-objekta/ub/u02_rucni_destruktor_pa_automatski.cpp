// EXPECT-UB: attempting double-free
// POGREŠNO: ručni poziv destruktora na automatskom objektu.
// Zašto: destruktor se za lokalni objekat poziva sam na kraju bloka. Ručni
//   poziv name.~basic_string() završi život objekta, a na } se destruktor
//   pozove još jednom, na objektu koji više ne postoji: string drugi put
//   oslobodi isti bafer.
// Ispravno: destruktor se poziva ručno SAMO za objekat napravljen placement
//   new-om (lekcija 13, sekcija 2), i tada tačno jednom. Za "isprazni sada":
//   name.clear() ili name = std::string{}.
#include <cstdio>
#include <string>

int main() {
    std::string name(100, 'x');
    name.~basic_string();
    std::printf("uništen ručno\n");
}
