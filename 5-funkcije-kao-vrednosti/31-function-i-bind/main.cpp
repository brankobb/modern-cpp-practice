#include <array>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <new>
#include <string>
#include <utility>
#include <vector>

// std::function i std::bind -- ISPRAVNI slučajevi. Sve se kompajlira bez
// upozorenja i radi bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i
// C++20). Brojevi sekcija prate notes.md.
// POGREŠNI slučajevi:
//   errors/   -- kod koji se NE kompajlira
//   ub/       -- kod koji se kompajlira, ali je undefined behavior
//   runtime/  -- kod koji se kompajlira, a program se prekine
// ./check_cases.sh 5-funkcije-kao-vrednosti/31-function-i-bind  proverava sve.

// Brojač alokacija (za sekciju 2): zamena globalnog operator new.
// Standard to dozvoljava; ovde služi samo da se VIDI kada std::function
// alocira.
static int alokacija = 0;
void* operator new(std::size_t n) {
    ++alokacija;
    if (void* p = std::malloc(n ? n : 1)) return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

int saberi(int a, int b) { return a + b; }
int oduzmi(int a, int b) { return a - b; }

struct Puta {
    int k;
    int operator()(int a, int b) const { return (a + b) * k; }
};

struct Senzor {
    int id_;
    int id() const { return id_; }
    void postavi(int v) { id_ = v; }
    int saPomakom(int pomak) const { return id_ + pomak; }
};

// ---------------------------------------------------------------- 1
void sekcija1() {
    std::cout << "\n== 1. std::function: jedan tip za sve što se poziva\n";
    std::function<int(int, int)> op;               // prazan
    std::cout << "prazan: " << static_cast<bool>(op) << '\n';
    op = saberi;                                   // funkcija
    std::cout << "funkcija: " << op(2, 3);
    op = Puta{10};                                 // funkcijski objekat
    std::cout << ", objekat: " << op(2, 3);
    op = [](int a, int b) { return a * b; };       // lambda
    std::cout << ", lambda: " << op(2, 3) << '\n';

    // Zato može da bude vrednost u kontejneru: tabela operacija.
    std::map<std::string, std::function<int(int, int)>> operacije{
        {"+", saberi}, {"-", oduzmi}, {"*", [](int a, int b) { return a * b; }}};
    for (const auto& [ime, f] : operacije) std::cout << "7 " << ime << " 3 = " << f(7, 3) << '\n';

    // Poziv praznog baca std::bad_function_call (runtime/r01 kad ga niko ne uhvati).
    std::function<void()> nista;
    try {
        nista();
    } catch (const std::bad_function_call&) {
        std::cout << "prazan poziv: std::bad_function_call\n";
    }
}

// ---------------------------------------------------------------- 2
void sekcija2() {
    std::cout << "\n== 2. std::function: konverzije, metode, cena\n";
    // Argumenti i povratna vrednost se konvertuju kao pri običnom pozivu.
    std::function<double(int)> pola = [](int x) { return x / 2; };   // int / int!
    std::cout << "pola(7) = " << pola(7) << " (deljenje je celobrojno u lambdi)\n";

    // Metoda: objekat postaje PRVI argument.
    Senzor s{7};
    std::function<int(const Senzor&)> dajId = &Senzor::id;
    std::function<void(Senzor&, int)> postavi = &Senzor::postavi;
    postavi(s, 9);
    std::cout << "dajId(s) = " << dajId(s) << ", std::invoke = " << std::invoke(&Senzor::id, s)
              << '\n';

    // Rekurzivna lambda preko std::function (lambda ne može da imenuje sebe).
    std::function<int(int)> fakt = [&fakt](int n) { return n <= 1 ? 1 : n * fakt(n - 1); };
    std::cout << "fakt(5) = " << fakt(5) << '\n';

    // Cena: veći objekat, indirektan poziv, a za veliko stanje -- heap.
    std::array<int, 2> mali{1, 2};
    std::array<int, 64> veliki{};
    int pre = alokacija;
    std::function<int()> f1 = [mali] { return mali[0]; };
    int posleMalog = alokacija;
    std::function<int()> f2 = [veliki] { return veliki[0]; };
    int posleVelikog = alokacija;
    std::cout << "alokacije: lambda sa 8 B stanja " << posleMalog - pre << ", sa 256 B "
              << posleVelikog - posleMalog << "; f1() + f2() = " << f1() + f2() << '\n';
}

// ---------------------------------------------------------------- 3
void sekcija3() {
    std::cout << "\n== 3. std::bind: fiksiranje i preuređivanje argumenata\n";
    using namespace std::placeholders;   // _1, _2, ...
    auto dodaj10 = std::bind(saberi, _1, 10);      // drugi argument fiksiran
    auto obrnuto = std::bind(oduzmi, _2, _1);      // zamenjen redosled
    auto uvek = std::bind(saberi, 2, 3);           // svi fiksirani: poziva se bez argumenata
    std::cout << "dodaj10(5) = " << dodaj10(5) << ", obrnuto(10, 3) = " << obrnuto(10, 3)
              << ", uvek() = " << uvek() << '\n';
    // Isto lambdom (EMC Item 34 -- čitljivije, sekcija 5):
    auto dodaj10L = [](int x) { return saberi(x, 10); };
    std::cout << "lambda: dodaj10L(5) = " << dodaj10L(5) << '\n';
}

// ---------------------------------------------------------------- 4
void dodaj(int& brojac, int koliko) { brojac += koliko; }

void sekcija4() {
    std::cout << "\n== 4. std::bind: metode, reference, mem_fn\n";
    using namespace std::placeholders;
    Senzor s{1};
    // Metoda: drugi argument bind-a je objekat (pokazivač, referenca ili kopija).
    auto postaviS = std::bind(&Senzor::postavi, &s, _1);   // pokazivač: menja s
    postaviS(5);
    auto saPomakom = std::bind(&Senzor::saPomakom, s, _1); // KOPIJA s-a u trenutku bind-a
    s.postavi(100);
    std::cout << "s.id() = " << s.id() << ", saPomakom(1) na kopiji = " << saPomakom(1) << '\n';

    // bind KOPIRA argumente -- i one koji idu u referencu (zadatak z2).
    int brojac = 0;
    auto uKopiju = std::bind(dodaj, brojac, 1);
    auto uOriginal = std::bind(dodaj, std::ref(brojac), 1);
    uKopiju();
    uOriginal();
    uOriginal();
    std::cout << "brojac posle 1x kopija, 2x std::ref: " << brojac << '\n';

    // std::mem_fn: metoda kao funkcijski objekat, objekat je argument.
    std::vector<Senzor> senzori{{3}, {1}, {2}};
    auto id = std::mem_fn(&Senzor::id);
    int zbir = 0;
    for (const Senzor& x : senzori) zbir += id(x);
    std::cout << "zbir id-jeva preko mem_fn: " << zbir << '\n';
}

// ---------------------------------------------------------------- 5
int sat = 8;
int trenutnoVreme() { return sat; }
int postaviAlarm(int kada) { return kada; }

void sekcija5() {
    std::cout << "\n== 5. std::bind: zamke, i zašto lambda\n";
    using namespace std::placeholders;
    auto dodaj10 = std::bind(saberi, _1, 10);
    // Višak argumenata se tiho IGNORIŠE (kod lambde bi bio greška).
    std::cout << "dodaj10(5, 99, 100) = " << dodaj10(5, 99, 100) << '\n';

    // Argumenti bind-a se računaju ODMAH, a lambda ih računa pri pozivu
    // (zadatak z3).
    auto alarmBind = std::bind(postaviAlarm, trenutnoVreme() + 1);
    auto alarmLambda = [] { return postaviAlarm(trenutnoVreme() + 1); };
    sat = 12;
    std::cout << "sada " << sat << "h -- bind: " << alarmBind() << "h, lambda: " << alarmLambda()
              << "h\n";
    // Preopterećena funkcija se ne može direktno dati bind-u (errors/e01);
    // lambda nema taj problem.
}

int main() {
    std::cout << std::boolalpha;
    sekcija1();
    sekcija2();
    sekcija3();
    sekcija4();
    sekcija5();
}
