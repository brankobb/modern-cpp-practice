// EXPECT-UB: constructor call on misaligned address
// POGREŠNO: placement new u niz bajtova koji nije poravnat za tip.
// Zašto: Reading ima double, pa mora da počne na adresi deljivoj sa 8. bytes
//   posle char tag-a počinje na neparnoj adresi. Na x86 to samo uspori
//   pristup; na ARM Cortex-M i drugim arhitekturama pristup double-u na
//   neporavnatoj adresi može da izazove hardverski izuzetak.
// Ispravno: alignas(Reading) unsigned char bytes[sizeof(Reading)];
//   (main.cpp, deo 3), ili union sa članom tipa T.
#include <cstdio>
#include <new>

struct Reading {
    double value = 1.5;
};

struct Slot {
    char tag;
    unsigned char bytes[sizeof(Reading) + 8];
};

Slot slot;

int main() {
    Reading* r = new (slot.bytes) Reading;
    std::printf("%f\n", r->value);
    r->~Reading();
}
