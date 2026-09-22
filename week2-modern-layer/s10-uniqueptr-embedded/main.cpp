#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

// Sesija 10 -- rešenje vežbe: sopstveni UniquePtr, embedded RAII, objekti
// bez heap-a. Vežba (bez rešenja) je u exercise.cpp. Sve se kompajlira i
// radi bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior (ili curi)
// ./check_cases.sh week2-modern-layer/s10-uniqueptr-embedded  proverava oba.

// Brojač alokacija (kao u s08): dokaz da deo 3 ne koristi heap.
static int allocations = 0;
void* operator new(std::size_t n) {
    ++allocations;
    if (void* p = std::malloc(n)) return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

struct Widget {
    explicit Widget(int widgetId) : id(widgetId) { std::cout << "Widget(" << id << ") "; }
    ~Widget() { std::cout << "~Widget(" << id << ") "; }
    int id;
};

// ================================================================ deo 1
template <typename T>
class UniquePtr {
public:
    constexpr UniquePtr() noexcept = default;
    explicit UniquePtr(T* p) noexcept : ptr_(p) {}
    ~UniquePtr() { delete ptr_; }

    UniquePtr(const UniquePtr&) = delete; // "unique": jedan vlasnik (errors/e01)
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept : ptr_(std::exchange(other.ptr_, nullptr)) {}
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        reset(other.release()); // radi i za p = std::move(p): release vrati isti pokazivač, reset ga ne briše
        return *this;
    }

    T& operator*() const noexcept { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }
    T* get() const noexcept { return ptr_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    T* release() noexcept { return std::exchange(ptr_, nullptr); } // pozivalac postaje vlasnik

    void reset(T* p = nullptr) noexcept {
        T* old = std::exchange(ptr_, p); // prvo postavi novi...
        if (old != p) delete old;        // ...pa obriši stari (ako se ~T nekako vrati na *this, stanje je već ispravno)
    }

private:
    T* ptr_ = nullptr;
};

template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args) { // s09: savršeno prosleđivanje
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

void part1_uniquePtr() {
    std::cout << "-- deo 1: sopstveni UniquePtr --\n  ";
    UniquePtr<Widget> a = makeUnique<Widget>(1);
    UniquePtr<Widget> b = std::move(a);
    std::cout << "| a=" << (a ? "pun" : "prazan") << " b->id=" << b->id << " | ";
    b.reset(new Widget(2)); // ~Widget(1)
    std::cout << "| ";
    UniquePtr<Widget>& alias = b;
    b = std::move(alias); // move dodela samom sebi: ništa ne sme da se obriše
    std::cout << "posle b = move(b): b->id=" << b->id << " | ";
    Widget* raw = b.release();
    std::cout << "release -> b=" << (b ? "pun" : "prazan") << " | ";
    delete raw;
    std::cout << "\n  ";
    std::vector<UniquePtr<Widget>> many;
    for (int i = 3; i < 6; ++i) many.push_back(makeUnique<Widget>(i)); // realokacija koristi noexcept move
    std::cout << "| u vektoru: " << many.size() << " | ";
    many.clear();
    std::cout << "\n  sizeof(UniquePtr<Widget>)=" << sizeof(UniquePtr<Widget>) << " nothrow move="
              << std::is_nothrow_move_constructible_v<UniquePtr<Widget>>
              << " kopija=" << std::is_copy_constructible_v<UniquePtr<Widget>> << "\n";
}

// ================================================================ deo 2
// Simulirani registar prekida (na mikrokontroleru: __disable_irq() /
// __enable_irq(), ili čitanje i pisanje PRIMASK registra).
bool interruptsEnabled = true;

class InterruptGuard {
public:
    InterruptGuard() noexcept : wasEnabled_(interruptsEnabled) { interruptsEnabled = false; }
    ~InterruptGuard() { interruptsEnabled = wasEnabled_; } // vrati PRETHODNO stanje, ne "uključi"
    InterruptGuard(const InterruptGuard&) = delete;
    InterruptGuard& operator=(const InterruptGuard&) = delete;

private:
    bool wasEnabled_;
};

class NaiveInterruptGuard { // NE RADI OVAKO: na izlazu uvek uključi
public:
    NaiveInterruptGuard() noexcept { interruptsEnabled = false; }
    ~NaiveInterruptGuard() { interruptsEnabled = true; }
    NaiveInterruptGuard(const NaiveInterruptGuard&) = delete;
    NaiveInterruptGuard& operator=(const NaiveInterruptGuard&) = delete;
};

const char* irqState() { return interruptsEnabled ? "uključeni" : "isključeni"; }

class SpinLock { // BasicLockable: lock() + unlock() -> radi sa std::lock_guard
public:
    void lock() noexcept {
        while (flag_.test_and_set(std::memory_order_acquire)) {
        }
    }
    void unlock() noexcept { flag_.clear(std::memory_order_release); }

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

void part2_embeddedGuards() {
    std::cout << "-- deo 2: RAII guard-ovi (prekidi, lock) --\n";
    {
        InterruptGuard outer;
        {
            InterruptGuard inner;
        }
        std::cout << "  InterruptGuard: posle unutrašnjeg guard-a prekidi su " << irqState() << "\n";
    }
    std::cout << "  posle spoljnog: " << irqState() << "\n";
    {
        NaiveInterruptGuard outer;
        {
            NaiveInterruptGuard inner;
        }
        std::cout << "  NaiveInterruptGuard: posle unutrašnjeg prekidi su " << irqState()
                  << "  <- BAG: kritična sekcija spoljnog više nije zaštićena\n";
    }
    SpinLock spin;
    int shared = 0;
    {
        std::lock_guard<SpinLock> lock(spin); // standardni guard radi sa bilo kojim lock()/unlock() tipom
        ++shared;
    }
    std::cout << "  std::lock_guard<SpinLock>: shared=" << shared << " (unlock u destruktoru)\n";
}

// ================================================================ deo 3
template <typename T>
class StaticStorage { // mesto za JEDAN T, bez heap-a
public:
    StaticStorage() = default;
    ~StaticStorage() { destroy(); }
    StaticStorage(const StaticStorage&) = delete;
    StaticStorage& operator=(const StaticStorage&) = delete;

    template <typename... Args>
    T& emplace(Args&&... args) {
        destroy();
        T* object = new (buffer_) T(std::forward<Args>(args)...); // placement new: samo konstrukcija
        constructed_ = true;
        return *object;
    }
    void destroy() noexcept {
        if (constructed_) {
            get().~T(); // ručni destruktor; NE delete (ub/u01)
            constructed_ = false;
        }
    }
    T& get() noexcept { return *std::launder(reinterpret_cast<T*>(buffer_)); } // C++17: launder za pristup kroz bafer
    bool hasValue() const noexcept { return constructed_; }

private:
    alignas(T) unsigned char buffer_[sizeof(T)]; // bez alignas: pogrešno poravnanje (ub/u03)
    bool constructed_ = false;
};

StaticStorage<Widget> globalSlot; // statička memorija: rezervisana pri pokretanju programa

void part3_noHeap() {
    std::cout << "-- deo 3: objekat bez heap-a (placement new) --\n  ";
    allocations = 0;
    globalSlot.emplace(7);
    std::cout << "| id=" << globalSlot.get().id << " | ";
    globalSlot.emplace(8); // prvo uništi 7, pa napravi 8
    std::cout << "| ";
    globalSlot.destroy();
    std::cout << "| hasValue=" << globalSlot.hasValue() << "\n  heap alokacija: " << allocations
              << "; sizeof(StaticStorage<Widget>)=" << sizeof(StaticStorage<Widget>) << "\n";
}

int main() {
    std::cout << std::boolalpha;
    part1_uniquePtr();
    part2_embeddedGuards();
    part3_noHeap();
}
