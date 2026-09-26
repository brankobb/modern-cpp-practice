// Rešenje zadatka z1_redosled_traga.

#include <iostream>

class Trag {
public:
    explicit Trag(const char* ime) : ime_(ime) { std::cout << ' ' << ime_ << "()"; }
    ~Trag() { std::cout << " ~" << ime_ << "()"; }
    Trag(const Trag&) = delete;
    Trag& operator=(const Trag&) = delete;

private:
    const char* ime_;
};

// Redosled: baza, pa članovi REDOM DEKLARACIJE, pa telo. Uništavanje
// obrnuto. Init lista ne menja redosled (zato je napisana istim redom kao
// deklaracije, da se čita kako se izvršava).
class Motor : public Trag {
public:
    Motor() : Trag("baza"), filter_("filter"), pumpa_("pumpa") { std::cout << " telo"; }
    ~Motor() { std::cout << " ~telo"; }

private:
    Trag filter_;
    Trag pumpa_;
};

int main() {
    std::cout << "blok:";
    {
        Trag a("a");
        Trag b("b");
    }
    std::cout << "\nniz:";
    {
        // Elementi niza se prave od prvog, a uništavaju od poslednjeg.
        // Trag nema kopiju, a ovo se ipak kompajlira: od C++17 prvalue
        // Trag("x0") direktno inicijalizuje element (bez kopije i move-a).
        Trag niz[] = {Trag("x0"), Trag("x1"), Trag("x2")};
    }
    std::cout << "\nklasa:";
    {
        Motor m;
    }
    std::cout << "\nprivremeni:";
    {
        // Privremeni objekat se uništava na kraju PUNOG izraza -- posle
        // zareza (operator ,) je i dalje živ.
        Trag("tmp"), std::cout << " isti-izraz";
        std::cout << " sledeći-izraz";
    }
    std::cout << '\n';
}
