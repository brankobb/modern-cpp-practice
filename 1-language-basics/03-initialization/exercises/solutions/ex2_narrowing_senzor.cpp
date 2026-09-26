// Rešenje zadatka ex2_narrowing_senzor.

#include <iostream>

void prikazi(unsigned char nivo) { std::cout << "nivo = " << int(nivo) << '\n'; }

// Ako radiš "unsigned char nivo = sirovo;" (nije dobro): 300 tiho postane
// 44 i displej pokaže pogrešan nivo, bez ikakvog upozorenja.
// {} odbija da prevede taj kod, pa moraš da odlučiš šta konverzija znači.
// Ovde: prvo ograniči opseg, pa skaliraj, pa tek onda eksplicitno konvertuj.
unsigned char uNivo(int sirovo) {
    if (sirovo < 0) sirovo = 0;
    if (sirovo > 1023) sirovo = 1023;
    return static_cast<unsigned char>(sirovo / 4);   // 0..255: staje sigurno
}

int main() {
    for (int s : {300, 1023, 2000, -5})
        prikazi(uNivo(s));
}
