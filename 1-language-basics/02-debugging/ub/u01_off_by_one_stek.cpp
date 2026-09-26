// EXPECT-UB: stack-buffer-overflow
// BAG za vežbu (notes.md, sekcija 5): petlja "i <= n" upisuje JEDAN element
// iza kraja niza na steku. Bez ASan-a program često "radi" -- upis
// pregazi susednu memoriju i greška se vidi mnogo kasnije, ili nikad.
// Iz izveštaja pročitaj: vrsta (stack-buffer-overflow), WRITE of size 4
// (upis jednog int-a), red u popuni() (#0), i "[32, 52) 'niz' ... overflows
// this variable" -- koja promenljiva je pregažena.
// Ispravno: i < n. Još bolje: std::array / std::vector i range-for, pa
// indeksa nema (lekcija 05).
#include <iostream>

void popuni(int* buf, int n) {
    for (int i = 0; i <= n; ++i)
        buf[i] = i * i;
}

int main() {
    int niz[5];
    popuni(niz, 5);
    std::cout << niz[0] << '\n';
}
