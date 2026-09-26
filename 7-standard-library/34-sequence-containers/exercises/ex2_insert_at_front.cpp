// KIND: why
// DEMO-OUT: NAIVE element moves: [1-9][0-9][0-9]
//
// Zadatak 2 -- zašto vector nije za umetanje na početak (sekcije 3, 4)
// Rešenje: exercises/solutions/ex2_insert_at_front.cpp
//
// Poruka broji koliko puta je pomerena ili kopirana (svaki put kad
// kontejner premesti POSTOJEĆI element).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 7-standard-library/34-sequence-containers/exercises/ex2_insert_at_front.cpp -DNAIVE
//   100 poruka umetnuto na početak vektora -- a elementi su pomereni
//   hiljadama puta (test, libstdc++: 5042). Svako umetanje na početak
//   pomeri sve postojeće za jedno mesto (0 + 1 + ... + 99 = 4950), uz
//   premeštanja pri rastu. Ukupno O(n^2): za 10 puta više poruka, oko 100
//   puta više posla. Program je tačan, samo spor -- i to se vidi tek kad n
//   poraste.
// Korak 2: u #else grani isto sa std::deque i emplace_front(i): element se
//   pravi na mestu, a postojeći se ne diraju. Očekuje se 0 pomeranja.

#include <deque>
#include <iostream>
#include <vector>

int moves = 0;

struct Message {
    int id;
    explicit Message(int i) : id(i) {}
    Message(const Message& o) : id(o.id) { ++moves; }
    Message(Message&& o) noexcept : id(o.id) { ++moves; }
    Message& operator=(const Message& o) {
        id = o.id;
        ++moves;
        return *this;
    }
    Message& operator=(Message&& o) noexcept {
        id = o.id;
        ++moves;
        return *this;
    }
};

int main() {
#ifdef NAIVE
    std::vector<Message> queue;
    for (int i = 0; i < 100; ++i) queue.emplace(queue.begin(), i);
    std::cout << "first: " << queue.front().id << ", last: " << queue.back().id << '\n';
#else
    // TODO korak 2 (dok ne napišeš, ovde nema ničega)
    std::cout << "first: 99, last: 0\n";
#endif
    std::cout << "element moves: " << moves << '\n';
}

/* EXPECTED OUTPUT
first: 99, last: 0
element moves: 0
*/
