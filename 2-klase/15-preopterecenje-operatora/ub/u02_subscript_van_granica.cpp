// EXPECT-UB: stack-buffer-overflow
// POGREŠNO: operator[] bez provere, pozvan sa indeksom 2 za niz od 2 elementa.
// Zašto: operator[] je obična funkcija; data_[i] van granica je UB kao i
//   kod običnog niza (lekcija 05). Konvencija (kao std::vector) je da []
//   ne proverava, pa je odgovornost na pozivaocu.
// Ispravno: indeks 0..1, ili funkcija at() koja proverava i baca
//   std::out_of_range (main.cpp, sekcija 7).
#include <cstdio>

class Vec2 {
public:
    double& operator[](int i) { return data_[i]; }

private:
    double data_[2] {};
};

int main() {
    Vec2 v;
    int i = 2;
    v[i] = 1.0;
    std::printf("%f\n", v[0]);
}
