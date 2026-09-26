// Rešenje završne vežbe dela 5: sistem događaja.

#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct Message {
    std::string source;
    double value;
};

// ---------------------------------------------------------------- korak 1
using Handler = std::function<void(const Message&)>;
using Filter = std::function<bool(const Message&)>;

class Dispatcher {
public:
    using Id = int;

    Id subscribe(const std::string& topic, Handler r) {
        subscriptions_.push_back({next_, topic, std::move(r)});
        return next_++;
    }

    bool unsubscribe(Id id) {
        for (auto it = subscriptions_.begin(); it != subscriptions_.end(); ++it) {
            if (it->id == id) {
                subscriptions_.erase(it);
                return true;
            }
        }
        return false;
    }

    void addFilter(Filter f) { filters_.push_back(std::move(f)); }

    // Vraća koliko je rukovalaca pozvano. Obilazi KOPIJU spiska: rukovalac
    // sme da odjavi sebe ili druge (korak 4), a erase iz vektora koji se
    // upravo obilazi poništio bi iteratore.
    int publish(const std::string& topic, const Message& p) {
        for (const Filter& f : filters_)
            if (!f(p)) return 0;
        const std::vector<Subscription> snapshot = subscriptions_;
        int called = 0;
        for (const Subscription& s : snapshot) {
            if (s.topic == topic && isActive(s.id)) {
                s.r(p);
                ++called;
            }
        }
        return called;
    }

    std::size_t subscriptionCount() const { return subscriptions_.size(); }

private:
    struct Subscription {
        Id id;
        std::string topic;
        Handler r;
    };

    // Rukovalac odjavljen u toku iste objave se više ne poziva.
    bool isActive(Id id) const {
        for (const Subscription& s : subscriptions_)
            if (s.id == id) return true;
        return false;
    }

    std::vector<Subscription> subscriptions_;
    std::vector<Filter> filters_;
    Id next_ = 1;
};

// ---------------------------------------------------------------- korak 2
double linear(double x, double k, double n) { return k * x + n; }

double applySteps(const std::vector<std::function<double(double)>>& chain, double x) {
    for (const auto& f : chain) x = f(x);
    return x;
}

// ---------------------------------------------------------------- korak 3
// Pretplaćuje se u konstruktoru, odjavljuje u destruktoru (RAII, lekcija 21):
// lambda hvata this, pa ne sme da nadživi objekat.
class Logger {
public:
    Logger(Dispatcher& d, const std::string& topic) : d_(d) {
        id_ = d_.subscribe(topic, [this](const Message& p) { write(p); });
    }
    ~Logger() { d_.unsubscribe(id_); }
    Logger(const Logger&) = delete;              // kopija bi delila isti id i odjavila ga dvaput
    Logger& operator=(const Logger&) = delete;

    int written() const { return written_; }

private:
    void write(const Message& p) {
        ++written_;
        std::cout << "  log: " << p.source << ' ' << p.value << '\n';
    }

    Dispatcher& d_;
    Dispatcher::Id id_ = 0;
    int written_ = 0;
};

int main() {
    using namespace std::placeholders;

    std::cout << "== step 1: subscribe, capture, unsubscribe\n";
    Dispatcher d;
    int counter = 0;
    const auto counterId = d.subscribe("temp", [&counter](const Message&) { ++counter; });
    std::string prefix = "  [temp] ";
    const auto printId = d.subscribe("temp", [prefix](const Message& p) { std::cout << prefix << p.value << '\n'; });
    prefix = "changed ";                          // lambda ima svoju kopiju
    const int called = d.publish("temp", {"hall", 21.5});   // pre ispisa: rukovaoci i sami pišu na cout
    std::cout << "publish temp: " << called << " handlers\n";
    std::cout << "publish humidity: " << d.publish("humidity", {"hall", 40}) << " handlers\n";
    d.unsubscribe(counterId);
    d.publish("temp", {"hall", 22.0});
    std::cout << "counter " << counter << " (did not see the second publish), subscriptions " << d.subscriptionCount() << '\n';
    d.unsubscribe(printId);

    std::cout << "== step 2: filter and processing chain (bind and lambda)\n";
    d.addFilter([](const Message& p) { return p.value > -50 && p.value < 150; });
    const std::vector<std::function<double(double)>> toFahrenheit{
        std::bind(linear, _1, 1.8, 32.0),          // isto što i [](double c) { return 1.8 * c + 32; }
        [](double f) { return std::round(f * 10) / 10; },
    };
    const auto fId =
        d.subscribe("temp", [&toFahrenheit](const Message& p) { std::cout << "  " << p.value << " C = " << applySteps(toFahrenheit, p.value) << " F\n"; });
    d.publish("temp", {"hall", 21.5});
    std::cout << "publish 999 (filter): " << d.publish("temp", {"hall", 999}) << " handlers\n";
    d.unsubscribe(fId);

    std::cout << "== step 3: the logger unsubscribes in its destructor\n";
    {
        Logger log(d, "alarm");
        d.publish("alarm", {"boiler", 91});
        d.publish("alarm", {"boiler", 95});
        std::cout << "written " << log.written() << ", subscriptions " << d.subscriptionCount() << '\n';
    }
    std::cout << "after the block: subscriptions " << d.subscriptionCount() << ", publish alarm: " << d.publish("alarm", {"boiler", 99})
              << " handlers\n";

    std::cout << "== step 4: a handler that unsubscribes during a publish\n";
    Dispatcher::Id onceId = 0;
    onceId = d.subscribe("start", [&d, &onceId](const Message& p) {
        std::cout << "  first start from " << p.source << ", unsubscribing\n";
        d.unsubscribe(onceId);
    });
    d.subscribe("start", [](const Message& p) { std::cout << "  start from " << p.source << '\n'; });
    d.publish("start", {"pump", 1});
    d.publish("start", {"valve", 1});
    std::cout << "subscriptions at the end: " << d.subscriptionCount() << '\n';
}
