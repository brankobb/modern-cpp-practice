// Završna vežba dela 8 -- pipeline merenja
// SANITIZER: thread
//   ./build.sh 8-concurrency/capstone/task.cpp
//   ./build.sh 8-concurrency/capstone/task.cpp --tsan
// Uputstvo, spisak lekcija i nova tema (std::condition_variable):
// 8-concurrency/capstone/notes.md
// Rešenje: solution.cpp (otvori tek kad tvoj izlaz bude isti kao blok
// EXPECTED OUTPUT na dnu fajla).
//
// Fajl se kompajlira i ovakav. Piši redom; posle svakog koraka
// otkomentariši njegov deo main()-a. Pokreni i sa --tsan: ThreadSanitizer
// ne sme ništa da prijavi, ni posle više pokretanja.
//
// Korak 1: template <typename T> class SafeQueue (notes.md, "Nova tema")
//   -- članovi: std::mutex, std::condition_variable, std::deque<T>,
//   bool closed_.
//   -- void send(T x): pod lock_guard-om dodaj na kraj; ako je red
//   zatvoren, baci std::logic_error("slanje u zatvoren red"); notify_one
//   posle otključavanja (lekcija 39, sekcije 5 i 6).
//   -- std::optional<T> receive(): unique_lock i wait SA PREDIKATOM (ima
//   nešto ili je zatvoren); prazan optional znači "zatvoren i prazan, nema
//   više posla" (lekcija 37, sekcija 1).
//   -- void close(): postavi closed_ pod mutex-om, pa notify_all.
// Korak 2: proizvođač i potrošač
//   -- int producer(SafeQueue<Reading>&, int sensor, int n,
//   int failAfter = -1): šalje {sensor, sensor * 100 + i} za i = 0..n-1;
//   kad je i == failAfter, baci std::runtime_error("sensor S: no
//   response"); vraća n.
//   -- std::map<int, Sum> consumer(SafeQueue<Reading>&): prima dok
//   receive() ne vrati prazan optional; svaki potrošač ima SVOJU mapu, bez
//   deljenja i bez mutex-a (lekcija 39, sekcije 2 i 4).
// Korak 3: Result run(int consumers, sensors, int failingSensor,
// int failAfter)
//   -- potrošači i proizvođači preko std::async(std::launch::async, ...),
//   red se prosleđuje sa std::ref (lekcija 40, sekcija 1).
//   -- class CloseAtEnd: RAII, zatvara red u destruktoru (lekcija 21,
//   sekcija 1); drži ga u bloku oko proizvođača, pa se red zatvori čim su
//   svi proizvođači gotovi -- i kad neki baci.
//   -- izuzetak proizvođača stiže kroz get(): uhvati ga i dodaj poruku u
//   errors (lekcija 40, sekcija 6); rezultate potrošača spoji sa mergeInto().
// Korak 4: isto, ali senzor 2 otkaže posle 50 merenja -- ostali se
// obrade do kraja, a program se ne zaglavi.

#include <condition_variable>
#include <deque>
#include <future>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

struct Reading {
    int sensor;
    long value;
};

struct Sum {
    int n = 0;
    long total = 0;
};

void mergeInto(std::map<int, Sum>& overall, const std::map<int, Sum>& part) {
    for (const auto& [sensor, z] : part) {
        overall[sensor].n += z.n;
        overall[sensor].total += z.total;
    }
}

struct Result {
    std::map<int, Sum> bySensor;
    std::vector<std::string> errors;
};

void print(const Result& r) {
    int overall = 0;
    for (const auto& [sensor, z] : r.bySensor) {
        std::cout << "  sensor " << sensor << ": " << z.n << " readings, total " << z.total << '\n';
        overall += z.n;
    }
    std::cout << "  total " << overall << " readings";
    for (const auto& g : r.errors) std::cout << "; error: " << g;
    std::cout << '\n';
}

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // std::cout << std::boolalpha << "== step 1: queue in a single thread\n";
    // SafeQueue<Reading> queue;
    // queue.send({1, 10});
    // queue.send({2, 20});
    // queue.close();
    // const auto a = queue.receive();
    // const auto b = queue.receive();
    // const auto c = queue.receive();
    // std::cout << "received " << a->value << ", " << b->value << ", empty after closing: " << !c.has_value()
    //           << '\n';
    // try {
    //     queue.send({3, 30});
    // } catch (const std::logic_error& e) {
    //     std::cout << "sending after closing: " << e.what() << '\n';
    // }

    // Korak 2 -- otkomentariši:
    // std::cout << "== step 2: one producer, one consumer (std::thread)\n";
    // SafeQueue<Reading> r2;
    // std::map<int, Sum> sums2;
    // std::thread cons([&] { sums2 = consumer(r2); });
    // std::thread prod([&] {
    //     producer(r2, 7, 1000);
    //     r2.close();
    // });
    // prod.join();
    // cons.join();
    // std::cout << "sensor 7: " << sums2[7].n << " readings, total " << sums2[7].total << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << "== step 3: three producers, two consumers (std::async)\n";
    // print(run(2, {{1, 500}, {2, 300}, {3, 200}}, 0, -1));

    // Korak 4 -- otkomentariši:
    // std::cout << "== step 4: sensor 2 fails after 50 readings\n";
    // print(run(3, {{1, 500}, {2, 300}, {3, 200}}, 2, 50));
}

/* EXPECTED OUTPUT
== step 1: queue in a single thread
received 10, 20, empty after closing: true
sending after closing: sending to a closed queue
== step 2: one producer, one consumer (std::thread)
sensor 7: 1000 readings, total 1199500
== step 3: three producers, two consumers (std::async)
  sensor 1: 500 readings, total 174750
  sensor 2: 300 readings, total 104850
  sensor 3: 200 readings, total 79900
  total 1000 readings
== step 4: sensor 2 fails after 50 readings
  sensor 1: 500 readings, total 174750
  sensor 2: 50 readings, total 11225
  sensor 3: 200 readings, total 79900
  total 750 readings; error: sensor 2: no response
*/
