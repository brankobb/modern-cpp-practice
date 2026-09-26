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
// ./check_cases.sh 8-concurrency/40-async-and-future  proverava sve.

using namespace std::chrono_literals;

// ---------------------------------------------------------------- 1
long rangeSum(const std::vector<int>& v, std::size_t from, std::size_t to) {
    return std::accumulate(v.begin() + static_cast<std::ptrdiff_t>(from),
                           v.begin() + static_cast<std::ptrdiff_t>(to), 0L);
}

void section1() {
    std::cout << "\n== 1. std::async returns a std::future\n";
    std::future<int> f = std::async([] { return 6 * 7; });
    std::cout << "valid before get: " << f.valid() << '\n';
    std::cout << "get: " << f.get() << '\n';                // čeka rezultat
    std::cout << "valid after get: " << f.valid() << '\n';  // rezultat je "potrošen" (runtime/r01)

    std::vector<int> v(1000);
    std::iota(v.begin(), v.end(), 1);
    std::vector<std::future<long>> parts;
    for (std::size_t i = 0; i < 4; ++i)
        parts.push_back(std::async(std::launch::async, rangeSum, std::cref(v), i * 250, (i + 1) * 250));
    long total = 0;
    for (auto& d : parts) total += d.get();              // bez vector<long>, std::ref i join
    std::cout << "sum 1..1000 in 4 tasks: " << total << '\n';
}

// ---------------------------------------------------------------- 2
void increment(int& x) { ++x; }

struct Calibrator {
    int offset;
    int apply(int x) const { return x + offset; }
};

void section2() {
    std::cout << "\n== 2. arguments, methods, shared_future\n";
    int counter = 0;
    std::async(std::launch::async, increment, std::ref(counter)).get();   // kao thread: kopira, osim std::ref
    std::cout << "after async(increment, std::ref(counter)): " << counter << '\n';

    Calibrator k{5};
    auto f = std::async(&Calibrator::apply, &k, 10);
    std::cout << "method: " << f.get() << '\n';

    // future: jedan čitalac, jedan get. shared_future: kopira se, get koliko god puta.
    std::shared_future<int> shared = std::async([] { return 100; }).share();
    std::vector<std::future<int>> readers;
    for (int i = 1; i <= 3; ++i)
        readers.push_back(std::async(std::launch::async, [shared, i] { return shared.get() + i; }));
    std::cout << "3 readers of the same shared_future:";
    for (auto& c : readers) std::cout << ' ' << c.get();
    std::cout << '\n';
}

// ---------------------------------------------------------------- 3
void section3() {
    std::cout << "\n== 3. launch policies\n";
    const auto mainId = std::this_thread::get_id();

    auto a = std::async(std::launch::async, [] { return std::this_thread::get_id(); });
    std::cout << "launch::async in another thread: " << (a.get() != mainId) << '\n';

    bool started = false;
    auto d = std::async(std::launch::deferred, [&started] {
        started = true;
        return std::this_thread::get_id();
    });
    std::cout << "deferred started before get: " << started << '\n';
    std::cout << "deferred in the thread that calls get: " << (d.get() == mainId) << '\n';
    std::cout << "deferred started after get: " << started << '\n';

    bool ranEver = false;
    {
        auto n = std::async(std::launch::deferred, [&ranEver] { ranEver = true; });
    }   // niko nije pozvao get/wait -- zadatak se nikad ne izvrši
    std::cout << "deferred without get ever started: " << ranEver << '\n';
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

void section4() {
    std::cout << "\n== 4. wait, wait_for, wait_until\n";
    std::promise<void> go;
    std::shared_future<void> signal = go.get_future().share();
    auto job = std::async(std::launch::async, [signal] {
        signal.wait();                                     // blokiran dok main ne da signal
        return 1;
    });
    std::cout << "wait_for(10ms) while waiting for the signal: " << status(job.wait_for(10ms)) << '\n';
    std::cout << "wait_until(now + 10ms): " << status(job.wait_until(std::chrono::steady_clock::now() + 10ms))
              << '\n';
    go.set_value();
    job.wait();                                          // čeka bez uzimanja rezultata
    std::cout << "after wait: " << status(job.wait_for(0ms)) << ", get = " << job.get() << '\n';

    auto deferredTask = std::async(std::launch::deferred, [] { return 2; });
    std::cout << "wait_for on deferred: " << status(deferredTask.wait_for(0ms)) << '\n';

    // Destruktor future-a iz std::async čeka kraj zadatka (EMC Item 38).
    bool done = false;
    {
        auto f = std::async(std::launch::async, [&done] {
            std::this_thread::sleep_for(20ms);
            done = true;
        });
    }   // f se uništi ovde -- i čeka, iako niko nije pozvao get
    std::cout << "after the future is destroyed the task is done: " << done << '\n';
}

// ---------------------------------------------------------------- 5
void section5() {
    std::cout << "\n== 5. std::promise\n";
    std::promise<int> sender;
    std::future<int> result = sender.get_future();
    std::thread t([&sender] { sender.set_value(21); });   // vrednost iz OBIČNE niti
    std::cout << "promise -> future: " << result.get() << '\n';
    t.join();

    std::future<int> abandoned;
    {
        std::promise<int> p;
        abandoned = p.get_future();
    }   // promise uništen bez set_value
    try {
        abandoned.get();
    } catch (const std::future_error& e) {
        std::cout << "future_error, broken_promise: " << (e.code() == std::future_errc::broken_promise) << '\n';
    }

    // packaged_task: funkcija + promise u jednom -- poziv upiše rezultat u future.
    std::packaged_task<int(int, int)> task([](int a, int b) { return a * b; });
    std::future<int> product = task.get_future();
    std::thread worker(std::move(task), 6, 7);
    std::cout << "packaged_task in a thread: " << product.get() << '\n';
    worker.join();
}

// ---------------------------------------------------------------- 6
int calibrate(int x) {
    if (x < 0) throw std::invalid_argument("negative reference: " + std::to_string(x));
    return x * 2;
}

void section6() {
    std::cout << "\n== 6. exceptions across threads\n";
    auto f = std::async(std::launch::async, calibrate, -3);
    try {
        f.get();                                           // izuzetak iz zadatka se baci OVDE
    } catch (const std::invalid_argument& e) {
        std::cout << "caught in main: " << e.what() << '\n';
    }

    std::promise<int> p;
    std::future<int> r = p.get_future();
    std::thread t([&p] {
        try {
            p.set_value(calibrate(-5));
        } catch (...) {
            p.set_exception(std::current_exception());     // exception_ptr (lekcija 18, sekcija 9)
        }
    });
    t.join();
    try {
        r.get();
    } catch (const std::exception& e) {
        std::cout << "from the promise: " << e.what() << '\n';
    }
}

int main() {
    std::cout << std::boolalpha;
    section1();
    section2();
    section3();
    section4();
    section5();
    section6();
}
