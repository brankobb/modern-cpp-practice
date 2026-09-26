// EXPECT-GCC: 'class std::deque<int>' has no member named 'data'
// EXPECT-CLANG: no member named 'data' in 'std::deque<int>'
// POGREŠNO: deque nije JEDAN blok memorije -- elementi su u više blokova
// (zato je push_front brz i zato reference ne "vise" pri rastu). Pokazivač
// na "sve elemente redom" ne postoji.
// Ispravno: vector (ili array) kad treba neprekidna memorija, npr. za C API
// ili DMA bafer: v.data().
#include <deque>
int main() {
    std::deque<int> d{1, 2};
    return *d.data();
}
