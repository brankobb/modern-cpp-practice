// EXPECT-UB: load of misaligned address
// POGREŠNO: reinterpret_cast<int*> na adresu koja nije poravnata za int.
// Zašto: int mora da počne na adresi deljivoj sa alignof(int) (obično 4).
//   buffer + 1 to nije. Na x86 to "radi" (sporije), a na ARM-u i drugim
//   arhitekturama može da sruši program. reinterpret_cast ništa ne proverava.
//   Uz to, čitanje unsigned char niza kao int krši i strict aliasing.
// Ispravno: std::memcpy(&value, buffer + 1, sizeof value); -- kopira
//   bajtove bez pretpostavki o poravnanju i tipu.
#include <cstdio>
#include <cstring>

int main() {
    alignas(int) unsigned char buffer[8];
    std::memset(buffer, 0, sizeof buffer);
    int* p = reinterpret_cast<int*>(buffer + 1);
    std::printf("%d\n", *p);
}
