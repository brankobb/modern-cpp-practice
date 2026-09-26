// Završna vežba dela 5 -- sistem događaja
//   ./build.sh 5-functions-as-values/capstone/task.cpp
// Uputstvo i spisak lekcija: 5-functions-as-values/capstone/notes.md
// Rešenje: solution.cpp (otvori tek kad tvoj izlaz bude isti kao blok
// EXPECTED OUTPUT na dnu fajla).
//
// Fajl se kompajlira i ovakav. Piši redom; posle svakog koraka
// otkomentariši njegov deo main()-a.
//
// Korak 1: class Dispatcher
//   -- using Id = int; Id subscribe(topic, Handler); bool unsubscribe(Id);
//   int publish(topic, message) -- pozove rukovaoce te teme redom kojim su
//   se pretplatili i vrati koliko ih je pozvano; subscriptionCount().
//   Handler je std::function<void(const Message&)> (lekcija 31, sekcija 1).
//   -- u main-u: lambda sa [&counter] i sa [prefix] (kopija u trenutku
//   pravljenja) -- lekcija 30, sekcija 5.
// Korak 2: addFilter(Filter) -- poruka stiže do rukovalaca samo ako
//   prođe sve filtere; linear(x, k, n) i applySteps(chain, x) nad
//   std::vector<std::function<double(double)>>. U main-u je prvi korak
//   lanca napravljen sa std::bind, drugi lambdom (lekcija 31, sekcije 3 i 5).
// Korak 3: class Logger -- u konstruktoru se pretplati (subscribe) lambdom koja hvata
//   this, u destruktoru se odjavi (unsubscribe) (RAII, lekcija 21; capture this:
//   lekcija 30, sekcija 7). Kopiranje zabrani -- zašto?
// Korak 4: rukovalac sme da odjavi sebe (ili drugog) USRED objave.
//   publish mora to da podnese: obilazi kopiju spiska, a preskače one koji
//   su u međuvremenu odjavljeni (lekcija 27, sekcija 6: invalidacija
//   iteratora).

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

using Handler = std::function<void(const Message&)>;
using Filter = std::function<bool(const Message&)>;

// TODO korak 1, 2, 3, 4

int main() {
    using namespace std::placeholders;

    // Korak 1 -- otkomentariši:
    // std::cout << "== step 1: subscribe, capture, unsubscribe\n";
    // Dispatcher d;
    // int counter = 0;
    // const auto counterId = d.subscribe("temp", [&counter](const Message&) { ++counter; });
    // std::string prefix = "  [temp] ";
    // const auto printId = d.subscribe("temp", [prefix](const Message& p) { std::cout << prefix << p.value << '\n'; });
    // prefix = "changed ";                          // lambda ima svoju kopiju
    // const int called = d.publish("temp", {"hall", 21.5});   // pre ispisa: rukovaoci i sami pišu na cout
    // std::cout << "publish temp: " << called << " handlers\n";
    // std::cout << "publish humidity: " << d.publish("humidity", {"hall", 40}) << " handlers\n";
    // d.unsubscribe(counterId);
    // d.publish("temp", {"hall", 22.0});
    // std::cout << "counter " << counter << " (did not see the second publish), subscriptions " << d.subscriptionCount() << '\n';
    // d.unsubscribe(printId);

    // Korak 2 -- otkomentariši:
    // std::cout << "== step 2: filter and processing chain (bind and lambda)\n";
    // d.addFilter([](const Message& p) { return p.value > -50 && p.value < 150; });
    // const std::vector<std::function<double(double)>> toFahrenheit{
    //     std::bind(linear, _1, 1.8, 32.0),          // isto što i [](double c) { return 1.8 * c + 32; }
    //     [](double f) { return std::round(f * 10) / 10; },
    // };
    // const auto fId =
    //     d.subscribe("temp", [&toFahrenheit](const Message& p) { std::cout << "  " << p.value << " C = " << applySteps(toFahrenheit, p.value) << " F\n"; });
    // d.publish("temp", {"hall", 21.5});
    // std::cout << "publish 999 (filter): " << d.publish("temp", {"hall", 999}) << " handlers\n";
    // d.unsubscribe(fId);

    // Korak 3 -- otkomentariši:
    // std::cout << "== step 3: the logger unsubscribes in its destructor\n";
    // {
    //     Logger log(d, "alarm");
    //     d.publish("alarm", {"boiler", 91});
    //     d.publish("alarm", {"boiler", 95});
    //     std::cout << "written " << log.written() << ", subscriptions " << d.subscriptionCount() << '\n';
    // }
    // std::cout << "after the block: subscriptions " << d.subscriptionCount() << ", publish alarm: " << d.publish("alarm", {"boiler", 99})
    //           << " handlers\n";

    // Korak 4 -- otkomentariši:
    // std::cout << "== step 4: a handler that unsubscribes during a publish\n";
    // Dispatcher::Id onceId = 0;
    // onceId = d.subscribe("start", [&d, &onceId](const Message& p) {
    //     std::cout << "  first start from " << p.source << ", unsubscribing\n";
    //     d.unsubscribe(onceId);
    // });
    // d.subscribe("start", [](const Message& p) { std::cout << "  start from " << p.source << '\n'; });
    // d.publish("start", {"pump", 1});
    // d.publish("start", {"valve", 1});
    // std::cout << "subscriptions at the end: " << d.subscriptionCount() << '\n';
}

/* EXPECTED OUTPUT
== step 1: subscribe, capture, unsubscribe
  [temp] 21.5
publish temp: 2 handlers
publish humidity: 0 handlers
  [temp] 22
counter 1 (did not see the second publish), subscriptions 1
== step 2: filter and processing chain (bind and lambda)
  21.5 C = 70.7 F
publish 999 (filter): 0 handlers
== step 3: the logger unsubscribes in its destructor
  log: boiler 91
  log: boiler 95
written 2, subscriptions 1
after the block: subscriptions 0, publish alarm: 0 handlers
== step 4: a handler that unsubscribes during a publish
  first start from pump, unsubscribing
  start from pump
  start from valve
subscriptions at the end: 1
*/
