// EXPECT-UB: attempting free on address which was not malloc\(\)-ed
// POGREŠNO: delete na objektu napravljenom placement new-om u statičkom baferu.
// Zašto: delete radi dve stvari: pozove destruktor i OSLOBODI memoriju.
//   Memorija ovde nije sa heap-a (statički niz), pa oslobađanje nije
//   dozvoljeno. g++ -Wall ovo vidi i pri kompajliranju (-Wfree-nonheap-object).
// Ispravno: samo destruktor, ručno: s->~Sensor(); (main.cpp, deo 3).
#include <cstdio>
#include <new>

struct Sensor {
    int id = 3;
};

alignas(Sensor) unsigned char storage[sizeof(Sensor)];

int main() {
    Sensor* s = new (storage) Sensor;
    std::printf("%d\n", s->id);
    delete s;
}
