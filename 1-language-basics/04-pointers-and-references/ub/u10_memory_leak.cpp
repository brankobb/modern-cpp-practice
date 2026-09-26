// EXPECT-UB: detected memory leaks
// Greška (nije UB, ali je bag): curenje memorije -- new bez delete.
// Svaki poziv processOne() alocira bafer i "zaboravi" ga kad funkcija vrati;
// u petlji se to sabira. LeakSanitizer to prijavi na izlazu programa.
// Napomena: LSan je KONZERVATIVAN -- ako vrednost pokazivača slučajno ostane
// negde u memoriji, taj objekat ne prijavi (zato petlja, a ne jedan poziv).
// Na Windows-u LeakSanitizer ne postoji, pa se ovo tamo neće prijaviti.
// Ispravno: auto buffer = std::make_unique<int[]>(100);  -- oslobađa se sam
#include <iostream>
int processOne(int i) {
    int* buffer = new int[100];
    buffer[0] = i;
    return buffer[0];
}
int main() {
    int sum = 0;
    for (int i = 0; i < 10; ++i) sum += processOne(i);
    std::cout << sum << "\n";
}
