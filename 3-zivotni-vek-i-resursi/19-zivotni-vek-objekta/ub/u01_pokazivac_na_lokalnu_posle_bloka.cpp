// EXPECT-UB: stack-use-after-scope
// POGREŠNO: pokazivač na lokalnu promenljivu koristi se posle kraja njenog bloka.
// Zašto: value ima automatic trajanje: živi do }. Posle toga memorija na
//   steku postoji, ali objekta u njoj više nema ([basic.life]), i kompajler
//   sme da je iskoristi za nešto drugo. Pokazivač i dalje sadrži adresu, pa
//   greška ne izgleda kao greška.
// Ispravno: neka promenljiva živi koliko i pokazivač (deklariši je u
//   spoljnom bloku), ili kopiraj vrednost umesto adrese.
#include <cstdio>

int main() {
    int* p = nullptr;
    {
        int value = 42;
        p = &value;
    }
    std::printf("%d\n", *p);
}
