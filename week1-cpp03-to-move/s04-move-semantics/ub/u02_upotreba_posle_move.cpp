// EXPECT-UB: load of null pointer of type 'int'
// POGREŠNO: dereferenciranje unique_ptr-a iz kog je vlasništvo prebačeno.
// Zašto: posle std::move(owner) owner je nullptr (za unique_ptr je to
//   garantovano), pa je *owner čitanje preko null pokazivača. Kod drugih
//   tipova moved-from stanje je "ispravno ali nepoznato"; ni tada se na
//   sadržaj ne oslanja. Ni g++ ni clang sa -Wall ovo ne vide (clang-tidy ima
//   proveru bugprone-use-after-move).
// Ispravno: posle std::move koristi samo novog vlasnika; stari objekat
//   sme samo da dobije novu vrednost ili da bude uništen.
#include <cstdio>
#include <memory>
#include <utility>

int main() {
    auto owner = std::make_unique<int>(42);
    auto newOwner = std::move(owner);
    std::printf("%d\n", *owner);
}
