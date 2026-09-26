// EXPECT-UB: attempting free on address which was not malloc\(\)-ed
// POGREŠNO: delete[] na pokazivaču koji pokazuje u sredinu niza.
// Zašto: delete[] mora dobiti TAČNO pokazivač koji je vratio new[]. Posle
//   aritmetike (a + 1, ++p u petlji) to više nije taj pokazivač. Česta
//   varijanta: pokazivač se pomera kroz niz, pa se na kraju obriše on
//   umesto originala. g++ -Wall ovde upozorava (-Wfree-nonheap-object).
// Ispravno: sačuvaj originalni pokazivač za delete[], a za prolaz koristi
//   drugi pokazivač ili indeks.
#include <cstdio>

int main() {
    int* a = new int[4]{1, 2, 3, 4};
    int* mid = a + 1;
    std::printf("%d\n", *mid);
    delete[] mid;
}
