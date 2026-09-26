// Rešenje zadatka ex1_trace_order.

#include <iostream>

class Trace {
public:
    explicit Trace(const char* name) : name_(name) { std::cout << ' ' << name_ << "()"; }
    ~Trace() { std::cout << " ~" << name_ << "()"; }
    Trace(const Trace&) = delete;
    Trace& operator=(const Trace&) = delete;

private:
    const char* name_;
};

// Redosled: baza, pa članovi REDOM DEKLARACIJE, pa telo. Uništavanje
// obrnuto. Init lista ne menja redosled (zato je napisana istim redom kao
// deklaracije, da se čita kako se izvršava).
class Engine : public Trace {
public:
    Engine() : Trace("base"), filter_("filter"), pump_("pump") { std::cout << " body"; }
    ~Engine() { std::cout << " ~body"; }

private:
    Trace filter_;
    Trace pump_;
};

int main() {
    std::cout << "block:";
    {
        Trace a("a");
        Trace b("b");
    }
    std::cout << "\narray:";
    {
        // Elementi niza se prave od prvog, a uništavaju od poslednjeg.
        // Trace nema kopiju, a ovo se ipak kompajlira: od C++17 prvalue
        // Trace("x0") direktno inicijalizuje element (bez kopije i move-a).
        Trace arr[] = {Trace("x0"), Trace("x1"), Trace("x2")};
    }
    std::cout << "\nclass:";
    {
        Engine e;
    }
    std::cout << "\ntemporary:";
    {
        // Privremeni objekat se uništava na kraju PUNOG izraza -- posle
        // zareza (operator ,) je i dalje živ.
        Trace("tmp"), std::cout << " same-expression";
        std::cout << " next-expression";
    }
    std::cout << '\n';
}
