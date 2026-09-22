#include <iostream>

// Namerni bug: off-by-one upis van granica niza.
// NE gledaj odmah kod -- kompajliraj ručno sa -g -fsanitize=address,
// pokreni pod gdb, i:
//   1) uhvati ASan izveštaj -- na kojoj liniji je overflow?
//   2) postavi breakpoint na tu liniju, `watch i` da vidiš kad i dostigne
//      problematičnu vrednost
//   3) `bt` da vidiš call stack u trenutku pucanja

void fillBuffer(int* buf, int size) {
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
