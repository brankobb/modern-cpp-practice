// Rešenje zadatka ex2_narrowing_sensor.

#include <iostream>

void show(unsigned char level) { std::cout << "level = " << int(level) << '\n'; }

// Ako radiš "unsigned char level = raw;" (nije dobro): 300 tiho postane
// 44 i displej pokaže pogrešan nivo, bez ikakvog upozorenja.
// {} odbija da prevede taj kod, pa moraš da odlučiš šta konverzija znači.
// Ovde: prvo ograniči opseg, pa skaliraj, pa tek onda eksplicitno konvertuj.
unsigned char toLevel(int raw) {
    if (raw < 0) raw = 0;
    if (raw > 1023) raw = 1023;
    return static_cast<unsigned char>(raw / 4);   // 0..255: staje sigurno
}

int main() {
    for (int s : {300, 1023, 2000, -5})
        show(toLevel(s));
}
