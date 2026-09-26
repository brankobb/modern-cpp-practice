// FLAGS: -D_GLIBCXX_SANITIZE_VECTOR
// EXPECT-UB: container-overflow
// POGREŠNO: reserve(10) pa v[0] = 42, kao da elementi postoje.
// Zašto: reserve menja samo KAPACITET; size() ostaje 0 i elemenata nema.
//   v[0] upisuje u alociranu, ali "neiskorišćenu" memoriju -- UB.
//   ⚠️ Običan ASan to NE vidi (test: program ispiše "42 size=0"), jer je
//   memorija alocirana. Hvata ga tek:
//     -D_GLIBCXX_SANITIZE_VECTOR  -> ASan "container-overflow" (ovaj fajl)
//     -D_GLIBCXX_ASSERTIONS       -> assertion u operator[] (__n < this->size())
// Ispravno: resize(10) ako elementi treba da postoje, ili push_back /
//   emplace_back posle reserve.
#include <cstdio>
#include <vector>

int main() {
    std::vector<int> v;
    v.reserve(10);
    v[0] = 42;
    std::printf("%d size=%zu\n", v[0], v.size());
}
