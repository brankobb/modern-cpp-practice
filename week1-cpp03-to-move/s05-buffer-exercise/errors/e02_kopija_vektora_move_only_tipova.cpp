// STD: c++17
// EXPECT-GCC: result type must be constructible from input type
// EXPECT-CLANG: result type must be constructible from input type
// POGREŠNO: kopija std::vector<Buffer>, a Buffer je move-only (rule of 0
//   sa std::unique_ptr).
// Zašto: kopija vektora kopira svaki element, a Buffer nema copy
//   konstruktor (unique_ptr ga je obrisao, s02). Greška dolazi iz dubine
//   standardne biblioteke (static_assert u libstdc++), pa poruka ne pominje
//   liniju sa "= a" -- to je čest oblik ove greške.
// Ispravno: std::vector<Buffer> b = std::move(a); ili dodaj Buffer-u copy
//   konstruktor koji pravi duboku kopiju.
#include <memory>
#include <vector>

struct Buffer {
    std::unique_ptr<int[]> data;
};

int main() {
    std::vector<Buffer> a(2);
    std::vector<Buffer> b = a;
    return static_cast<int>(b.size());
}
