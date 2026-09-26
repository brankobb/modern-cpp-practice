// KIND: usage
//
// Zadatak 1 -- unique_ptr, shared_ptr i weak_ptr po nameni (sekcije 1-4)
//   ./build.sh 6-smart-pointers/32-smart-pointers/exercises/ex1_ownership.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_ownership.cpp
//
// Korak 1: jedan vlasnik -> unique_ptr.
//   std::unique_ptr<Sensor> makeSensor(int id) -- fabrika
//   (std::make_unique). class System sa std::vector<std::unique_ptr<Sensor>>
//   i void add(std::unique_ptr<Sensor> s) -- "sink" po vrednosti:
//   pozivalac mora da napiše std::move, pa se u kodu VIDI da predaje
//   vlasništvo. std::size_t count() const.
// Korak 2: više vlasnika -> shared_ptr. Dva modula (struct Module sa
//   std::shared_ptr<const Config> cfg) dele istu konfiguraciju
//   (std::make_shared). use_count() pokazuje broj vlasnika.
// Korak 3: posmatrač bez vlasništva -> weak_ptr. struct Observer sa
//   std::weak_ptr<const Config> cfg i metodom void check() const: lock()
//   vrati shared_ptr (pun ako objekat još živi, prazan ako ne).

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct Sensor {
    explicit Sensor(int sensorId) : id(sensorId) { std::cout << "Sensor(" << id << ")\n"; }
    ~Sensor() { std::cout << "~Sensor(" << id << ")\n"; }
    int id;
};

struct Config {
    explicit Config(std::string i) : name(std::move(i)) {}
    std::string name;
    ~Config() { std::cout << "~Config(" << name << ")\n"; }
};

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // {
    //     System sys;
    //     auto s = makeSensor(1);
    //     sys.add(std::move(s));
    //     sys.add(makeSensor(2));
    //     std::cout << "s after handing over: " << (s ? "full" : "empty") << ", in the system: " << sys.count() << '\n';
    // }

    // Korak 2 i 3 -- otkomentariši:
    // Observer obs;
    // {
    //     auto cfg = std::make_shared<const Config>("v1");
    //     Module a{cfg}, b{cfg};
    //     obs.cfg = cfg;
    //     std::cout << "owners: " << cfg.use_count() << '\n';
    //     obs.check();
    // }
    // obs.check();
}

/* EXPECTED OUTPUT
Sensor(1)
Sensor(2)
s after handing over: empty, in the system: 2
~Sensor(1)
~Sensor(2)
owners: 3
observer sees: v1
~Config(v1)
observer sees: nothing (object destroyed)
*/
