// EXPECT-UB: heap-buffer-overflow
// POGREŠNO: word[i] sa i većim od size().
// Zašto: operator[] ne proverava granice (kao kod vector-a). Dozvoljen je
//   samo word[word.size()], koji vraća '\0'; sve iznad je UB.
// Ispravno: word.at(i) (baca std::out_of_range, main.cpp sekcija 3), ili
//   provera i < word.size() pre pristupa.
#include <cstdio>
#include <string>

int main() {
    std::string word(40, 'w');
    std::size_t i = word.size() + 8;
    std::printf("%c\n", word[i]);
}
