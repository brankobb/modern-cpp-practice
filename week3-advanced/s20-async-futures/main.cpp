#include <chrono>
#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

// std::async, std::future, std::promise -- ISPRAVNI slučajevi.
// Sve se kompajlira bez upozorenja i radi bez ASan/UBSan i bez TSan
// prijava (./build.sh ... --tsan), g++ 13 i clang 18, C++17 i C++20.
// Brojevi sekcija prate notes.md. Zadaci ne pišu na cout (redosled nije
// određen); vrednosti se vraćaju kroz future.
// POGREŠNI slučajevi:
//   errors/   -- kod koji se NE kompajlira
//   ub/       -- data race između zadataka
//   runtime/  -- std::terminate: get dvaput, "broken promise", dvaput
//                set_value, izuzetak iz zadatka bez try
// ./check_cases.sh week3-advanced/s20-async-futures  proverava sve.

using namespace std::chrono_literals;

// ---------------------------------------------------------------- 1
long zbirOpsega(const std::vector<int>& v, std::size_t od, std::size_t doKraja) {
    return std::accumulate(v.begin() + static_cast<std::ptrdiff_t>(od),
                           v.begin() + static_cast<std::ptrdiff_t>(doKraja), 0L);
}

void sekcija1() {
    std::cout << "\n== 1. std::async vraća std::future\n";
    std::future<int> f = std::async([] { return 6 * 7; });
    std::cout << "valid pre get: " << f.valid() << '\n';
    std::cout << "get: " << f.get() << '\n';                // čeka rezultat
    std::cout << "valid posle get: " << f.valid() << '\n';  // rezultat je "potrošen" (runtime/r01)

    std::vector<int> v(1000);
    std::iota(v.begin(), v.end(), 1);
    std::vector<std::future<long>> delovi;
    for (std::size_t i = 0; i < 4; ++i)
        delovi.push_back(std::async(std::launch::async, zbirOpsega, std::cref(v), i * 250, (i + 1) * 250));
    long ukupno = 0;
    for (auto& d : delovi) ukupno += d.get();              // bez vector<long>, std::ref i join
    std::cout << "zbir 1..1000 u 4 zadatka: " << ukupno << '\n';
}

// ---------------------------------------------------------------- 2
void uvecaj(int& x) { ++x; }

struct Kalibrator {
    int pomak;
    int primeni(int x) const { return x + pomak; }
};

void sekcija2() {
    std::cout << "\n== 2. argumenti, metode, shared_future\n";
    int brojac = 0;
    std::async(std::launch::async, uvecaj, std::ref(brojac)).get();   // kao thread: kopira, osim std::ref
    std::cout << "posle async(uvecaj, std::ref(brojac)): " << brojac << '\n';

    Kalibrator k{5};
    auto f = std::async(&Kalibrator::primeni, &k, 10);
    std::cout << "metoda: " << f.get() << '\n';

    // future: jedan čitalac, jedan get. shared_future: kopira se, get koliko god puta.
    std::shared_future<int> deljen = std::async([] { return 100; }).share();
    std::vector<std::future<int>> citaoci;
    for (int i = 1; i <= 3; ++i)
        citaoci.push_back(std::async(std::launch::async, [deljen, i] { return deljen.get() + i; }));
    std::cout << "3 čitaoca istog shared_future:";
    for (auto& c : citaoci) std::cout << ' ' << c.get();
    std::cout << '\n';
}

// ---------------------------------------------------------------- 3
void sekcija3() {
    std::cout << "\n== 3. launch politike\n";
    const auto glavna = std::this_thread::get_id();

    auto a = std::async(std::launch::async, [] { return std::this_thread::get_id(); });
    std::cout << "launch::async u drugoj niti: " << (a.get() != glavna) << '\n';

    bool pokrenut = false;
    auto d = std::async(std::launch::deferred, [&pokrenut] {
        pokrenut = true;
        return std::this_thread::get_id();
    });
    std::cout << "deferred pokrenut pre get: " << pokrenut << '\n';
    std::cout << "deferred u niti koja zove get: " << (d.get() == glavna) << '\n';
    std::cout << "deferred pokrenut posle get: " << pokrenut << '\n';

    bool nikad = false;
    {
        auto n = std::async(std::launch::deferred, [&nikad] { nikad = true; });
    }   // niko nije pozvao get/wait -- zadatak se nikad ne izvrši
    std::cout << "deferred bez get ikad pokrenut: " << nikad << '\n';
}

// ---------------------------------------------------------------- 4
const char* status(std::future_status s) {
    switch (s) {
        case std::future_status::ready: return "ready";
        case std::future_status::timeout: return "timeout";
        case std::future_status::deferred: return "deferred";
    }
    return "?";
}

void sekcija4() {
    std::cout << "\n== 4. wait, wait_for, wait_until\n";
    std::promise<void> kreni;
    std::shared_future<void> signal = kreni.get_future().share();
    auto posao = std::async(std::launch::async, [signal] {
        signal.wait();                                     // blokiran dok main ne da signal
        return 1;
    });
    std::cout << "wait_for(10ms) dok čeka signal: " << status(posao.wait_for(10ms)) << '\n';
    std::cout << "wait_until(sada + 10ms): " << status(posao.wait_until(std::chrono::steady_clock::now() + 10ms))
              << '\n';
    kreni.set_value();
    posao.wait();                                          // čeka bez uzimanja rezultata
    std::cout << "posle wait: " << status(posao.wait_for(0ms)) << ", get = " << posao.get() << '\n';

    auto odlozen = std::async(std::launch::deferred, [] { return 2; });
    std::cout << "wait_for na deferred: " << status(odlozen.wait_for(0ms)) << '\n';

    // Destruktor future-a iz std::async čeka kraj zadatka (EMC Item 38).
    bool gotov = false;
    {
        auto f = std::async(std::launch::async, [&gotov] {
            std::this_thread::sleep_for(20ms);
            gotov = true;
        });
    }   // f se uništi ovde -- i čeka, iako niko nije pozvao get
    std::cout << "posle uništenja future-a zadatak je gotov: " << gotov << '\n';
}

// ---------------------------------------------------------------- 5
void sekcija5() {
    std::cout << "\n== 5. std::promise\n";
    std::promise<int> obecanje;
    std::future<int> rezultat = obecanje.get_future();
    std::thread t([&obecanje] { obecanje.set_value(21); });   // vrednost iz OBIČNE niti
    std::cout << "promise -> future: " << rezultat.get() << '\n';
    t.join();

    std::future<int> napusten;
    {
        std::promise<int> p;
        napusten = p.get_future();
    }   // promise uništen bez set_value
    try {
        napusten.get();
    } catch (const std::future_error& e) {
        std::cout << "future_error, broken_promise: " << (e.code() == std::future_errc::broken_promise) << '\n';
    }

    // packaged_task: funkcija + promise u jednom -- poziv upiše rezultat u future.
    std::packaged_task<int(int, int)> zadatak([](int a, int b) { return a * b; });
    std::future<int> proizvod = zadatak.get_future();
    std::thread radnik(std::move(zadatak), 6, 7);
    std::cout << "packaged_task u niti: " << proizvod.get() << '\n';
    radnik.join();
}

// ---------------------------------------------------------------- 6
int kalibrisi(int x) {
    if (x < 0) throw std::invalid_argument("negativna referenca: " + std::to_string(x));
    return x * 2;
}

void sekcija6() {
    std::cout << "\n== 6. izuzeci preko niti\n";
    auto f = std::async(std::launch::async, kalibrisi, -3);
    try {
        f.get();                                           // izuzetak iz zadatka se baci OVDE
    } catch (const std::invalid_argument& e) {
        std::cout << "uhvaćeno u main-u: " << e.what() << '\n';
    }

    std::promise<int> p;
    std::future<int> r = p.get_future();
    std::thread t([&p] {
        try {
            p.set_value(kalibrisi(-5));
        } catch (...) {
            p.set_exception(std::current_exception());     // exception_ptr (s11, sekcija 9)
        }
    });
    t.join();
    try {
        r.get();
    } catch (const std::exception& e) {
        std::cout << "iz promise-a: " << e.what() << '\n';
    }
}

int main() {
    std::cout << std::boolalpha;
    sekcija1();
    sekcija2();
    sekcija3();
    sekcija4();
    sekcija5();
    sekcija6();
}
