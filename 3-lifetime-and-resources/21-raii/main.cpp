#include <cstdio>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Lekcija 21 -- RAII i bezbednost pri izuzecima: ISPRAVNI slučajevi. Sve se
// kompajlira i radi bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, a pri pokretanju curi ili pukne
// ./check_cases.sh 3-lifetime-and-resources/21-raii  proverava oba.

// ---------------------------------------------------------------- 1
// RAII: konstruktor zauzme resurs, destruktor ga oslobodi. Pošto jezik
// UVEK pozove destruktor lokalnog objekta (i kad izuzetak prekine
// funkciju), resurs ne može da procuri.
class File {
public:
    File() : handle_(std::tmpfile()) {
        if (handle_ == nullptr) throw std::runtime_error("tmpfile failed");
        std::cout << "[opened] ";
    }
    ~File() {
        if (handle_ != nullptr) {
            std::fclose(handle_); // destruktor ne baca: greška zatvaranja se ovde samo ignoriše
            std::cout << "[closed] ";
        }
    }
    File(const File&) = delete; // dve kopije = dva fclose (errors/e01)
    File& operator=(const File&) = delete;

    void write(const std::string& text) {
        if (std::fputs(text.c_str(), handle_) < 0) throw std::runtime_error("write failed");
    }

private:
    std::FILE* handle_;
};

void processAndFail(File& file) {
    file.write("first line\n");
    throw std::runtime_error("processing failed");
}

void s01_raii() {
    std::cout << "-- 1. RAII: the resource belongs to an object --\n  ";
    try {
        File file;
        processAndFail(file); // izuzetak -> ~File se svejedno pozove
    } catch (const std::runtime_error& e) {
        std::cout << "| caught: " << e.what() << "\n";
    }
}

// ---------------------------------------------------------------- 2
struct Tracer {
    explicit Tracer(const char* name) : name_(name) { std::cout << name_ << "() "; }
    ~Tracer() { std::cout << "~" << name_ << "() "; }

private:
    const char* name_;
};

void inner() {
    Tracer c("inner");
    throw std::runtime_error("from inner");
}
void middle() {
    Tracer b("middle");
    inner();
    std::cout << "(this never runs) ";
}

void s02_stackUnwinding() {
    std::cout << "-- 2. stack unwinding --\n  ";
    try {
        Tracer a("outer");
        middle();
    } catch (const std::runtime_error&) {
        std::cout << "| catch\n";
    }
    std::cout << "  <- the exception passes through functions and destroys their locals, innermost first\n";
}

// ---------------------------------------------------------------- 3
struct Config {
    Config() { throw std::runtime_error("bad config"); }
};

class Service { // resursi su u ČLANOVIMA koji se sami oslobađaju
public:
    Service() : buffer_(std::make_unique<int[]>(256)), config_(std::make_unique<Config>()) {}

private:
    std::unique_ptr<int[]> buffer_; // napravljen -> biće uništen i kad Config() baci
    std::unique_ptr<Config> config_;
};

void s03_constructorFailure() {
    std::cout << "-- 3. exception in a constructor, resources in members --\n";
    try {
        Service s;
    } catch (const std::runtime_error& e) {
        std::cout << "  Service(): " << e.what() << " -- buffer_ freed, no leak (with new: ub/u01)\n";
    }
}

// ---------------------------------------------------------------- 4
struct Item {
    std::string name;
    Item(std::string n) : name(std::move(n)) {}
    Item(const Item& other) : name(other.name) {
        if (name == "broken") throw std::runtime_error("copy failed");
    }
    Item& operator=(const Item&) = default;
};

class Inventory {
public:
    Inventory(std::initializer_list<const char*> names) { // pravi Item-e konstruktorom, bez kopije
        items_.reserve(names.size()); // bez ovoga bi realokacija KOPIRALA Item-e (Item nema move, lekcija 23)
        for (const char* n : names) items_.emplace_back(n);
    }

    // BASIC garancija: ako kopija baci, objekat je ispravan, ali IZMENJEN.
    void assignBasic(const Inventory& other) {
        items_.clear();
        for (const Item& item : other.items_) items_.push_back(item); // baca na sredini
    }

    // STRONG garancija: sve ili ništa. Kopija se pravi sa strane, pa se
    // zameni operacijom koja ne baca (copy-and-swap).
    void assignStrong(const Inventory& other) {
        std::vector<Item> copy = other.items_; // ako baci, items_ nije ni taknut
        items_.swap(copy);                     // swap ne baca (nothrow)
    }

    std::string list() const {
        std::string out;
        for (const Item& item : items_) out += item.name + " ";
        return out.empty() ? "(empty)" : out;
    }

private:
    std::vector<Item> items_;
};

void s04_guarantees() {
    std::cout << "-- 4. guarantees: basic, strong, nothrow --\n";
    Inventory source{"apple", "broken", "pear"}; // kopija "broken" baca
    Inventory basic{"old1", "old2"};
    Inventory strong{"old1", "old2"};
    try {
        basic.assignBasic(source);
    } catch (const std::runtime_error&) {
        std::cout << "  basic after exception:  " << basic.list() << " <- valid, but neither old nor new\n";
    }
    try {
        strong.assignStrong(source);
    } catch (const std::runtime_error&) {
        std::cout << "  strong after exception: " << strong.list() << " <- untouched\n";
    }
}

// ---------------------------------------------------------------- 5
class Connection {
public:
    ~Connection() noexcept { // destruktori su noexcept i bez ovoga (C++11)
        try {
            close();
        } catch (const std::exception& e) {
            std::cout << "  ~Connection swallowed an error: " << e.what() << "\n"; // zabeleži, ne bacaj dalje
        }
    }
    // Operacija koja može da ne uspe je posebna funkcija, pa pozivalac
    // može da reaguje (EC++ Item 8). Destruktor je samo rezervna opcija.
    void close() {
        if (closed_) return;
        closed_ = true;
        throw std::runtime_error("network down while closing");
    }

private:
    bool closed_ = false;
};

void s05_destructorsDontThrow() {
    std::cout << "-- 5. a destructor does not throw (EC++ Item 8) --\n";
    try {
        Connection explicitClose;
        explicitClose.close(); // pozivalac dobije izuzetak i odluči
    } catch (const std::runtime_error& e) {
        std::cout << "  close() threw: " << e.what() << "\n";
    }
    {
        Connection forgotten; // niko nije pozvao close(): destruktor to uradi i proguta grešku
    }
    std::cout << "  (an exception from a destructor would call std::terminate, ub/u02)\n";
}

// ---------------------------------------------------------------- 6
template <typename F>
class ScopeGuard { // "uradi ovo na izlazu iz bloka, kako god se izašlo"
public:
    explicit ScopeGuard(F action) : action_(std::move(action)) {}
    ~ScopeGuard() { action_(); }
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

private:
    F action_;
};

struct FileCloser { // deleter za unique_ptr: poziva se umesto delete
    void operator()(std::FILE* f) const noexcept { std::fclose(f); }
};

void s06_generalRaii() {
    std::cout << "-- 6. RAII without writing a class --\n";
    {
        // unique_ptr sa deleterom: destruktor pozove std::fclose (lekcija 32).
        std::unique_ptr<std::FILE, FileCloser> file(std::tmpfile());
        std::cout << "  unique_ptr<FILE, FileCloser>: open=" << (file != nullptr ? "yes" : "no") << "\n";
    }
    std::cout << "  ";
    try {
        ScopeGuard guard([] { std::cout << "[guard: restoring state] "; });
        throw std::runtime_error("x");
    } catch (const std::runtime_error&) {
        std::cout << "| catch\n";
    }
    // std::lock_guard, std::scoped_lock, std::vector, std::string: sve je RAII.
}

int main() {
    s01_raii();
    s02_stackUnwinding();
    s03_constructorFailure();
    s04_guarantees();
    s05_destructorsDontThrow();
    s06_generalRaii();
}
