// Rešenje zadatka z3_shared_from_this.

#include <iostream>
#include <memory>
#include <vector>

struct Sesija;
std::vector<std::shared_ptr<Sesija>> registar;

// Ako praviš std::shared_ptr<Sesija>(this) (nije dobro): drugi, nezavisan
// kontrolni blok -- objekat se obriše dvaput.
// Treba ovako: enable_shared_from_this u sebi čuva weak_ptr na postojeći
// kontrolni blok (popuni ga make_shared / shared_ptr konstruktor), a
// shared_from_this() od njega napravi shared_ptr.
struct Sesija : std::enable_shared_from_this<Sesija> {
    void registruj() { registar.push_back(shared_from_this()); }
};

int main() {
    {
        auto s = std::make_shared<Sesija>();
        s->registruj();
        std::cout << "use_count: " << s.use_count() << '\n';
    }
    std::cout << "posle bloka, u registru: " << registar.size() << ", use_count: "
              << registar[0].use_count() << '\n';
    registar.clear();

    // Korak 3: bez vlasnika weak_ptr je prazan, pa shared_from_this() baca
    // (C++17; ranije je bilo UB).
    Sesija naSteku;
    try {
        naSteku.registruj();
    } catch (const std::bad_weak_ptr&) {
        std::cout << "na steku: bad_weak_ptr\n";
    }
}
