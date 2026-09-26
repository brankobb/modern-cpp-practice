// KIND: why
// DEMO-UB: NAIVE detected memory leaks
//
// Zadatak 3 -- zašto ručni new curi čim nešto baci izuzetak (sekcija 7, R.11)
// Rešenje: exercises/solutions/ex3_leak_on_exception.cpp
//
// openAll(n) otvara n kanala. Kanal 3 ne postoji i njegov konstruktor
// baci izuzetak.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/13-dynamic-memory/exercises/ex3_leak_on_exception.cpp -DNAIVE
//   Izuzetak je uhvaćen i program se "uredno" završi, ali na izlazu
//   LeakSanitizer (deo ASan-a na Linux-u) prijavi "detected memory leaks":
//   niz pokazivača i kanali 0, 1, 2 nisu oslobođeni. Kad izuzetak izleti
//   iz openAll, niko više nema pokazivač na njih. Brojač živih kanala
//   pokazuje isto: destruktori se nisu pozvali.
//   (Na Windows-u ASan ne prijavljuje curenje -- tamo gledaj brojač.
//   Ako izlaz preusmeriš u fajl ili pipe, red "error: ..." može da
//   nestane: LeakSanitizer završi program pre nego što se isprazni bafer
//   std::cout-a. U terminalu se vidi.)
// Korak 2: u #else grani napiši openAll tako da vraća
//   std::vector<std::unique_ptr<Channel>>. Kad izuzetak izleti usred petlje,
//   vektor se uništi, a sa njim i svi već otvoreni kanali.
// Korak 3: probaj i bez pokazivača: std::vector<Channel> sa reserve(n) i
//   emplace_back(i). Zašto reserve? (bez njega bi rast vektora
//   premeštao Channel-e -- ovde radi, ali ne treba ti)

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

int alive = 0;

struct Channel {
    explicit Channel(int channelId) : id(channelId) {
        if (channelId == 3) throw std::runtime_error("channel 3 does not exist");
        ++alive;
    }
    ~Channel() { --alive; }
    Channel(const Channel& o) : id(o.id) { ++alive; }
    Channel& operator=(const Channel&) = default;
    int id;
};

#ifdef NAIVE
Channel** openAll(int n) {
    Channel** c = new Channel*[n];
    for (int i = 0; i < n; ++i) c[i] = new Channel(i);   // i == 3: baca
    return c;
}

int main() {
    try {
        Channel** c = openAll(5);
        (void)c;
    } catch (const std::exception& e) {
        std::cout << "error: " << e.what() << ", channels alive: " << alive << '\n';
    }
}
#else
// TODO korak 2 i 3

int main() {
    // Korak 2 -- otkomentariši:
    // try {
    //     auto c = openAll(5);
    // } catch (const std::exception& e) {
    //     std::cout << "error: " << e.what() << ", channels alive: " << alive << '\n';
    // }
    // auto three = openAll(3);
    // std::cout << "opened: " << three.size() << ", alive: " << alive << '\n';

    // Korak 3 -- otkomentariši:
    // try {
    //     auto c = openAllByValue(5);
    // } catch (const std::exception& e) {
    //     std::cout << "by value: " << e.what() << ", channels alive: " << alive - 3 << '\n';
    // }
}
#endif

/* EXPECTED OUTPUT
error: channel 3 does not exist, channels alive: 0
opened: 3, alive: 3
by value: channel 3 does not exist, channels alive: 0
*/
