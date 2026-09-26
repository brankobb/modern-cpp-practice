// EXPECT-UB: heap-buffer-overflow
// POGREŠNO: v.erase(it) u petlji, pa ++it na obrisanom iteratoru.
// Zašto: erase invalidira it i sve iteratore posle njega. Petlja nastavi sa
//   nevažećim iteratorom; kad se obriše poslednji element, it preskoči
//   end() i čita van niza.
// Ispravno: it = v.erase(it); (i ++it samo kad ništa nije obrisano), ili
//   erase-remove idiom, ili C++20 std::erase_if(v, pred) (main.cpp, sekcija 7).
#include <cstdio>
#include <vector>

int main() {
    std::vector<int> v{1, 2, 3, 4, 5, 6};
    for (auto it = v.begin(); it != v.end(); ++it) {
        if (*it % 2 == 0) v.erase(it);
    }
    std::printf("%zu\n", v.size());
}
