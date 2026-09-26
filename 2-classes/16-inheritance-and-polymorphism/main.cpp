#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Nasleđivanje i polimorfizam -- ISPRAVNI slučajevi. Sve se kompajlira i
// radi bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 2-classes/16-inheritance-and-polymorphism  proverava oba.

// ---------------------------------------------------------------- 1
class Account {
public:
    explicit Account(std::string owner) : owner_(std::move(owner)) {}
    virtual ~Account() = default;
    const std::string& owner() const { return owner_; }
    int balance() const { return balance_; }
    void deposit(int amount) { balance_ += amount; }

protected:
    int balance_ = 0; // protected: vidi ga izvedena klasa, ali ne i ostatak programa

private:
    std::string owner_; // private: ne vidi ga ni izvedena klasa (errors/e06)
};

// public nasleđivanje = "is-a": SavingsAccount JESTE Account i može se
// koristiti svuda gde se očekuje Account (EC++ Item 32).
class SavingsAccount : public Account {
public:
    SavingsAccount(std::string owner, int ratePercent) : Account(std::move(owner)), ratePercent_(ratePercent) {}
    void addInterest() { balance_ += balance_ * ratePercent_ / 100; } // pristup protected članu

private:
    int ratePercent_;
};

int totalBalance(const std::vector<const Account*>& accounts) {
    int total = 0;
    for (const Account* a : accounts) total += a->balance();
    return total;
}

void s01_basics() {
    std::cout << "-- 1. public inheritance: is-a --\n";
    SavingsAccount savings("Ann", 10);
    savings.deposit(1000);  // nasleđena funkcija
    savings.addInterest();  // sopstvena funkcija
    Account plain("Bob");
    plain.deposit(50);
    // SavingsAccount* se sam pretvara u Account* (izvedena -> bazna).
    std::cout << "  " << savings.owner() << ": " << savings.balance() << ", total via const Account*: "
              << totalBalance({&savings, &plain}) << "\n";
}

// ---------------------------------------------------------------- 2
class Part {
public:
    explicit Part(const char* name) : name_(name) { std::cout << name_ << "() "; }
    ~Part() { std::cout << "~" << name_ << "() "; }

private:
    const char* name_;
};

class Vehicle {
public:
    explicit Vehicle(int wheels) : wheels_(wheels), frame_("frame") { std::cout << "Vehicle(" << wheels_ << ") "; }
    virtual ~Vehicle() { std::cout << "~Vehicle() "; }
    int wheels() const { return wheels_; }

private:
    int wheels_;
    Part frame_;
};

class Bike : public Vehicle {
public:
    Bike() : Vehicle(2), bell_("bell") { std::cout << "Bike() "; } // baza se inicijalizuje u init listi (errors/e09)
    ~Bike() override { std::cout << "~Bike() "; }

private:
    Part bell_;
};

class Truck : public Vehicle {
public:
    using Vehicle::Vehicle; // C++11: nasledi konstruktore baze (Truck(int))
};

void s02_constructionOrder() {
    std::cout << "-- 2. construction and destruction order --\n";
    std::cout << "  ";
    {
        Bike bike; // baza (sa svojim članovima) -> članovi izvedene -> telo izvedene
        std::cout << "| ";
    } // obrnuto: telo izvedene -> članovi izvedene -> baza
    std::cout << "\n  Truck(6) via using Vehicle::Vehicle: ";
    {
        Truck truck(6);
        std::cout << "wheels=" << truck.wheels() << " | ";
    }
    std::cout << "\n";
}

// ---------------------------------------------------------------- 3
class Logger {
public:
    virtual ~Logger() = default;
    std::string log(int value) const { return "Logger::log(int " + std::to_string(value) + ")"; }
    std::string log(const std::string& text) const { return "Logger::log(string " + text + ")"; }
};

class HidingLogger : public Logger {
public:
    // Novo ime log u izvedenoj klasi SAKRIJE SVE log iz baze, i one sa
    // drugim parametrima (errors/e08). Nije overload preko granice klase.
    std::string log(double value) const { return "HidingLogger::log(double " + std::to_string(value) + ")"; }
};

class FileLogger : public Logger {
public:
    using Logger::log; // vrati imena iz baze u ovaj scope -> sada su svi overload-i zajedno
    std::string log(double value) const { return "FileLogger::log(double " + std::to_string(value) + ")"; }
};

void s03_nameHiding() {
    std::cout << "-- 3. name hiding and using Base::f --\n";
    HidingLogger hiding;
    FileLogger file;
    std::cout << "  HidingLogger.log(5) -> " << hiding.log(5) << "  <- int -> double, Logger::log(int) is not visible\n";
    std::cout << "  FileLogger.log(5)   -> " << file.log(5) << "\n";
    std::cout << "  FileLogger.log(\"x\") -> " << file.log(std::string("x")) << "\n";
}

// ---------------------------------------------------------------- 4
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;            // pure virtual: Shape je apstraktna (errors/e02)
    virtual std::string name() const { return "Shape"; }
    std::string describe() const {               // nije virtual, ali poziva virtual funkcije
        return name() + " with area " + std::to_string(static_cast<int>(area()));
    }
};

class Circle : public Shape {
public:
    explicit Circle(double r) : r_(r) {}
    double area() const override { return 3.14159 * r_ * r_; } // override: kompajler proveri potpis (errors/e03)
    std::string name() const override { return "Circle"; }

private:
    double r_;
};

class Square final : public Shape { // final: od Square se ne može dalje nasleđivati (errors/e04)
public:
    explicit Square(double side) : side_(side) {}
    double area() const override { return side_ * side_; }
    std::string name() const override { return "Square"; }

private:
    double side_;
};

void s04_virtualDispatch() {
    std::cout << "-- 4. virtual, override, final --\n";
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(2.0));
    shapes.push_back(std::make_unique<Square>(3.0));
    for (const auto& s : shapes) {
        std::cout << "  " << s->describe() << "\n"; // poziva se funkcija STVARNOG tipa objekta
    }
    Circle c(1.0);
    const Shape& ref = c;
    std::cout << "  through a reference: ref.name()=" << ref.name()
              << ", qualified ref.Shape::name()=" << ref.Shape::name() << "  <- qualification turns off virtual dispatch\n";
}

// ---------------------------------------------------------------- 5
class Resource {
public:
    virtual ~Resource() { std::cout << "~Resource() "; } // virtual destruktor (EC++ Item 7)
};

class FileResource : public Resource {
public:
    ~FileResource() override { std::cout << "~FileResource() "; }
};

void s05_virtualDestructor() {
    std::cout << "-- 5. virtual destructor --\n";
    std::cout << "  delete through Resource*: ";
    Resource* r = new FileResource;
    delete r; // zbog virtual ~Resource() poziva se prvo ~FileResource (bez njega: ub/u01)
    std::cout << "\n";
}

// ---------------------------------------------------------------- 6
class Widget {
public:
    Widget() { std::cout << "in Widget(): " << kind() << "; "; } // tokom konstrukcije baze objekat JE Widget
    virtual ~Widget() = default;
    virtual std::string kind() const { return "Widget::kind"; }
    void show() const { std::cout << "after construction: " << kind(); }
};

class Button : public Widget {
public:
    Button() : label_("OK") {}
    std::string kind() const override { return "Button::kind(" + label_ + ")"; } // koristi label_

private:
    std::string label_;
};

void s06_virtualInConstructor() {
    std::cout << "-- 6. virtual call in a constructor (EC++ Item 9) --\n";
    std::cout << "  ";
    Button b;
    b.show();
    std::cout << "\n  <- in the base constructor the Button part does not exist yet (label_ is not constructed)\n";
}

// ---------------------------------------------------------------- 7
class Plain {
public:
    int x() const { return x_; }

private:
    int x_ = 0;
};

class WithVirtual {
public:
    virtual ~WithVirtual() = default;
    int x() const { return x_; }

private:
    int x_ = 0;
};

// Čita prvih 8 bajtova objekta: kod g++ i clang (Itanium ABI) to je vptr.
// Standard ne propisuje vtable -- ovo je samo pogled "ispod haube".
std::uintptr_t vptrOf(const void* object) {
    std::uintptr_t value = 0;
    std::memcpy(&value, object, sizeof value);
    return value;
}

std::uintptr_t vptrDuringBaseConstruction = 0;

struct Machine {
    Machine() { vptrDuringBaseConstruction = vptrOf(this); } // koji vptr ima objekat dok se pravi baza?
    virtual ~Machine() = default;
    virtual const char* kind() const { return "Machine"; }
};

struct Robot : Machine {
    const char* kind() const override { return "Robot"; }
};

struct Printable {
    virtual ~Printable() = default;
};
struct Storable {
    virtual ~Storable() = default;
};
struct Record : Printable, Storable {}; // dve polimorfne baze -> dva vptr-a

void s07_vtable() {
    std::cout << "-- 7. vptr and vtable (course 106-107) --\n";
    std::cout << "  sizeof(Plain)=" << sizeof(Plain) << " sizeof(WithVirtual)=" << sizeof(WithVirtual)
              << "  <- a hidden pointer to the table of virtual functions (+ padding)\n";
    (void)Plain{}.x();
    (void)WithVirtual{}.x();

    Machine m1;
    Machine m2;
    Robot r1;
    Robot r2;
    std::cout << "  same vptr: Machine/Machine=" << (vptrOf(&m1) == vptrOf(&m2) ? "yes" : "no")
              << " Robot/Robot=" << (vptrOf(&r1) == vptrOf(&r2) ? "yes" : "no")
              << " Machine/Robot=" << (vptrOf(&m1) == vptrOf(&r1) ? "yes" : "no") << "  <- one vtable per CLASS\n";
    std::cout << "  while the Machine part of a Robot is built, vptr points to the Machine vtable: "
              << (vptrDuringBaseConstruction == vptrOf(&m1) ? "yes" : "no") << " (that is why a virtual call in a constructor goes to the base, section 6)\n";
    std::cout << "  sizeof(Printable)=" << sizeof(Printable) << " sizeof(Record : Printable, Storable)=" << sizeof(Record)
              << "  <- one vptr for each polymorphic base\n";
}

// ---------------------------------------------------------------- 8
class Animal {
public:
    virtual ~Animal() = default;
    virtual std::string speak() const { return "..."; }
};

class Dog : public Animal {
public:
    explicit Dog(std::string name) : name_(std::move(name)) {}
    std::string speak() const override { return name_ + ": Woof!"; }

private:
    std::string name_; // ovaj podatak se gubi pri slicing-u
};

std::string byValue(Animal a) { return a.speak(); }      // NE RADI OVAKO: kopira samo Animal deo
std::string byRef(const Animal& a) { return a.speak(); } // bez kopije: pravi tip ostaje

void s08_slicing() {
    std::cout << "-- 8. slicing --\n";
    Dog rex("Rex");
    std::cout << "  byValue(rex): " << byValue(rex) << "   <- sliced down to Animal\n";
    std::cout << "  byRef(rex):   " << byRef(rex) << "\n";

    Animal copy = rex; // kopija u bazni objekat -- isto
    std::cout << "  Animal copy = rex; copy.speak(): " << copy.speak() << "\n";

    std::vector<Animal> zoo;
    zoo.push_back(rex); // kontejner baznih objekata čuva samo bazne delove
    std::cout << "  vector<Animal>[0].speak(): " << zoo[0].speak() << "\n";

    std::vector<std::unique_ptr<Animal>> realZoo; // ispravno: čuvaj pokazivače
    realZoo.push_back(std::make_unique<Dog>("Max"));
    std::cout << "  vector<unique_ptr<Animal>>[0]->speak(): " << realZoo[0]->speak() << "\n";
    // Da je Animal zabranio kopiranje (C.67), byValue i Animal copy = rex se
    // ne bi kompajlirali (errors/e01).
}

// ---------------------------------------------------------------- 9
class Document {
public:
    virtual ~Document() = default;
    Document& operator=(const Document&) = delete; // dodela bi odsekla (C.67)
    virtual std::unique_ptr<Document> clone() const = 0; // "virtual copy konstruktor" (C.130)
    virtual std::string kind() const = 0;

protected:
    Document() = default;
    Document(const Document&) = default; // samo izvedene klase smeju da kopiraju bazni deo
};

class Report : public Document {
public:
    explicit Report(std::string title) : title_(std::move(title)) {}
    std::unique_ptr<Document> clone() const override { return std::make_unique<Report>(*this); }
    std::string kind() const override { return "Report(" + title_ + ")"; }

private:
    std::string title_;
};

void s09_clone() {
    std::cout << "-- 9. copying a polymorphic object: clone() --\n";
    std::unique_ptr<Document> original = std::make_unique<Report>("Q3");
    std::unique_ptr<Document> copy = original->clone(); // kopija PRAVOG tipa, bez slicing-a
    std::cout << "  original->clone()->kind() = " << copy->kind() << "\n";
}

// ---------------------------------------------------------------- 10
struct Device {
    explicit Device(int id) : id_(id) { std::cout << "Device(" << id << ") "; }
    int id_;
};
// virtual nasleđivanje: Printer i Scanner dele JEDAN Device podobjekat.
struct Printer : virtual Device {
    Printer() : Device(1) {}
};
struct Scanner : virtual Device {
    Scanner() : Device(2) {}
};
struct Copier : Printer, Scanner {
    Copier() : Device(3) {} // virtual bazu pravi NAJIZVEDENIJA klasa; Device(1) i Device(2) se ignorišu
};

void s10_multipleInheritance() {
    std::cout << "-- 10. multiple inheritance and the diamond --\n";
    std::cout << "  ";
    Copier c;
    std::cout << "| c.id_=" << c.id_ << " (one Device; without virtual there would be two and c.id_ would be ambiguous, errors/e12)\n";
}

// ---------------------------------------------------------------- 11
class Engine {
public:
    std::string start() const { return "engine running"; }
};

class Car { // kompozicija: Car IMA Engine (EC++ Item 38) -- obično bolje od private nasleđivanja
public:
    std::string drive() const { return engine_.start() + ", car driving"; }

private:
    Engine engine_;
};

class Scooter : private Engine { // private nasleđivanje: "implementirano pomoću" (EC++ Item 39)
public:
    std::string ride() const { return start() + ", scooter riding"; }
};

void s11_compositionVsPrivate() {
    std::cout << "-- 11. composition vs private inheritance --\n";
    Car car;
    Scooter scooter;
    std::cout << "  Car::drive(): " << car.drive() << "; Scooter::ride(): " << scooter.ride()
              << "  (Engine& e = scooter; does not compile, errors/e07)\n";
}

// ---------------------------------------------------------------- 12
// Interfejs: samo pure virtual funkcije, bez podataka.
class Sink {
public:
    virtual ~Sink() = 0; // pure virtual destruktor: klasa je apstraktna i bez drugih = 0 funkcija
    virtual void log(const std::string& message) const = 0;
};

Sink::~Sink() = default; // MORA da postoji: izvedeni destruktor ga poziva (bez ovoga: errors/e13)

// Pure virtual funkcija SME da ima telo: podrazumevano ponašanje koje
// izvedena klasa mora eksplicitno da izabere.
void Sink::log(const std::string& message) const { std::cout << "[default] " << message; }

class ConsoleSink : public Sink {
public:
    void log(const std::string& message) const override {
        std::cout << "[console] ";
        Sink::log(message); // poziv tela pure virtual funkcije -- samo kvalifikovano
    }
};

void s12_abstractClasses() {
    std::cout << "-- 12. abstract classes and interfaces (course 112) --\n";
    ConsoleSink console;
    const Sink& sink = console;
    std::cout << "  ";
    sink.log("message");
    std::cout << "\n  <- Sink has a pure virtual destructor (with a definition) and a pure virtual log (with a body)\n";
}

int main() {
    s01_basics();
    s02_constructionOrder();
    s03_nameHiding();
    s04_virtualDispatch();
    s05_virtualDestructor();
    s06_virtualInConstructor();
    s07_vtable();
    s08_slicing();
    s09_clone();
    s10_multipleInheritance();
    s11_compositionVsPrivate();
    s12_abstractClasses();
}
