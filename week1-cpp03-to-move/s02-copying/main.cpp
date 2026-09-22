#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

// Sesija 2 -- kopiranje (C++03 pravila, koja važe i danas): ISPRAVNI
// slučajevi. Sve se kompajlira i radi bez ASan/UBSan prijava (g++ 13 i
// clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh week1-cpp03-to-move/s02-copying  proverava oba.

// ---------------------------------------------------------------- 1
struct Person { // nijedna specijalna funkcija nije napisana -- kompajler piše sve (EC++ Item 5)
    std::string name;
    std::vector<int> scores;
};

void s01_generatedMembers() {
    std::cout << "-- 1. šta kompajler sam napiše --\n";
    std::cout << std::boolalpha << "  Person: default ctor=" << std::is_default_constructible_v<Person>
              << " copy ctor=" << std::is_copy_constructible_v<Person>
              << " copy dodela=" << std::is_copy_assignable_v<Person>
              << " destruktor=" << std::is_destructible_v<Person> << "\n" << std::noboolalpha;

    Person a{"Ana", {90, 85}};
    Person b = a;       // kompajlerov copy ctor: kopira član po član
    b.name[0] = 'I';    // string i vector kopiraju SADRŽAJ, pa je b nezavisan
    b.scores.push_back(70);
    std::cout << "  posle izmene kopije: a=" << a.name << "/" << a.scores.size() << " ocene, b=" << b.name << "/"
              << b.scores.size() << " ocene  <- član po član je ovde duboka kopija\n";
}

// ---------------------------------------------------------------- 2
struct Counted {
    Counted() = default;
    Counted(const Counted&) { ++copies; }
    Counted& operator=(const Counted&) {
        ++assignments;
        return *this;
    }
    static void reset() { copies = assignments = 0; }
    inline static int copies = 0;
    inline static int assignments = 0;
};

void byValue(Counted) {}
void byConstRef(const Counted&) {}

void s02_whenCopied() {
    std::cout << "-- 2. kada se pravi kopija --\n";
    Counted original;

    Counted::reset();
    Counted a = original;       // inicijalizacija novog objekta -> copy ctor
    Counted b(original);        // isto
    int afterInit = Counted::copies;

    Counted::reset();
    byValue(original);          // parametar po vrednosti -> copy ctor
    byConstRef(original);       // referenca -> ništa
    int afterCalls = Counted::copies;

    Counted::reset();
    std::vector<Counted> items(3);
    for (Counted item : items) (void)item;         // range-for PO VREDNOSTI kopira svaki element
    int afterLoopByValue = Counted::copies;
    Counted::reset();
    for (const Counted& item : items) (void)item;  // po referenci: ništa
    int afterLoopByRef = Counted::copies;

    Counted::reset();
    a = b;                      // oba već postoje -> copy DODELA, ne konstruktor
    std::cout << "  Counted a = x; Counted b(x): kopija=" << afterInit << "; byValue(x) + byConstRef(x): kopija="
              << afterCalls << "\n  for (Counted item : v) sa 3 elementa: kopija=" << afterLoopByValue
              << "; for (const Counted& item : v): kopija=" << afterLoopByRef << "\n  a = b: copy konstruktor="
              << Counted::copies << ", copy dodela=" << Counted::assignments << "\n";
}

// ---------------------------------------------------------------- 3
struct Order {
    const int id; // const član: kompajlerova DODELA je obrisana (errors/e01), kopija radi
    int quantity;
};

void s03_deletedByMembers() {
    std::cout << "-- 3. kad kompajler NE može da napiše kopiju --\n";
    std::cout << std::boolalpha << "  Order{const int id}: copy ctor=" << std::is_copy_constructible_v<Order>
              << " copy dodela=" << std::is_copy_assignable_v<Order>
              << "  <- const i referenca kao član brišu dodelu (errors/e01, e02)\n" << std::noboolalpha;
    Order first{1, 5};
    Order copy = first; // konstrukcija kopije je u redu: id se INICIJALIZUJE, ne dodeljuje
    std::cout << "  Order copy = first: id=" << copy.id << " quantity=" << copy.quantity << "\n";
}

// ---------------------------------------------------------------- 4
// Klasa koja poseduje memoriju preko sirovog pokazivača: kompajlerova
// kopija bi kopirala samo POKAZIVAČ (plitka kopija, ub/u01). Zato su tri
// funkcije napisane zajedno -- rule of 3.
class Name {
public:
    explicit Name(const char* text) : data_(copyOf(text)) {}

    Name(const Name& other) : data_(copyOf(other.data_)) {} // 1) duboka kopija

    Name& operator=(const Name& other) {                    // 2) duboka dodela
        if (this == &other) return *this;  // a = a: bez ove provere bi se prvo obrisao i other.data_ (EC++ Item 11)
        char* fresh = copyOf(other.data_); // PRVO nova memorija (ako new baci, *this je netaknut, s03)
        delete[] data_;                    // pa tek onda stara
        data_ = fresh;
        return *this;
    }

    ~Name() { delete[] data_; }                             // 3) oslobađanje

    const char* c_str() const { return data_; }
    void setFirst(char c) { data_[0] = c; }

private:
    static char* copyOf(const char* text) {
        char* result = new char[std::strlen(text) + 1];
        std::strcpy(result, text);
        return result;
    }
    char* data_;
};

void s04_deepCopy() {
    std::cout << "-- 4. plitka vs duboka kopija: rule of 3 --\n";
    Name a("Ana");
    Name b = a;
    Name c("Ceca");
    c = a;
    b.setFirst('I');
    c.setFirst('E');
    Name& alias = a;
    a = alias; // dodela samom sebi
    std::cout << "  a=" << a.c_str() << " b=" << b.c_str() << " c=" << c.c_str()
              << "  <- svaki objekat ima svoju memoriju; a = a radi\n";
}

// ---------------------------------------------------------------- 5
class Base {
public:
    explicit Base(int id) : id_(id) {}
    int id() const { return id_; }

private:
    int id_;
};

class Forgetful : public Base { // NE RADI OVAKO: copy konstruktor zaboravlja bazu i član
public:
    Forgetful(int id, std::string tag) : Base(id), tag_(std::move(tag)) {}
    Forgetful(const Forgetful& other) : Base(0), extra_(other.extra_) {} // Base(0) umesto Base(other), tag_ nije kopiran
    const std::string& tag() const { return tag_; }

private:
    std::string tag_;
    int extra_ = 0;
};

class Careful : public Base { // ISPRAVNO: kopira sve delove (EC++ Item 12)
public:
    Careful(int id, std::string tag) : Base(id), tag_(std::move(tag)) {}
    Careful(const Careful& other) : Base(other), tag_(other.tag_), extra_(other.extra_) {}
    Careful& operator=(const Careful& other) {
        Base::operator=(other); // i bazni deo u dodeli
        tag_ = other.tag_;
        extra_ = other.extra_;
        return *this;
    }
    const std::string& tag() const { return tag_; }

private:
    std::string tag_;
    int extra_ = 0;
};

void s05_copyAllParts() {
    std::cout << "-- 5. kopiraj SVE delove (EC++ Item 12) --\n";
    Forgetful f(7, "hitno");
    Forgetful fCopy(f);
    Careful c(7, "hitno");
    Careful cCopy(c);
    std::cout << "  Forgetful kopija: id=" << fCopy.id() << " tag=\"" << fCopy.tag() << "\"  <- baza i tag izgubljeni, bez upozorenja\n";
    std::cout << "  Careful kopija:   id=" << cCopy.id() << " tag=\"" << cCopy.tag() << "\"\n";
    std::cout << "  (najbolje: ne piši copy konstruktor kad kompajlerov radi -- rule of 0)\n";
}

// ---------------------------------------------------------------- 6
class Connection {
public:
    explicit Connection(int port) : port_(port) {}
    Connection(const Connection&) = delete;            // jedinstven resurs: kopija nema smisla (EC++ Item 6)
    Connection& operator=(const Connection&) = delete;
    int port() const { return port_; }

private:
    int port_;
};

void s06_forbidCopy() {
    std::cout << "-- 6. zabrana kopiranja --\n";
    Connection db(5432);
    const Connection& same = db; // referenca: bez kopije
    std::cout << std::boolalpha << "  Connection: copy ctor=" << std::is_copy_constructible_v<Connection>
              << ", referenca radi: port=" << same.port() << "\n" << std::noboolalpha;
}

int main() {
    s01_generatedMembers();
    s02_whenCopied();
    s03_deletedByMembers();
    s04_deepCopy();
    s05_copyAllParts();
    s06_forbidCopy();
}
