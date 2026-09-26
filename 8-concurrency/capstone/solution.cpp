// Rešenje završne vežbe dela 8: pipeline merenja sa condition_variable.

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

// ---------------------------------------------------------------- korak 1
// Red koji više niti bezbedno puni i prazni. receive() ČEKA dok nešto ne
// stigne ili dok se red ne zatvori -- bez vrćenja u petlji.
template <typename T>
class SafeQueue {
public:
    void send(T x) {
        {
            std::lock_guard<std::mutex> g(m_);
            if (closed_) throw std::logic_error("sending to a closed queue");
            q_.push_back(std::move(x));
        }
        cv_.notify_one();                            // probudi jednog koji čeka; van zaključavanja
    }

    // Prazan optional: red je zatvoren I prazan -- nema više posla.
    std::optional<T> receive() {
        std::unique_lock<std::mutex> l(m_);          // wait traži unique_lock (otključa dok spava)
        cv_.wait(l, [this] { return !q_.empty() || closed_; });   // predikat: i lažna buđenja
        if (q_.empty()) return std::nullopt;
        T x = std::move(q_.front());
        q_.pop_front();
        return x;
    }

    void close() {
        {
            std::lock_guard<std::mutex> g(m_);
            closed_ = true;
        }
        cv_.notify_all();                            // SVI koji čekaju treba da vide kraj
    }

private:
    std::mutex m_;
    std::condition_variable cv_;
    std::deque<T> q_;
    bool closed_ = false;
};

struct Reading {
    int sensor;
    long value;
};

struct Sum {
    int n = 0;
    long total = 0;
};

// ---------------------------------------------------------------- korak 2
// Proizvođač šalje n merenja; senzor koji "otkaže" posle k merenja baci izuzetak.
int producer(SafeQueue<Reading>& queue, int sensor, int n, int failAfter = -1) {
    for (int i = 0; i < n; ++i) {
        if (i == failAfter) throw std::runtime_error("sensor " + std::to_string(sensor) + ": no response");
        queue.send({sensor, sensor * 100L + i});
    }
    return n;
}

// Potrošač prima dok red ne bude zatvoren i prazan; svaki ima SVOJ zbir, bez deljenja.
std::map<int, Sum> consumer(SafeQueue<Reading>& queue) {
    std::map<int, Sum> sums;
    while (auto m = queue.receive()) {
        Sum& z = sums[m->sensor];
        ++z.n;
        z.total += m->value;
    }
    return sums;
}

void mergeInto(std::map<int, Sum>& overall, const std::map<int, Sum>& part) {
    for (const auto& [sensor, z] : part) {
        overall[sensor].n += z.n;
        overall[sensor].total += z.total;
    }
}

// ---------------------------------------------------------------- korak 3
// RAII: red se zatvara i kad izuzetak preskoči ostatak funkcije -- inače bi
// potrošači čekali zauvek, a future iz async-a bi u destruktoru čekao njih.
class CloseAtEnd {
public:
    explicit CloseAtEnd(SafeQueue<Reading>& r) : queue_(r) {}
    ~CloseAtEnd() { queue_.close(); }
    CloseAtEnd(const CloseAtEnd&) = delete;
    CloseAtEnd& operator=(const CloseAtEnd&) = delete;

private:
    SafeQueue<Reading>& queue_;
};

struct Result {
    std::map<int, Sum> bySensor;
    std::vector<std::string> errors;
};

Result run(int consumers, const std::vector<std::pair<int, int>>& sensors, int failingSensor, int failAfter) {
    SafeQueue<Reading> queue;
    std::vector<std::future<std::map<int, Sum>>> p;
    for (int i = 0; i < consumers; ++i) p.push_back(std::async(std::launch::async, consumer, std::ref(queue)));

    Result r;
    {
        CloseAtEnd closer(queue);
        std::vector<std::future<int>> producers;
        for (const auto& [sensor, n] : sensors)
            producers.push_back(std::async(std::launch::async, producer, std::ref(queue), sensor, n,
                                        sensor == failingSensor ? failAfter : -1));
        for (auto& f : producers) {
            try {
                f.get();                             // izuzetak iz proizvođača stiže ovde
            } catch (const std::exception& e) {
                r.errors.push_back(e.what());
            }
        }
    }                                                // svi proizvođači gotovi: zatvori red
    for (auto& f : p) mergeInto(r.bySensor, f.get());
    return r;
}

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

int main() {
    std::cout << std::boolalpha << "== step 1: queue in a single thread\n";
    SafeQueue<Reading> queue;
    queue.send({1, 10});
    queue.send({2, 20});
    queue.close();
    const auto a = queue.receive();
    const auto b = queue.receive();
    const auto c = queue.receive();
    std::cout << "received " << a->value << ", " << b->value << ", empty after closing: " << !c.has_value()
              << '\n';
    try {
        queue.send({3, 30});
    } catch (const std::logic_error& e) {
        std::cout << "sending after closing: " << e.what() << '\n';
    }

    std::cout << "== step 2: one producer, one consumer (std::thread)\n";
    SafeQueue<Reading> r2;
    std::map<int, Sum> sums2;
    std::thread cons([&] { sums2 = consumer(r2); });
    std::thread prod([&] {
        producer(r2, 7, 1000);
        r2.close();
    });
    prod.join();
    cons.join();
    std::cout << "sensor 7: " << sums2[7].n << " readings, total " << sums2[7].total << '\n';

    std::cout << "== step 3: three producers, two consumers (std::async)\n";
    print(run(2, {{1, 500}, {2, 300}, {3, 200}}, 0, -1));

    std::cout << "== step 4: sensor 2 fails after 50 readings\n";
    print(run(3, {{1, 500}, {2, 300}, {3, 200}}, 2, 50));
}
