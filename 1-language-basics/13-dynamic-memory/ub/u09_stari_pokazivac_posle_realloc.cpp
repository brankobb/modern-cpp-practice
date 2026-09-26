// EXPECT-UB: heap-use-after-free
// POGREŠNO: korišćenje starog pokazivača posle realloc-a.
// Zašto: kad realloc ne može da proširi blok na mestu, alocira novi, kopira
//   sadržaj i OSLOBODI stari, pa svi pokazivači na stari blok vise. Po
//   standardu je stari pokazivač nevažeći čak i kad je nova adresa ista.
//   Isto važi za std::vector posle push_back (lekcija 04, ub/u06).
//   g++ -Wall ovde upozorava (-Wuse-after-free).
// Ispravno: posle realloc-a koristi samo pokazivač koji je on vratio, ili
//   čuvaj indekse umesto pokazivača.
#include <cstdio>
#include <cstdlib>

int main() {
    int* a = static_cast<int*>(std::malloc(2 * sizeof(int)));
    a[0] = 1;
    int* first = a;
    int* grown = static_cast<int*>(std::realloc(a, 1000 * sizeof(int)));
    std::printf("%d\n", *first);
    std::free(grown);
}
