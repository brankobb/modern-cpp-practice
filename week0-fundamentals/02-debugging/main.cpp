#include <iostream>

// Namerni bug: off-by-one upis van granica niza.
// NE gledaj odmah kod -- kompajliraj ručno sa -g -fsanitize=address,
// pokreni pod gdb, i:
//   1) uhvati ASan izveštaj -- na kojoj liniji je overflow?
//   2) postavi breakpoint na tu liniju, `watch i` da vidiš kad i dostigne
//      problematičnu vrednost
//   3) `bt` da vidiš call stack u trenutku pucanja

void fillBuffer(int* buf, int size) {
    // Ako koristiš i <= size u petlji koja pristupa buf[i] (NIJE DOBRO)
    // jer validni indeksi niza veličine size su 0..size-1 -- i == size je
    // JEDAN PREKO granice (off-by-one), pišeš u memoriju koja ne pripada
    // nizu.
    // Treba da koristiš i < size (striktno manje) kad iteriraš preko SVIH
    // elemenata niza -- ovo je jedna od najčešćih grešaka u C/C++ kodu.
    // Možeš i koristiti range-based for (for (int& x : arr)) ili
    // std::array/std::vector sa .size() kad god je moguće -- eliminiše
    // ovu klasu bagova u korenu jer nema ručnog indeksiranja.
    for (int i = 0; i <= size; ++i) { // BUG: treba < ne <=
        buf[i] = i * i;
    }
}

int main() {
    const int n = 5;
    int buffer[n];
    fillBuffer(buffer, n);

    for (int i = 0; i < n; ++i) {
        std::cout << buffer[i] << " ";
    }
    std::cout << "\n";
}
