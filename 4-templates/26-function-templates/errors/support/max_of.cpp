#include "max_of.h"
// Definicija šablona u .cpp fajlu. Ovde se ne instancira ni za jedan tip,
// pa u max_of.o ne postoji nijedna funkcija maxOf<...>.
template <typename T>
T maxOf(T a, T b) { return b < a ? a : b; }
