// EXPECT-RUN: terminate called after throwing an instance of 'std::out_of_range'
// POGREŠNO: substr(pos) proverava pos i baca std::out_of_range ako je
// pos > size() (kao i std::string::substr). Ostale metode -- operator[],
// remove_prefix -- NE proveravaju.
// Ispravno: proveri pos <= sv.size() pre substr-a.
#include <string_view>
int main() {
    std::string_view sv = "temp";
    return static_cast<int>(sv.substr(10).size());
}
