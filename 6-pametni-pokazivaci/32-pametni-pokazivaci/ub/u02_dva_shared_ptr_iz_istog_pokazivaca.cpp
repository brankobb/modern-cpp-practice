// EXPECT-UB: attempting double-free
// POGREŠNO: dva nezavisna shared_ptr napravljena od istog sirovog pokazivača.
// Zašto: svaki shared_ptr(raw) pravi SVOJ kontrolni blok sa use_count = 1.
//   Ne znaju jedan za drugog, pa oba obrišu isti int. Isti bag nastaje sa
//   shared_ptr<T>(this) unutar klase (rešenje: enable_shared_from_this,
//   main.cpp sekcija 8).
// Ispravno: jedan shared_ptr, a ostali su njegove kopije
//   (auto second = first;), i to najbolje od std::make_shared -- tada
//   sirov pokazivač ni ne postoji.
#include <cstdio>
#include <memory>

int main() {
    int* raw = new int(7);
    std::shared_ptr<int> first(raw);
    std::shared_ptr<int> second(raw);
    std::printf("%ld %ld\n", first.use_count(), second.use_count());
}
