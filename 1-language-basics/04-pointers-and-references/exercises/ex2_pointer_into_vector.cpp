// KIND: why
// DEMO-UB: NAIVE heap-use-after-free
//
// Zadatak 2 -- zašto pokazivač na element vector-a "ne drži" (sekcija 12)
// Rešenje: exercises/solutions/ex2_pointer_into_vector.cpp
//
// Registar čuva senzore u std::vector. Naivna ideja: add() vrati
// pokazivač na upravo dodat senzor, pa ga pozivalac čuva kao "ručku".
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/04-pointers-and-references/exercises/ex2_pointer_into_vector.cpp -DNAIVE
//   ASan prijavi heap-use-after-free. Nađi u izveštaju tri steka: gde je
//   čitano, gde je memorija oslobođena (unutar push_back!) i gde je
//   alocirana. Zašto push_back oslobađa staru memoriju?
// Korak 2: u #else grani napiši isti registar, ali add() vraća INDEKS
//   (std::size_t), a Sensor& get(std::size_t i) vraća referencu na
//   element tek kad ti zatreba. Otkomentariši test.
// Korak 3: zašto reserve() pre dodavanja nije pravo rešenje za registar
//   koji raste neograničeno? (upiši odgovor u komentar)

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

struct Sensor {
    std::string name;
    double temp;
};

#ifdef NAIVE
class Registry {
public:
    Sensor* add(std::string name, double temp) {
        sensors_.push_back({std::move(name), temp});
        return &sensors_.back();          // pokazuje u TRENUTNI blok memorije
    }
private:
    std::vector<Sensor> sensors_;
};

int main() {
    Registry r;
    Sensor* engine = r.add("engine", 70.0);
    r.add("battery", 38.0);               // realokacija: engine sada visi
    engine->temp += 1.0;                  // heap-use-after-free
    std::cout << engine->temp << '\n';
}
#else
class Registry {
public:
    // TODO korak 2
};

int main() {
    // Korak 2 -- otkomentariši:
    // Registry r;
    // std::size_t engine = r.add("engine", 70.0);
    // for (int i = 0; i < 100; ++i) r.add("s" + std::to_string(i), i);
    // r.get(engine).temp += 1.0;
    // std::cout << r.get(engine).name << ' ' << r.get(engine).temp << '\n';
    // std::cout << "total: " << r.count() << '\n';
}
#endif

/* EXPECTED OUTPUT
engine 71
total: 101
*/
