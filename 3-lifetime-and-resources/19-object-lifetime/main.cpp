#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

// Lekcija 19 -- životni vek objekta: ISPRAVNI slučajevi. Sve se kompajlira i
// radi bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   ub/  -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 3-lifetime-and-resources/19-object-lifetime  proverava ih.
//
// Svaki objekat se javlja iz konstruktora i destruktora, pa ispis pokazuje
// TAČAN trenutak kad objekat počinje i prestaje da živi.

struct Tracer {
    explicit Tracer(std::string name) : name_(std::move(name)) { std::cout << name_ << "() "; }
    ~Tracer() { std::cout << "~" << name_ << "() "; }
    Tracer(const Tracer&) = delete; // da se kopije ne bi pojavljivale u ispisu
    Tracer& operator=(const Tracer&) = delete;
    const std::string& name() const { return name_; }

private:
    std::string name_;
};

// ---------------------------------------------------------------- 1
// static trajanje: pravi se PRE main, uništava POSLE main (vidi kraj ispisa).
Tracer global("global");

Tracer& counterOwner() {
    static Tracer owner("static-local"); // pravi se pri PRVOM pozivu funkcije
    return owner;
}

void s01_storageDuration() {
    std::cout << "-- 1. storage duration: automatic, static, dynamic --\n  ";
    {
        Tracer local("automatic");            // do kraja bloka
        auto heap = std::make_unique<Tracer>("dynamic"); // dok ga neko ne obriše (ovde unique_ptr)
        std::cout << "| ";
    }
    std::cout << "\n  first call to counterOwner(): ";
    counterOwner();
    std::cout << "\n  second call to counterOwner(): ";
    counterOwner(); // ne pravi se ponovo
    std::cout << "(nothing -- it already exists)\n";
}

// ---------------------------------------------------------------- 2
void s02_scopeOrder() {
    std::cout << "-- 2. order in a block and in an array --\n  ";
    {
        Tracer a("a");
        Tracer b("b");
        std::cout << "| ";
    } // obrnutim redom: b pa a
    std::cout << "\n  array: ";
    {
        Tracer arr[3] = {Tracer("x0"), Tracer("x1"), Tracer("x2")}; // C++17: bez kopije (garantovan elision)
        std::cout << "| ";
    } // elementi niza: od poslednjeg ka prvom
    std::cout << "\n";
}

// ---------------------------------------------------------------- 3
Tracer makeTracer(const char* name) { return Tracer(name); } // C++17: bez kopije

void s03_temporaries() {
    std::cout << "-- 3. temporary objects --\n";
    std::cout << "  unnamed: ";
    std::cout << Tracer("temp").name().size() << " letters "; // privremeni živi do kraja CELOG izraza (;)
    std::cout << "| after ;\n";

    std::cout << "  bound to const&: ";
    {
        const Tracer& ref = makeTracer("extended"); // život se produžava do kraja scope-a reference
        std::cout << "using " << ref.name() << " | ";
    }
    std::cout << "\n  (extension applies only to DIRECT binding, lesson 04 section 8)\n";
}

// ---------------------------------------------------------------- 4
struct Engine : Tracer {
    Engine() : Tracer("Engine-base"), pump_("pump"), filter_("filter") { std::cout << "Engine-body "; }
    ~Engine() { std::cout << "~Engine-body "; }

private:
    Tracer pump_;   // članovi: redom DEKLARACIJE, ne redom u init listi
    Tracer filter_;
};

void s04_basesAndMembers() {
    std::cout << "-- 4. base, members, body --\n  ";
    {
        Engine e;
        std::cout << "| ";
    }
    std::cout << "\n  (construction: base -> members in declaration order -> body; destruction in reverse)\n";
}

// ---------------------------------------------------------------- 5
struct Fragile {
    Fragile() : first_("first"), second_("second") {
        std::cout << "body-throws ";
        throw std::runtime_error("constructor failed");
    }
    ~Fragile() { std::cout << "~Fragile "; } // NIKAD se ne poziva: objekat nije napravljen

private:
    Tracer first_;
    Tracer second_;
};

void s05_exceptionInConstructor() {
    std::cout << "-- 5. exception in a constructor --\n  ";
    try {
        Fragile f;
    } catch (const std::runtime_error& e) {
        std::cout << "| caught: " << e.what() << "\n";
    }
    std::cout << "  <- members already constructed are destroyed, ~Fragile is not called.\n"
              << "     That is why a resource in a constructor should be held by a MEMBER with a destructor (lesson 21, RAII).\n";
}

// ---------------------------------------------------------------- 6
void s06_exitSkipsLocals() {
    std::cout << "-- 6. std::exit: locals are NOT destroyed, statics are --\n  ";
    Tracer local("local-before-exit");
    std::cout << "| std::exit(0): ";
    std::exit(0); // ne vraća se: nema unwinding-a; posle toga idu static destruktori
}

int main() {
    std::cout << "\n"; // global je već napravljen, pre prve linije main-a
    s01_storageDuration();
    s02_scopeOrder();
    s03_temporaries();
    s04_basesAndMembers();
    s05_exceptionInConstructor();
    s06_exitSkipsLocals();
}
