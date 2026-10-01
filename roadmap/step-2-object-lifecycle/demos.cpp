// Korak 2 -- demonstracije uz notes.md (svaka sekcija "// ----- N" prati
// sekciju N u notes.md).
//   ./build.sh roadmap/step-2-object-lifecycle/demos.cpp
//   ./build.sh roadmap/step-2-object-lifecycle/demos.cpp -DREORDER        -- -Wreorder upozorenje (sekcija 8)
//   ./build.sh roadmap/step-2-object-lifecycle/demos.cpp -DTRY_IMPLICIT   -- explicit ne pušta int (sekcija 6)

#include <cstdio>
#include <utility>

// Noisy ispisuje svaku specijalnu funkciju, pa se vidi TAČNO šta jezik poziva.
class Noisy {
public:
    Noisy() : v_{0} { std::printf("    Noisy() default ctor\n"); }
    explicit Noisy(int v) : v_{v} { std::printf("    Noisy(%d) ctor\n", v_); }
    Noisy(const Noisy& o) : v_{o.v_} { std::printf("    Noisy(const Noisy&) copy ctor (%d)\n", v_); }
    Noisy(Noisy&& o) noexcept : v_{o.v_} {
        o.v_ = 0;
        std::printf("    Noisy(Noisy&&) move ctor (%d)\n", v_);
    }
    Noisy& operator=(const Noisy& o) {
        v_ = o.v_;
        std::printf("    operator=(const Noisy&) copy assign (%d)\n", v_);
        return *this;
    }
    Noisy& operator=(Noisy&& o) noexcept {
        v_ = o.v_;
        o.v_ = 0;
        std::printf("    operator=(Noisy&&) move assign (%d)\n", v_);
        return *this;
    }
    ~Noisy() { std::printf("    ~Noisy() dtor (%d)\n", v_); }

    int value() const { return v_; }

private:
    int v_;
};

void by_value(Noisy n) { std::printf("    in by_value(%d)\n", n.value()); }
void by_cref(const Noisy& n) { std::printf("    in by_cref(%d)\n", n.value()); }
Noisy make(int v) { return Noisy{v}; }   // C++17: garantovana elizija, nema copy/move

// ----- 1. Delegirajući konstruktor
class Timer {
public:
    Timer() : Timer(1000) { std::printf("    Timer() body (delegated)\n"); }
    explicit Timer(int period_ms) : period_ms_{period_ms} { std::printf("    Timer(%d) body\n", period_ms_); }

private:
    int period_ms_;
};

// ----- 6. explicit
struct Meters {
    explicit Meters(double v) : value{v} {}
    double value;
};
struct Celsius {
    Celsius(double v) : value{v} {}   // NAMERNO bez explicit
    double value;
};
void set_distance(Meters m) { std::printf("    distance %.1f m\n", m.value); }
void set_temperature(Celsius c) { std::printf("    temperature %.1f C\n", c.value); }

// ----- 7. this, static, friend
class Sensor {
public:
    explicit Sensor(int id) : id_{id} { ++alive_; }
    ~Sensor() { --alive_; }
    Sensor(const Sensor&) = delete;
    Sensor& operator=(const Sensor&) = delete;

    Sensor& set_gain(int g) {         // vraća *this -> lančano pozivanje
        gain_ = g;
        return *this;
    }
    Sensor& set_offset(int o) {
        this->offset_ = o;            // this-> je ovde opcion
        return *this;
    }
    static int alive() { return alive_; }   // nema this: ne pripada objektu

    friend void dump(const Sensor& s);       // slobodna funkcija sa pristupom privatnom

private:
    int id_;
    int gain_ = 1;
    int offset_ = 0;
    static inline int alive_ = 0;            // jedan za sve objekte
};
void dump(const Sensor& s) { std::printf("    sensor %d: gain %d, offset %d\n", s.id_, s.gain_, s.offset_); }

// ----- 8. Inicijalizaciona lista: redosled, const i reference članovi
struct Part {
    explicit Part(const char* n) : name{n} { std::printf("    Part %s ctor\n", name); }
    ~Part() { std::printf("    Part %s dtor\n", name); }
    const char* name;
};

class Device {
public:
    // Lista je napisana istim redom kao deklaracija -- tako i treba.
    Device(int id, int& counter) : first_{"first"}, second_{"second"}, id_{id}, counter_{counter} { ++counter_; }
    int id() const { return id_; }

private:
    Part first_;          // konstruiše se PRVI (redosled deklaracije!)
    Part second_;         // pa ovaj; uništavaju se obrnuto
    const int id_;        // const član: MORA u init listu
    int& counter_;        // referenca: MORA u init listu
};

#ifdef REORDER
struct Window {
    // g++/clang: -Wreorder -- lista kaže "height_ pa width_", ali se
    // inicijalizuje redom deklaracije: width_ PRVO, iz još neinicijalizovanog height_.
    Window(int h) : height_{h}, width_{height_ * 2} {}
    int width_;
    int height_;
};
#endif

// ----- 5. Nasleđivanje: redosled, virtual, slicing
class Shape {
public:
    Shape() { std::printf("    Shape ctor\n"); }
    virtual ~Shape() { std::printf("    Shape dtor\n"); }
    virtual const char* name() const { return "shape"; }
};

class Circle final : public Shape {
public:
    Circle() { std::printf("    Circle ctor\n"); }
    ~Circle() override { std::printf("    Circle dtor\n"); }
    const char* name() const override { return "circle"; }
};

struct Plain {
    int x;
    void f() {}
};
struct WithVirtual {
    int x;
    virtual void f() {}
    virtual ~WithVirtual() = default;
};

// Slicing: Base po vrednosti "odseče" izvedeni deo.
struct Animal {
    virtual ~Animal() = default;
    virtual const char* sound() const { return "..."; }
};
struct Dog : Animal {
    const char* sound() const override { return "woof"; }
};
const char* sound_by_value(Animal a) { return a.sound(); }        // kopira SAMO Animal deo
const char* sound_by_ref(const Animal& a) { return a.sound(); }   // virtual radi

int main() {
    std::printf("== 1. which constructor is called\n");
    {
        std::printf("  Noisy a;\n");
        Noisy a;
        std::printf("  Noisy b{1};\n");
        Noisy b{1};
        std::printf("  Noisy c = b;          // NOVI objekat -> copy ctor\n");
        Noisy c = b;
        std::printf("  c = b;                // POSTOJEĆI objekat -> copy assign\n");
        c = b;
        std::printf("  Noisy d = std::move(c);\n");
        Noisy d = std::move(c);
        std::printf("  a = Noisy{2};         // privremeni: ctor, move assign, dtor privremenog\n");
        a = Noisy{2};
        std::printf("  Noisy e = Noisy{3};   // C++17: SAMO ctor (garantovana elizija)\n");
        Noisy e = Noisy{3};
        std::printf("  Noisy f = make(4);    // isto, i kroz return\n");
        Noisy f = make(4);
        std::printf("  by_value(b);          // kopija u parametar\n");
        by_value(b);
        std::printf("  by_cref(b);           // bez kopije\n");
        by_cref(b);
        std::printf("  end of scope: obrnuto od konstrukcije (f e d c b a)\n");
    }

    std::printf("== 1. delegating constructor\n");
    Timer t;
    (void)t;

    std::printf("== 5. inheritance\n");
    {
        std::printf("  Shape* s = new Circle;\n");
        Shape* s = new Circle;
        std::printf("    s->name() = %s (virtual: dinamički tip)\n", s->name());
        std::printf("  delete s;   (virtual ~Shape: oba destruktora)\n");
        delete s;
    }
    std::printf("    sizeof(WithVirtual) > sizeof(Plain): %s (vptr)\n",
                sizeof(WithVirtual) > sizeof(Plain) ? "yes" : "no");
    Dog dog;
    std::printf("    by ref: %s, by value: %s (slicing!)\n", sound_by_ref(dog), sound_by_value(dog));
    std::printf("== 6. explicit\n");
    set_distance(Meters{5.0});
    // set_distance(5.0);                    // ne kompajlira se: Meters je explicit
#ifdef TRY_IMPLICIT
    set_distance(5.0);
#endif
    set_temperature(36.6);                   // radi -- a da li si to hteo? 36.6 čega?

    std::printf("== 7. this, static, friend\n");
    {
        Sensor s1{1};
        Sensor s2{2};
        s1.set_gain(4).set_offset(-2);
        dump(s1);
        dump(s2);
        std::printf("    alive: %d\n", Sensor::alive());
    }
    std::printf("    alive after scope: %d\n", Sensor::alive());

    std::printf("== 8. member init order\n");
    {
        int counter = 0;
        Device dev{7, counter};
        std::printf("    id = %d, counter = %d\n", dev.id(), counter);
    }

}

/* EXPECTED OUTPUT
== 1. which constructor is called
  Noisy a;
    Noisy() default ctor
  Noisy b{1};
    Noisy(1) ctor
  Noisy c = b;          // NOVI objekat -> copy ctor
    Noisy(const Noisy&) copy ctor (1)
  c = b;                // POSTOJEĆI objekat -> copy assign
    operator=(const Noisy&) copy assign (1)
  Noisy d = std::move(c);
    Noisy(Noisy&&) move ctor (1)
  a = Noisy{2};         // privremeni: ctor, move assign, dtor privremenog
    Noisy(2) ctor
    operator=(Noisy&&) move assign (2)
    ~Noisy() dtor (0)
  Noisy e = Noisy{3};   // C++17: SAMO ctor (garantovana elizija)
    Noisy(3) ctor
  Noisy f = make(4);    // isto, i kroz return
    Noisy(4) ctor
  by_value(b);          // kopija u parametar
    Noisy(const Noisy&) copy ctor (1)
    in by_value(1)
    ~Noisy() dtor (1)
  by_cref(b);           // bez kopije
    in by_cref(1)
  end of scope: obrnuto od konstrukcije (f e d c b a)
    ~Noisy() dtor (4)
    ~Noisy() dtor (3)
    ~Noisy() dtor (1)
    ~Noisy() dtor (0)
    ~Noisy() dtor (1)
    ~Noisy() dtor (2)
== 1. delegating constructor
    Timer(1000) body
    Timer() body (delegated)
== 5. inheritance
  Shape* s = new Circle;
    Shape ctor
    Circle ctor
    s->name() = circle (virtual: dinamički tip)
  delete s;   (virtual ~Shape: oba destruktora)
    Circle dtor
    Shape dtor
    sizeof(WithVirtual) > sizeof(Plain): yes (vptr)
    by ref: woof, by value: ... (slicing!)
== 6. explicit
    distance 5.0 m
    temperature 36.6 C
== 7. this, static, friend
    sensor 1: gain 4, offset -2
    sensor 2: gain 1, offset 0
    alive: 2
    alive after scope: 0
== 8. member init order
    Part first ctor
    Part second ctor
    id = 7, counter = 1
    Part second dtor
    Part first dtor
*/
