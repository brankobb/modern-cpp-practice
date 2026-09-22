#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Sesija 4 -- move semantika (C++11): ISPRAVNI slučajevi. Sve se kompajlira
// i radi bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh week1-cpp03-to-move/s04-move-semantics  proverava oba.

// ---------------------------------------------------------------- 1
// decltype((izraz)) otkriva kategoriju izraza: T& = lvalue, T&& = xvalue,
// T = prvalue.
template <typename T>
const char* category() {
    if (std::is_lvalue_reference_v<T>) return "lvalue";
    if (std::is_rvalue_reference_v<T>) return "xvalue";
    return "prvalue";
}
#define CATEGORY(expr) category<decltype((expr))>()

std::string makeName() { return "privremeni"; }
std::string& firstName() {
    static std::string name = "Ana";
    return name;
}

void s01_valueCategories() {
    std::cout << "-- 1. kategorije vrednosti --\n";
    std::string s = "abc";
    int x = 1;
    std::cout << "  s: " << CATEGORY(s) << ", x + 1: " << CATEGORY(x + 1) << ", 42: " << CATEGORY(42)
              << ", \"abc\": " << CATEGORY("abc") << "\n";
    std::cout << "  makeName(): " << CATEGORY(makeName()) << ", firstName(): " << CATEGORY(firstName())
              << ", std::move(s): " << CATEGORY(std::move(s)) << "\n";
    std::cout << "  <- lvalue ima ime/adresu; prvalue je privremena vrednost; xvalue je \"objekat koji sme da se isprazni\"\n";
}

// ---------------------------------------------------------------- 2
const char* take(std::string&) { return "take(string&)"; }
const char* take(const std::string&) { return "take(const string&)"; }
const char* take(std::string&&) { return "take(string&&)"; }

void s02_referenceBinding() {
    std::cout << "-- 2. vezivanje referenci --\n";
    std::string s = "x";
    const std::string cs = "y";
    std::cout << "  take(s)=" << take(s) << ", take(cs)=" << take(cs) << ", take(makeName())=" << take(makeName())
              << ", take(std::move(s))=" << take(std::move(s)) << "\n";
    std::cout << "  <- T&& prima samo rvalue (errors/e01); const T& prima sve, ali tek ako nema boljeg\n";
}

// ---------------------------------------------------------------- 3
void s03_moveIsACast() {
    std::cout << "-- 3. std::move je samo cast --\n";
    std::string s(40, 'x');
    static_assert(std::is_same_v<decltype(std::move(s)), std::string&&>, "std::move vraća T&&");
    auto&& r = std::move(s); // samo referenca: ništa se ne pomera
    (void)r;
    std::cout << "  posle auto&& r = std::move(s): s.size()=" << s.size() << "  <- nepromenjen\n";
    std::string target = std::move(s); // TEK SADA move konstruktor stringa uzima bafer
    std::cout << "  posle std::string target = std::move(s): target.size()=" << target.size()
              << " s.size()=" << s.size() << " (libstdc++; standard kaže samo: ispravan, nepoznat sadržaj)\n";
}

// ---------------------------------------------------------------- 4
class Buffer {
public:
    explicit Buffer(std::size_t size) : data_(new int[size]{}), size_(size) {}

    Buffer(const Buffer& other) : data_(new int[other.size_]), size_(other.size_) { // kopija: nova memorija
        std::copy(other.data_, other.data_ + size_, data_);
        std::cout << "[copy ctor] ";
    }
    // Move: preuzmi pokazivač, a other ostavi PRAZAN (inače double free, ub/u01).
    Buffer(Buffer&& other) noexcept
        : data_(std::exchange(other.data_, nullptr)), size_(std::exchange(other.size_, 0)) {
        std::cout << "[move ctor] ";
    }
    Buffer& operator=(const Buffer& other) { // copy-and-swap (s03)
        Buffer copy(other);
        swap(copy);
        std::cout << "[copy =] ";
        return *this;
    }
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) { // a = std::move(a): bez provere bi se prvo obrisala sopstvena memorija (ub/u03)
            delete[] data_;
            data_ = std::exchange(other.data_, nullptr);
            size_ = std::exchange(other.size_, 0);
        }
        std::cout << "[move =] ";
        return *this;
    }
    ~Buffer() { delete[] data_; }

    void swap(Buffer& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
    }
    std::size_t size() const { return size_; }

private:
    int* data_;
    std::size_t size_;
};

Buffer makeBuffer(std::size_t n) { return Buffer(n); } // C++17: bez kopije i bez move-a (week2 s07)

void s04_moveOperations() {
    std::cout << "-- 4. move konstruktor i move dodela --\n  ";
    Buffer a(100);
    Buffer b = std::move(a); // a je lvalue; std::move ga pretvori u xvalue -> move ctor
    std::cout << "b=" << b.size() << " a=" << a.size() << " (moved-from: prazan)\n  ";
    Buffer c = b;            // lvalue bez std::move -> copy ctor
    std::cout << "c=" << c.size() << "\n  ";
    c = makeBuffer(7);       // privremeni (prvalue) -> move dodela, bez std::move
    std::cout << "c=" << c.size() << "\n  ";
    a = b;                   // copy dodela (copy-and-swap interno napravi kopiju)
    std::cout << "a=" << a.size() << "  <- copy = je copy-and-swap: kopija pa swap\n";
}

// ---------------------------------------------------------------- 5
std::vector<Buffer> storage;

void storeCopying(Buffer&& value) {
    storage.push_back(value); // value IMA IME -> lvalue -> KOPIJA, iako je tip Buffer&&
}
void storeMoving(Buffer&& value) {
    storage.push_back(std::move(value)); // std::move vraća rvalue -> move
}

void s05_namedRvalueReference() {
    std::cout << "-- 5. imenovana rvalue referenca je lvalue --\n";
    storage.reserve(4);
    std::cout << "  storeCopying(Buffer(3)): ";
    storeCopying(Buffer(3));
    std::cout << "\n  storeMoving(Buffer(3)):  ";
    storeMoving(Buffer(3));
    std::cout << "\n  <- unutar funkcije parametar T&& je obična promenljiva sa imenom; treba std::move\n";
}

// ---------------------------------------------------------------- 6
void s06_moveFromConst() {
    std::cout << "-- 6. std::move na const objektu kopira (EMC Item 23) --\n  ";
    const Buffer frozen(5);
    Buffer copy = std::move(frozen); // const Buffer&& ne može u Buffer&&, ali može u const Buffer& -> kopija
    std::cout << "copy=" << copy.size() << " frozen=" << frozen.size() << "  <- tiho kopirano, bez greške\n";
}

// ---------------------------------------------------------------- 7
void s07_movedFromState() {
    std::cout << "-- 7. moved-from stanje: ispravno, ali nepoznato --\n";
    std::string text(40, 'x');
    std::string other = std::move(text);
    text = "nova vrednost"; // ✅ dodela uvek radi na moved-from objektu
    std::cout << "  moved-from string posle dodele: \"" << text << "\"\n";

    auto owner = std::make_unique<int>(42);
    auto newOwner = std::move(owner);
    std::cout << "  moved-from unique_ptr je GARANTOVANO nullptr: " << (owner == nullptr ? "da" : "ne")
              << ", *newOwner=" << *newOwner << "  (*owner je UB, ub/u02)\n";
}

// ---------------------------------------------------------------- 8
void s08_moveIsNotAlwaysCheap() {
    std::cout << "-- 8. move nije uvek jeftin (EMC Item 29) --\n";
    std::string longText(100, 'x');
    const char* longBuffer = longText.data();
    std::string longMoved = std::move(longText);
    std::string shortText = "kratko"; // stane u sam string objekat (SSO): nema bafera na heap-u
    const char* shortBuffer = shortText.data();
    std::string shortMoved = std::move(shortText);
    std::cout << "  dugačak string: isti bafer posle move-a: " << (longMoved.data() == longBuffer ? "da" : "ne")
              << "; kratak (SSO): " << (shortMoved.data() == shortBuffer ? "da" : "ne") << "  <- znakovi se kopiraju\n";
    std::array<int, 1000> big{};
    std::array<int, 1000> movedBig = std::move(big); // std::array nema bafer: move = kopija 1000 int-ova
    std::cout << "  std::array<int, 1000>: move kopira " << movedBig.size() * sizeof(int) << " bajtova\n";
}

// ---------------------------------------------------------------- 9
void s09_moveOnlyTypes() {
    std::cout << "-- 9. tipovi koji se samo pomeraju --\n";
    std::vector<std::unique_ptr<int>> owners;
    auto p = std::make_unique<int>(7);
    owners.push_back(std::move(p)); // push_back(p) se ne kompajlira (errors/e02)
    owners.push_back(std::make_unique<int>(8)); // privremeni: move sam od sebe
    std::cout << "  vector<unique_ptr<int>>: " << *owners[0] << " " << *owners[1] << ", p posle move: "
              << (p ? "nije prazan" : "prazan") << "\n";
}

int main() {
    s01_valueCategories();
    s02_referenceBinding();
    s03_moveIsACast();
    s04_moveOperations();
    s05_namedRvalueReference();
    s06_moveFromConst();
    s07_movedFromState();
    s08_moveIsNotAlwaysCheap();
    s09_moveOnlyTypes();
}
