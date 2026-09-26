#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <string>
#include <thread>
#include <utility>
#include <vector>

// Niti, mutex, lock_guard -- ISPRAVNI slučajevi.
// Sve se kompajlira bez upozorenja i radi bez ASan/UBSan i bez TSan
// prijava (./build.sh ... --tsan), g++ 13 i clang 18, C++17 i C++20.
// Brojevi sekcija prate notes.md. Niti NE pišu na cout: redosled bi
// zavisio od raspoređivača. Rezultate upisuju u promenljive, a main ih
// ispiše posle join().
// POGREŠNI slučajevi:
//   errors/   -- kod koji se NE kompajlira
//   ub/       -- data race, redosled zaključavanja, detach sa referencom
//   runtime/  -- std::terminate: nit bez join, izuzetak iz niti, dupli join
// ./check_cases.sh 8-concurrency/39-threads  proverava sve.

// ---------------------------------------------------------------- 1
void section1() {
    std::cout << "\n== 1. first thread\n";
    std::string message;
    std::thread t([&message] { message = "hello from another thread"; });
    t.join();                       // čeka kraj niti; tek POSLE ovoga je čitanje bezbedno
    std::cout << message << '\n';
}

// ---------------------------------------------------------------- 2
int functionResult = 0;
void plainFunction() { functionResult = 1; }

struct Functor {
    int* out;
    void operator()() const { *out = 3; }
};

struct Sensor {
    int value = 0;
    void read(int v) { value = v; }
};

void section2() {
    std::cout << "\n== 2. creating threads: function, lambda, functor, method\n";
    int fromLambda = 0, fromFunctor = 0;
    Sensor s;
    std::thread t1(plainFunction);
    std::thread t2([&fromLambda] { fromLambda = 2; });
    std::thread t3(Functor{&fromFunctor});
    std::thread t4(&Sensor::read, &s, 4);      // metoda: pokazivač na objekat, pa argumenti
    std::cout << "joinable before join: " << t1.joinable() << '\n';
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    std::cout << "joinable after join: " << t1.joinable() << '\n';
    std::cout << "results: " << functionResult << ' ' << fromLambda << ' ' << fromFunctor << ' ' << s.value
              << '\n';

    // std::thread se ne kopira, samo premešta (errors/e02).
    std::thread a([] {});
    std::thread b = std::move(a);
    std::cout << "after move: a.joinable() = " << a.joinable() << ", b.joinable() = " << b.joinable() << '\n';
    b.join();

    std::vector<std::thread> threads;
    std::vector<int> squares(4);
    for (int i = 0; i < 4; ++i)
        threads.emplace_back([&squares, i] { squares[static_cast<std::size_t>(i)] = i * i; });   // svaka nit svoj element
    for (auto& t : threads) t.join();
    std::cout << "vector<thread>, squares:";
    for (int k : squares) std::cout << ' ' << k;
    std::cout << '\n';
}

// ---------------------------------------------------------------- 3
void increment(int& x) { ++x; }
void sameAddress(const int& x, const int* original, bool& same) { same = (&x == original); }
void take(std::unique_ptr<int> p, int& out) { out = *p; }

void section3() {
    std::cout << "\n== 3. arguments are COPIED into the thread\n";
    int counter = 0;
    std::thread t1(increment, std::ref(counter));   // bez std::ref: errors/e01
    t1.join();
    std::cout << "after increment(std::ref(counter)): " << counter << '\n';

    bool same = true;
    std::thread t2(sameAddress, counter, &counter, std::ref(same));   // const int& -- ali na KOPIJU
    t2.join();
    std::cout << "const int& parameter sees the original: " << same << '\n';

    auto p = std::make_unique<int>(42);
    int taken = 0;
    std::thread t3(take, std::move(p), std::ref(taken));   // move-only argument: std::move
    t3.join();
    std::cout << "unique_ptr moved into the thread: " << taken << ", p is now " << (p ? "full" : "empty") << '\n';
}

// ---------------------------------------------------------------- 4
// Nit nema povratnu vrednost (šta vrati funkcija, odbaci se). Rezultat
// ide kroz parametar-referencu ili capture; lekcija 40 ima std::future.
void partSum(const std::vector<int>& v, std::size_t from, std::size_t to, long& out) {
    out = std::accumulate(v.begin() + static_cast<std::ptrdiff_t>(from),
                            v.begin() + static_cast<std::ptrdiff_t>(to), 0L);
}

void section4() {
    std::cout << "\n== 4. returning results from a thread\n";
    std::vector<int> v(1000);
    std::iota(v.begin(), v.end(), 1);           // 1..1000
    const std::size_t partCount = 4, step = v.size() / partCount;
    std::vector<long> parts(partCount);           // svaka nit SVOJ element -- nema deljenja
    std::vector<std::thread> threads;
    for (std::size_t i = 0; i < partCount; ++i)
        threads.emplace_back(partSum, std::cref(v), i * step, (i + 1) * step, std::ref(parts[i]));
    for (auto& t : threads) t.join();
    std::cout << "parts:";
    for (long d : parts) std::cout << ' ' << d;
    std::cout << ", total " << std::accumulate(parts.begin(), parts.end(), 0L) << '\n';
}

// ---------------------------------------------------------------- 5
void section5() {
    std::cout << "\n== 5. std::mutex\n";
    long counter = 0;
    std::mutex m;
    auto work = [&] {
        for (int i = 0; i < 10000; ++i) {
            m.lock();
            ++counter;                            // kritična sekcija; bez mutex-a: ub/u01
            m.unlock();
        }
    };
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) threads.emplace_back(work);
    for (auto& t : threads) t.join();
    std::cout << "4 threads x 10000: " << counter << '\n';

    bool succeeded = true;
    m.lock();
    std::thread t([&] {
        succeeded = m.try_lock();                    // ne čeka: false ako je zaključan
        if (succeeded) m.unlock();
    });
    t.join();
    m.unlock();
    std::cout << "try_lock while main holds it: " << succeeded << '\n';
}

// ---------------------------------------------------------------- 6
struct Account {
    std::mutex m;
    int balance;
    explicit Account(int s) : balance(s) {}
};

void transfer(Account& src, Account& dst, int amount) {
    std::scoped_lock lock(src.m, dst.m);      // C++17: oba odjednom, bez deadlock-a (ub/u02)
    src.balance -= amount;
    dst.balance += amount;
}

void section6() {
    std::cout << "\n== 6. lock_guard and scoped_lock (RAII)\n";
    std::vector<std::string> journal;
    std::mutex m;
    auto write = [&](int id) {
        std::lock_guard<std::mutex> guard(m);   // unlock u destruktoru, i kad se baci izuzetak
        journal.push_back("thread " + std::to_string(id));
    };
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) threads.emplace_back(write, i);
    for (auto& t : threads) t.join();
    std::sort(journal.begin(), journal.end());   // redosled upisa nije određen
    std::cout << "log (sorted):";
    for (const auto& d : journal) std::cout << " [" << d << ']';
    std::cout << '\n';

    Account a(100), b(100);
    std::thread t1([&] { for (int i = 0; i < 1000; ++i) transfer(a, b, 1); });
    std::thread t2([&] { for (int i = 0; i < 1000; ++i) transfer(b, a, 1); });   // suprotan redosled
    t1.join();
    t2.join();
    std::cout << "after 2 x 1000 transfers in both directions: " << a.balance << ' ' << b.balance << '\n';

    std::unique_lock<std::mutex> ul(m);          // kao lock_guard, ali ume i unlock/lock
    ul.unlock();
    std::cout << "unique_lock after unlock: owns_lock = " << ul.owns_lock() << '\n';
}

// ---------------------------------------------------------------- 7
void section7() {
    std::cout << "\n== 7. std::thread methods and std::this_thread\n";
    std::thread::id threadId;
    std::thread t([&threadId] { threadId = std::this_thread::get_id(); });
    std::thread::id fromOutside = t.get_id();
    t.join();
    std::cout << "get_id inside == get_id outside: " << (threadId == fromOutside) << '\n';
    std::cout << "thread id != main's id: " << (threadId != std::this_thread::get_id()) << '\n';
    std::cout << "after join: t.get_id() == std::thread::id{}: " << (t.get_id() == std::thread::id{}) << '\n';

    auto start = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    auto elapsed = std::chrono::steady_clock::now() - start;
    std::cout << "sleep_for(20ms) slept at least 20ms: " << (elapsed >= std::chrono::milliseconds(20)) << '\n';
    std::this_thread::yield();                   // "pusti druge" -- samo savet raspoređivaču

    std::thread x([] {}), y;
    std::swap(x, y);
    std::cout << "after swap: x.joinable() = " << x.joinable() << ", y.joinable() = " << y.joinable() << '\n';
    y.join();
}

int main() {
    std::cout << std::boolalpha;
    section1();
    section2();
    section3();
    section4();
    section5();
    section6();
    section7();
}
