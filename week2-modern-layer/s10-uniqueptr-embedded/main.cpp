#include <iostream>
#include <new>
#include <utility>

// Deo 1: sopstveni move-only smart pointer.
template <typename T>
class UniquePtr {
public:
    UniquePtr() noexcept = default;
    explicit UniquePtr(T* p) noexcept : ptr_(p) {}

    // Ako OSTAVIŠ copy ctor/assignment kompajlerski generisane (NIJE
    // DOBRO) jer bi dva UniquePtr-a onda delila isti sirovi pokazivač --
    // ista shallow-copy/double-free zamka koju smo videli u week1 s02,
    // samo umotanu u "pametan" pokazivač koji bi trebalo da to sprečava.
    // Treba da EKSPLICITNO obrišeš copy (= delete) -- to je definicija
    // "unique" u imenu, vlasništvo je EKSKLUZIVNO.
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    ~UniquePtr() { delete ptr_; }

    T& operator*() const { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }
    T* get() const noexcept { return ptr_; }

    T* release() noexcept {
        T* p = ptr_;
        ptr_ = nullptr;
        return p;
    }

    void reset(T* p = nullptr) noexcept {
        delete ptr_;
        ptr_ = p;
    }

private:
    T* ptr_ = nullptr;
};

// Deo 3: RAII guard bez heap-a (npr. embedded lock guard).
class FakeLock {
public:
    void lock() { std::cout << "lock\n"; }
    void unlock() { std::cout << "unlock\n"; }
};

class LockGuard {
public:
    explicit LockGuard(FakeLock& lock) : lock_(lock) { lock_.lock(); }
    ~LockGuard() { lock_.unlock(); }
    // Ako DOZVOLIŠ kopiranje LockGuard-a (NIJE DOBRO) jer bi kopija
    // pozvala unlock() DVA PUTA na istom lock-u kad oba objekta izađu iz
    // scope-a (jedanput za original, jedanput za kopiju) -- drugi
    // unlock() na već otključanom lock-u je bag (u pravom mutex-u:
    // undefined behavior).
    // Treba da obrišeš copy za SVAKI RAII guard čiji dtor radi
    // "oslobađanje" akcije koja se ne sme dogoditi dvaput.
    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

private:
    FakeLock& lock_;
};

struct Widget {
    Widget(int id) : id_(id) { std::cout << "Widget(" << id_ << ")\n"; }
    ~Widget() { std::cout << "~Widget(" << id_ << ")\n"; }
    int id_;
};

int main() {
    std::cout << "-- sopstveni UniquePtr --\n";
    UniquePtr<int> p(new int(42));
    std::cout << *p << "\n";

    std::cout << "-- RAII lock guard --\n";
    {
        FakeLock lk;
        LockGuard guard(lk);
        std::cout << "critical section\n";
    } // unlock ovde automatski

    std::cout << "-- placement new na statickom baferu (bez heap-a) --\n";
    // Ako pozoveš obican new Widget(7) ovde (NIJE DOBRO za embedded bez
    // heap-a) jer new ALOCIRA memoriju sa heap-a -- na sistemu bez heap-a
    // (ili gde je heap zabranjen iz sigurnosnih/determinističkih razloga)
    // to jednostavno ne radi ili je nepredvidivo.
    // Treba da koristiš placement new na VEĆ POSTOJEĆOJ, unapred
    // rezervisanoj memoriji (kao alignas bafer ispod) -- objekat se gradi
    // TAMO GDE VEĆ IMAŠ memoriju, bez nove alokacije.
    // Možeš i koristiti statički alociran niz Widget objekata (Widget
    // arr[N];) ako unapred znaš tačan broj -- placement new ti treba samo
    // kad želiš KONTROLU NAD TRENUTKOM konstrukcije (odloženu
    // inicijalizaciju na već rezervisanoj memoriji).
    alignas(Widget) unsigned char buf[sizeof(Widget)];
    Widget* w = new (buf) Widget(7);
    // Ako pozoveš delete w; ovde (NIJE DOBRO) jer delete pokušava da
    // OSLOBODI memoriju kao da je sa heap-a -- ali buf je stack niz
    // bajtova, ne heap alokacija -- UB (verovatno crash ili korupcija).
    // Treba da pozoveš destruktor RUČNO (w->~Widget();) -- to je jedini
    // ispravan način da "uništiš" placement-new objekat bez oslobađanja
    // memorije koja mu ne pripada.
    w->~Widget(); // ručno pozvan destruktor, obavezno za placement new
}
