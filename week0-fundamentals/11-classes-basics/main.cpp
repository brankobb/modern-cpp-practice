#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

// Klase: osnove -- ISPRAVNI slučajevi. Sve se kompajlira i radi bez
// ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh week0-fundamentals/11-classes-basics  proverava oba.
// Neki pogrešni slučajevi su već u ranijim lekcijama i ovde se samo navode
// (npr. lekcija 03: explicit, const i referenca kao članovi).

// ---------------------------------------------------------------- 1
struct Point { // struct: podrazumevano public. Nema invarijante, svaka kombinacija x, y je ispravna.
    int x;
    int y;
};

class Account { // class: podrazumevano private. Ima invarijantu: balance_ >= 0.
public:
    bool withdraw(int amount) {
        if (amount <= 0 || amount > balance_) return false; // interfejs čuva invarijantu
        balance_ -= amount;
        return true;
    }
    void deposit(int amount) {
        if (amount > 0) balance_ += amount;
    }
    int balance() const { return balance_; }

    // Pristup je po KLASI, ne po objektu: funkcija člana vidi private
    // podatke i DRUGOG objekta iste klase.
    bool richerThan(const Account& other) const { return balance_ > other.balance_; }

private:
    int balance_ = 0; // da je public, bilo ko bi mogao da napiše a.balance_ = -100 (errors/e01)
};

void s01_classAndStruct() {
    std::cout << "-- 1. class vs struct, enkapsulacija --\n";
    Point p{1, 2}; // struct bez invarijante: agregat, direktan pristup je u redu
    std::cout << "  Point{1, 2}: x=" << p.x << " y=" << p.y << "\n";

    Account a;
    Account b;
    a.deposit(100);
    bool ok = a.withdraw(30);
    bool tooMuch = a.withdraw(500);
    b.deposit(10);
    std::cout << std::boolalpha << "  deposit(100), withdraw(30)=" << ok << ", withdraw(500)=" << tooMuch
              << " -> balance=" << a.balance() << "\n";
    std::cout << "  a.richerThan(b)=" << a.richerThan(b) << "  <- čita b.balance_, iako je private\n"
              << std::noboolalpha;
}

// ---------------------------------------------------------------- 2
class Temperature {
public:
    Temperature() : celsius_(0.0) {}                            // podrazumevani konstruktor
    explicit Temperature(double celsius) : celsius_(celsius) {} // explicit: nema tihe konverzije iz double
    double celsius() const { return celsius_; }

private:
    double celsius_;
};

class Sensor {
public:
    // Init lista (": id_(id), ...") INICIJALIZUJE članove. Dodela u telu
    // konstruktora bi ih prvo default-inicijalizovala pa dodelila, a za
    // const i reference to ni ne može (lekcija 03, errors/e20, e21).
    Sensor(int id, const std::string& location, int& readings)
        : id_(id), location_(location), readings_(readings) {}

    void read() { ++readings_; }
    std::string describe() const { return "senzor " + std::to_string(id_) + " @ " + location_; }

private:
    const int id_;          // const član: samo kroz init listu
    std::string location_;
    int& readings_;         // referenca kao član: samo kroz init listu
};

// Članovi se inicijalizuju redom kojim su DEKLARISANI u klasi, bez obzira na
// redosled u init listi. Zato ovde text_ mora biti pre decorated_.
class Label {
public:
    explicit Label(const std::string& text) : text_(text), decorated_("[" + text_ + "]") {}
    const std::string& decorated() const { return decorated_; }

private:
    std::string text_;      // 1. -- decorated_ ga koristi, pa mora biti prvi
    std::string decorated_; // 2.
};

void s02_constructors() {
    std::cout << "-- 2. konstruktori i init lista --\n";
    Temperature zero;           // podrazumevani konstruktor
    Temperature warm(21.5);     // Temperature warm = 21.5; ne radi zbog explicit (lekcija 03, errors/e06)
    Temperature cold{-3.0};
    std::cout << "  Temperature: zero=" << zero.celsius() << " warm=" << warm.celsius() << " cold=" << cold.celsius() << "\n";
    // Temperature bad(); -- NIJE objekat nego deklaracija funkcije (most vexing parse, lekcija 03)

    int readings = 0;
    Sensor s(7, "kuhinja", readings);
    s.read();
    s.read();
    std::cout << "  " << s.describe() << ", readings=" << readings << " (član je referenca na spoljni brojač)\n";

    Label label("Ana");
    std::cout << "  Label: " << label.decorated() << "  <- text_ je deklarisan pre decorated_\n";
}

// ---------------------------------------------------------------- 3
class Noisy {
public:
    explicit Noisy(std::string name) : name_(std::move(name)) { std::cout << name_ << "() "; }
    ~Noisy() { std::cout << "~" << name_ << "() "; }

private:
    std::string name_;
};

class Room {
public:
    Room() : lamp_("lamp"), desk_("desk") { std::cout << "Room() "; }
    ~Room() { std::cout << "~Room() "; }

private:
    Noisy lamp_; // članovi se prave redom deklaracije, pre tela konstruktora
    Noisy desk_; // i uništavaju obrnutim redom, posle tela destruktora
};

void s03_destructor() {
    std::cout << "-- 3. destruktor i redosled --\n";
    std::cout << "  lokalne promenljive: ";
    {
        Noisy first("a");
        Noisy second("b");
    } // uništavaju se obrnutim redom: prvo b, pa a
    std::cout << "\n  Room: ";
    {
        Room room;
        std::cout << "| ";
    }
    std::cout << "\n";
}

// ---------------------------------------------------------------- 4
class Window {
public:
    Window() = default;                           // koristi podrazumevane vrednosti ispod
    explicit Window(int width) : width_(width) {} // init lista pobeđuje podrazumevanu vrednost
    std::string describe() const {
        return std::to_string(width_) + "x" + std::to_string(height_) + " \"" + title_ + "\"";
    }

private:
    int width_ = 800;             // NSDMI (C++11): podrazumevana vrednost u deklaraciji
    int height_{600};             // može i sa {}, ali NE sa (): int height_(600); (lekcija 03, errors/e09)
    std::string title_ = "bez naslova";
};

void s04_memberInitializers() {
    std::cout << "-- 4. podrazumevane vrednosti članova (NSDMI) --\n";
    Window a;
    Window b(1024);
    std::cout << "  Window()=" << a.describe() << "  Window(1024)=" << b.describe() << "\n";
}

// ---------------------------------------------------------------- 5
class Builder {
public:
    Builder& add(int n) { // vraća REFERENCU na sebe -> chaining radi na istom objektu
        total_ += n;
        return *this;
    }
    int total() const {
        static_assert(std::is_same_v<decltype(this), const Builder*>, "u const funkciji this je const Builder*");
        return total_;
    }

private:
    int total_ = 0;
};

class BrokenBuilder {
public:
    BrokenBuilder add(int n) { // NE RADI OVAKO: vraća KOPIJU
        total_ += n;
        return *this;
    }
    int total() const { return total_; }

private:
    int total_ = 0;
};

void s05_this() {
    std::cout << "-- 5. this i method chaining --\n";
    Builder good;
    good.add(1).add(2).add(3);
    BrokenBuilder broken;
    broken.add(1).add(2).add(3); // add(2) i add(3) menjaju privremene kopije
    std::cout << "  Builder& add():      total=" << good.total() << "\n";
    std::cout << "  BrokenBuilder add(): total=" << broken.total() << "  <- samo prvi add je promenio objekat\n";
}

// ---------------------------------------------------------------- 6
class Connection {
public:
    Connection() : id_(++created_) { ++alive_; }
    ~Connection() { --alive_; }
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    static int alive() { return alive_; } // static funkcija: nema this, vidi samo static članove (errors/e03)
    int id() const { return id_; }

    static constexpr int kMaxConnections = 8; // static constexpr: implicitno inline (C++17)

private:
    int id_;
    inline static int created_ = 0; // C++17: definicija u klasi (lekcija 06, errors/e11)
    inline static int alive_ = 0;
};

void s06_staticMembers() {
    std::cout << "-- 6. static članovi --\n";
    std::cout << "  pre: alive=" << Connection::alive() << " (static postoji i bez ijednog objekta)\n";
    {
        Connection a;
        Connection b;
        Connection c;
        std::cout << "  unutra: alive=" << Connection::alive() << ", c.id()=" << c.id()
                  << ", max=" << Connection::kMaxConnections << "\n";
    }
    std::cout << "  posle: alive=" << Connection::alive() << "\n";
}

// ---------------------------------------------------------------- 7
class Inventory {
public:
    int count() const { return count_; } // const: ne menja objekat, sme na const objektu
    void add(int n) { count_ += n; }     // nije const: ne sme na const objektu (lekcija 07, errors/e04)

private:
    int count_ = 0;
};

void s07_constMemberFunctions() {
    std::cout << "-- 7. const member funkcije (detaljno u lekciji 07) --\n";
    Inventory inv;
    inv.add(5);
    const Inventory& view = inv;
    std::cout << "  preko const reference: view.count()=" << view.count() << " (view.add(1) se ne bi kompajliralo)\n";
}

// ---------------------------------------------------------------- 8
class Name {
public:
    explicit Name(const char* text) : data_(new char[std::strlen(text) + 1]) { std::strcpy(data_, text); }

    // DUBOKA kopija: nova memorija i kopija SADRŽAJA. Kompajlerov copy
    // konstruktor bi kopirao samo POKAZIVAČ (plitka kopija), pa bi dva
    // objekta obrisala istu memoriju (ub/u02).
    Name(const Name& other) : data_(new char[std::strlen(other.data_) + 1]) {
        std::strcpy(data_, other.data_);
        ++copies;
    }
    Name& operator=(const Name&) = delete; // dodela je tema week1 s02 (rule of 3)
    ~Name() { delete[] data_; }

    const char* c_str() const { return data_; }
    void setFirst(char c) { data_[0] = c; }

    static int copies;

private:
    char* data_;
};
int Name::copies = 0;

void takeByValue(Name n) { (void)n; }      // parametar po vrednosti -> copy konstruktor
void takeByRef(const Name& n) { (void)n; } // referenca -> bez kopije

void s08_copyConstructor() {
    std::cout << "-- 8. copy konstruktor --\n";
    Name a("Ana");
    Name b = a;   // copy konstruktor (NIJE dodela: b tek nastaje)
    Name c(a);    // copy konstruktor
    takeByValue(a);
    takeByRef(a);
    std::cout << "  Name b = a; Name c(a); takeByValue(a); takeByRef(a) -> kopija=" << Name::copies << "\n";
    b.setFirst('I');
    std::cout << "  posle b.setFirst('I'): a=" << a.c_str() << " b=" << b.c_str() << " c=" << c.c_str()
              << "  <- duboka kopija, a se ne menja\n";
}

// ---------------------------------------------------------------- 9
class Rect {
public:
    Rect(int width, int height) : width_(width), height_(height) { // "glavni" konstruktor
        if (width <= 0 || height <= 0) throw std::invalid_argument("dimenzije moraju biti > 0");
    }
    Rect() : Rect(1, 1) {}                    // delegira: ista provera, bez ponavljanja koda
    explicit Rect(int side) : Rect(side, side) {}
    int area() const { return width_ * height_; }

private:
    int width_;
    int height_;
};

class Tracked {
public:
    explicit Tracked(int v) : value_(v) { std::cout << "ciljni gotov "; }
    Tracked() : Tracked(0) { // posle ciljnog konstruktora objekat se smatra napravljenim
        std::cout << "telo delegirajućeg baca ";
        throw std::runtime_error("greška");
    }
    ~Tracked() { std::cout << "~Tracked(" << value_ << ") "; } // zato se destruktor POZIVA ako telo baci

private:
    int value_;
};

void s09_delegatingConstructors() {
    std::cout << "-- 9. delegirajući konstruktori (C++11) --\n";
    Rect unit;
    Rect square(3);
    Rect wide(4, 2);
    std::cout << "  Rect()=" << unit.area() << " Rect(3)=" << square.area() << " Rect(4, 2)=" << wide.area() << "\n";
    try {
        Rect bad(0, 5);
    } catch (const std::invalid_argument& e) {
        std::cout << "  Rect(0, 5) -> " << e.what() << "\n";
    }
    std::cout << "  Tracked(): ";
    try {
        Tracked t;
    } catch (const std::runtime_error&) {
        std::cout << "| uhvaćen\n";
    }
}

// ---------------------------------------------------------------- 10
struct Defaulted {
    int a;
    Defaulted() = default; // kompajlerov konstruktor: T{} daje nule
};
struct UserProvided {
    int a;
    UserProvided() {} // tvoj (prazan) konstruktor: T{} NE daje nule, a je neodređen
};

class Token {
public:
    explicit Token(int v) : value_(v) {}
    Token() = default;                        // vrati podrazumevani, koji je nestao zbog Token(int)
    Token(const Token&) = delete;             // zabrani kopiranje (EC++ Item 6, EMC Item 11)
    Token& operator=(const Token&) = delete;
    int value() const { return value_; }

private:
    int value_ = -1;
};

void s10_defaultAndDelete() {
    std::cout << "-- 10. = default i = delete (C++11) --\n";
    Defaulted d{};
    std::cout << "  Defaulted{}.a=" << d.a << " (value-init -> 0)\n";
    std::cout << std::boolalpha << "  trivijalan podrazumevani konstruktor: Defaulted="
              << std::is_trivially_default_constructible_v<Defaulted>
              << " UserProvided=" << std::is_trivially_default_constructible_v<UserProvided> << "\n";
    // UserProvided u{}; pa u.a -- čitanje neodređene vrednosti, zato se ovde ne ispisuje.

    Token t;
    Token u(42);
    std::cout << "  Token()=" << t.value() << " Token(42)=" << u.value()
              << ", kopiranje: " << std::is_copy_constructible_v<Token> << " (Token c = u; se ne kompajlira, errors/e08)\n"
              << std::noboolalpha;
}

int main() {
    s01_classAndStruct();
    s02_constructors();
    s03_destructor();
    s04_memberInitializers();
    s05_this();
    s06_staticMembers();
    s07_constMemberFunctions();
    s08_copyConstructor();
    s09_delegatingConstructors();
    s10_defaultAndDelete();
}
