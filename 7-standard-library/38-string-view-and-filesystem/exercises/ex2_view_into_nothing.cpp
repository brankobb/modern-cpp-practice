// KIND: why
// DEMO-UB: NAIVE heap-use-after-free
//
// Zadatak 2 -- zašto string_view ne sme da bude član koji "čuva" ime (sekcija 3; ub/u01)
// Rešenje: exercises/solutions/ex2_view_into_nothing.cpp
//
// Channel pamti svoje ime. Ime se sastavlja u funkciji make().
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 7-standard-library/38-string-view-and-filesystem/exercises/ex2_view_into_nothing.cpp -DNAIVE
//   ASan: heap-use-after-free. Član ime je string_view -- pokazuje na
//   lokalni std::string iz napravi(), koji je uništen na izlasku iz
//   funkcije. Channel "ima ime" koje više ne postoji.
//   Bez ASan-a (npr. g++ -std=c++17 -DNAIVE ...) ispis je smeće:
//   nasumični bajtovi i komad imena drugog kanala. Šta se tačno vidi
//   zavisi od toga šta je u međuvremenu prepisalo tu memoriju.
// Korak 2: u #else grani napiši Channel i make() tako da Channel POSEDUJE
//   ime. Pravilo: string_view za parametre i kratkotrajne poglede, nikad
//   za podatak koji objekat treba da čuva.

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#ifdef NAIVE
struct Channel {
    std::string_view name;
    int stamp;
};
Channel make(int stamp) {
    std::string name = "temperature_sensor_hall_" + std::to_string(stamp);
    return Channel{name, stamp};
}
#else
// TODO korak 2 (dok ne napišeš, sva imena su "?")
struct Channel {
    std::string_view name;
    int stamp;
};
Channel make(int stamp) { return Channel{"?", stamp}; }
#endif

int main() {
    std::vector<Channel> channels;
    for (int i = 1; i <= 2; ++i) channels.push_back(make(i));
    for (const auto& k : channels) std::cout << k.stamp << ": " << k.name << '\n';
}

/* EXPECTED OUTPUT
1: temperature_sensor_hall_1
2: temperature_sensor_hall_2
*/
