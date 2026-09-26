#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Izuzeci -- ISPRAVNI slučajevi. Sve se kompajlira bez upozorenja i radi
// bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20). Brojevi
// sekcija prate notes.md.
// POGREŠNI slučajevi:
//   errors/   -- kod koji se NE kompajlira
//   runtime/  -- kod koji se kompajlira, a program se prekine (std::terminate)
// ./check_cases.sh 3-lifetime-and-resources/18-exceptions  proverava oba.

struct Trace {
    explicit Trace(const char* name) : name_(name) { std::cout << ' ' << name_ << "()"; }
    ~Trace() { std::cout << " ~" << name_ << "()"; }
    Trace(const Trace&) = delete;
    Trace& operator=(const Trace&) = delete;
    const char* name_;
};

// ---------------------------------------------------------------- 1
double divide(double a, double b) {
    if (b == 0.0) throw std::invalid_argument("divisor is 0");
    return a / b;
}

void section1() {
    std::cout << "\n== 1. throw, try, catch\n";
    try {
        std::cout << "10 / 4 = " << divide(10, 4) << '\n';
        // Od C++17 se << računa sleva nadesno, pa je "10 / 0 = " već ispisano
        // kad divide() baci. Izuzetak prekida IZRAZ na tom mestu, ne unazad.
        std::cout << "10 / 0 = " << divide(10, 0) << '\n';
        std::cout << "this is not printed\n";
    } catch (const std::invalid_argument& e) {           // hvataj po const&
        std::cout << "caught invalid_argument: " << e.what() << '\n';   // ostatak try bloka preskočen
    }
    std::cout << "the program goes on\n";
}

// ---------------------------------------------------------------- 2
void throwKind(int kind) {
    switch (kind) {
        case 0: throw std::out_of_range("index 7");
        case 1: throw std::runtime_error("sensor not responding");
        case 2: throw std::logic_error("wrong call order");
        default: throw 42;                                 // može i ne-klasa, ali ne treba
    }
}

void section2() {
    std::cout << "\n== 2. several catch blocks: the first that matches\n";
    for (int i = 0; i < 4; ++i) {
        try {
            throwKind(i);
        } catch (const std::out_of_range& e) {      // izvedena klasa PRE bazne (logic_error)
            std::cout << "out_of_range: " << e.what() << '\n';
        } catch (const std::logic_error& e) {
            std::cout << "logic_error: " << e.what() << '\n';
        } catch (const std::exception& e) {         // sve standardne
            std::cout << "exception: " << e.what() << '\n';
        } catch (...) {                              // sve ostalo -- uvek poslednji
            std::cout << "unknown exception\n";
        }
    }
}

// ---------------------------------------------------------------- 3
// Sopstvena klasa: nasledi standardnu (runtime_error čuva poruku i daje
// what()), dodaj podatke koji pomažu pozivaocu.
class SensorError : public std::runtime_error {
public:
    SensorError(int sensorId, const std::string& message)
        : std::runtime_error("sensor " + std::to_string(sensorId) + ": " + message), id_(sensorId) {}
    int id() const noexcept { return id_; }

private:
    int id_;
};

void section3() {
    std::cout << "\n== 3. a custom exception class\n";
    try {
        throw SensorError(7, "out of range");
    } catch (const SensorError& e) {
        std::cout << e.what() << " (id " << e.id() << ")\n";
    }
    try {
        throw SensorError(3, "timeout");
    } catch (const std::exception& e) {              // i kao std::exception -- virtual what()
        std::cout << "as std::exception: " << e.what() << '\n';
    }
}

// ---------------------------------------------------------------- 4
void inner() {
    Trace c("c");
    throw std::runtime_error("deep down");
}
void middle() {
    Trace b("b");
    inner();
    std::cout << " this is not printed";
}

void section4() {
    std::cout << "\n== 4. stack unwinding\n";
    try {
        Trace a("a");
        middle();
    } catch (const std::exception& e) {
        std::cout << " | caught: " << e.what() << '\n';
    }
}

// ---------------------------------------------------------------- 5
void readConfig() {
    try {
        throw SensorError(5, "bad CRC");
    } catch (const std::exception& e) {
        std::cout << "inside: logged (" << e.what() << "), passing it on\n";
        throw;   // isti objekat, isti DINAMIČKI tip (throw e; bi napravio kopiju std::exception)
    }
}

void section5() {
    std::cout << "\n== 5. nested try and rethrowing\n";
    try {
        readConfig();
    } catch (const SensorError& e) {
        std::cout << "outside: still SensorError, id " << e.id() << '\n';
    }
}

// ---------------------------------------------------------------- 6
void loadBlock(int block) {
    try {
        throw std::runtime_error("CRC mismatch");
    } catch (...) {
        // Dodaj kontekst, a zadrži originalni izuzetak "unutra".
        std::throw_with_nested(std::runtime_error("block " + std::to_string(block)));
    }
}

void loadFile() {
    try {
        loadBlock(3);
    } catch (...) {
        std::throw_with_nested(std::runtime_error("file config.bin"));
    }
}

void printChain(const std::exception& e, int level = 0) {
    std::cout << std::string(static_cast<std::size_t>(level) * 2, ' ') << e.what() << '\n';
    try {
        std::rethrow_if_nested(e);           // baci unutrašnji, ako postoji
    } catch (const std::exception& cause) {
        printChain(cause, level + 1);
    }
}

void section6() {
    std::cout << "\n== 6. std::nested_exception: a chain of causes\n";
    try {
        loadFile();
    } catch (const std::exception& e) {
        printChain(e);
    }
}

// ---------------------------------------------------------------- 7
struct Buffer {
    explicit Buffer(std::size_t n) {
        if (n > 1024) throw std::length_error("buffer too large");
        std::cout << " Buffer(" << n << ")";
    }
    ~Buffer() { std::cout << " ~Buffer"; }
};

class Device {
public:
    // function-try-block: hvata i izuzetke iz init liste. Iz ovog catch-a
    // se NE može "vratiti" (errors/e03): objekat ne postoji, pa izuzetak
    // uvek ide dalje -- ovde preveden u tip koji pozivalac očekuje.
    explicit Device(std::size_t n) try : trace_("trace"), buffer_(n) {
        std::cout << " body";
    } catch (const std::length_error& e) {
        std::cout << " | Device: " << e.what();
        throw SensorError(0, "Device not created");
    }
    ~Device() { std::cout << " ~Device"; }

private:
    Trace trace_;
    Buffer buffer_;
};

void section7() {
    std::cout << "\n== 7. constructor and destructor\n";
    try {
        Device ok(16);
        std::cout << " |";
    } catch (...) {
    }
    std::cout << '\n';
    try {
        Device bad(4096);
    } catch (const SensorError& e) {
        // trace_ je napravljen, pa uništen PRE nego što handler function-try-
        // block-a počne; ~Device se NE poziva (objekat nikad nije postojao).
        std::cout << " | outside: " << e.what() << '\n';
    }
    // Destruktor je implicitno noexcept: izuzetak iz njega = std::terminate
    // (runtime/r02).
    static_assert(std::is_nothrow_destructible_v<Device>);
}

// ---------------------------------------------------------------- 8
int safe(int x) noexcept { return x * 2; }
int mayThrow(int x) { return x > 0 ? x : throw std::domain_error("x <= 0"); }

template <typename T>
void swapValues(T& a, T& b) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                     std::is_nothrow_move_assignable_v<T>) {
    T t = std::move(a);
    a = std::move(b);
    b = std::move(t);
}

struct MoveMayThrow {
    MoveMayThrow() = default;
    MoveMayThrow(MoveMayThrow&&) {}
    MoveMayThrow& operator=(MoveMayThrow&&) { return *this; }
};

void section8() {
    std::cout << "\n== 8. noexcept\n";
    // noexcept(izraz) je OPERATOR: pita pri kompajliranju, ne izvršava izraz.
    std::cout << std::boolalpha;
    std::cout << "noexcept(safe(1)): " << noexcept(safe(1)) << '\n';
    std::cout << "noexcept(mayThrow(1)): " << noexcept(mayThrow(1)) << '\n';
    int a = 1, b = 2;
    MoveMayThrow m1, m2;
    std::cout << "swapValues<int> noexcept: " << noexcept(swapValues(a, b)) << '\n';
    std::cout << "swapValues<MoveMayThrow> noexcept: " << noexcept(swapValues(m1, m2)) << '\n';
    std::cout << std::noboolalpha;
    swapValues(a, b);
    std::cout << "after swapValues: a=" << a << " b=" << b << '\n';
}

// ---------------------------------------------------------------- 9
std::exception_ptr runJob(int x) {
    try {
        mayThrow(x);
        return nullptr;
    } catch (...) {
        return std::current_exception();   // sačuvaj izuzetak kao vrednost
    }
}

void section9() {
    std::cout << "\n== 9. std::exception_ptr\n";
    std::vector<std::exception_ptr> results{runJob(5), runJob(-1)};
    for (std::size_t i = 0; i < results.size(); ++i) {
        if (!results[i]) {
            std::cout << "job " << i << ": ok\n";
            continue;
        }
        try {
            std::rethrow_exception(results[i]);   // kasnije, na drugom mestu (npr. druga nit)
        } catch (const std::exception& e) {
            std::cout << "job " << i << ": " << e.what() << '\n';
        }
    }
}

int main() {
    section1();
    section2();
    section3();
    section4();
    section5();
    section6();
    section7();
    section8();
    section9();
}
