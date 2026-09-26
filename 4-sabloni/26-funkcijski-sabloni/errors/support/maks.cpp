#include "maks.h"
// Definicija šablona u .cpp fajlu. Ovde se ne instancira ni za jedan tip,
// pa u maks.o ne postoji nijedna funkcija maks<...>.
template <typename T>
T maks(T a, T b) { return b < a ? a : b; }
