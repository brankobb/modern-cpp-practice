#include <cstddef>
#include <iostream>
#include <new>
#include <utility>

// VEŽBA -- piši sam, pa uporedi sa main.cpp (rešenje).
//   ./build.sh 6-smart-pointers/33-unique-ptr-embedded-exercise/exercise.cpp
// Fajl se kompajlira i ovakav. Otključavaj testove u main() kako dodaješ
// delove, i pokreći pod ASan-om.
//
// Deo 1 -- UniquePtr<T> (lekcije 22, 23, 32):
//   - explicit UniquePtr(T* p = nullptr) noexcept
//   - destruktor: delete
//   - kopija = delete; move konstruktor i dodela noexcept (izvor -> nullptr)
//   - operator*, operator->, get(), explicit operator bool
//   - release(): vrati pokazivač i odreci se vlasništva
//   - reset(T* p = nullptr): PRVO zapamti stari, postavi novi, PA obriši stari
//   - makeUnique<T>(args...): savršeno prosleđivanje (lekcija 27)
//   - proveri: sizeof(UniquePtr<int>) == sizeof(int*), noexcept move
// Deo 2 -- embedded RAII (lekcija 21):
//   - InterruptGuard: konstruktor zapamti PRETHODNO stanje prekida i
//     isključi ih; destruktor vrati PRETHODNO stanje (ne uvek "uključi"!)
//   - proveri ugnežđene guard-ove: posle unutrašnjeg prekidi moraju ostati
//     isključeni
// Deo 3 -- objekat bez heap-a (lekcija 13, sekcija 2):
//   - StaticStorage<T>: alignas(T) unsigned char buf[sizeof(T)];
//     emplace(args...) -> placement new; destroy() -> ručni ~T();
//     destruktor StaticStorage uništi objekat ako postoji
//   - proveri da se ne alocira ništa na heap-u

// Simulirani registar prekida (na pravom mikrokontroleru: __disable_irq()
// i __enable_irq(), ili čitanje/pisanje PRIMASK registra).
bool interruptsEnabled = true;

template <typename T>
class UniquePtr {
public:
    // TODO deo 1
};

class InterruptGuard {
public:
    // TODO deo 2
};

template <typename T>
class StaticStorage {
public:
    // TODO deo 3
};

struct Widget {
    explicit Widget(int widgetId) : id(widgetId) { std::cout << "Widget(" << id << ") "; }
    ~Widget() { std::cout << "~Widget(" << id << ") "; }
    int id;
};

int main() {
    std::cout << "interrupts at the start: " << (interruptsEnabled ? "enabled" : "disabled") << "\n";

    // Deo 1 -- otkomentariši:
    // UniquePtr<Widget> a(new Widget(1));
    // UniquePtr<Widget> b = std::move(a);   // a mora biti prazan
    // b.reset(new Widget(2));               // ~Widget(1) pa ništa curi
    // std::cout << (a ? "a full" : "a empty") << " b->id=" << b->id << "\n";

    // Deo 2 -- otkomentariši:
    // {
    //     InterruptGuard outer;
    //     { InterruptGuard inner; }
    //     std::cout << "after the inner one: " << (interruptsEnabled ? "enabled (BUG)" : "disabled") << "\n";
    // }

    // Deo 3 -- otkomentariši:
    // StaticStorage<Widget> slot;
    // slot.emplace(7);
    // slot.destroy();
}
