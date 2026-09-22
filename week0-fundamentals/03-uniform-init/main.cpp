#include <atomic>
#include <initializer_list>
#include <iostream>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

// Sekcije 1-5: formalna podela inicijalizacije. Sekcije posle toga prate
// Effective Modern C++ Item 7 ("Distinguish between () and {} when
// creating objects") i par dodatnih tema (auto+{}, new{}, aggregate,
// designated initializers). Svaka tvrdnja ovde je testirana
// kompajliranjem/pokretanjem pre nego što je ušla u komentare.

struct DefaultInitDemo {
    int x; // BEZ default member initializer-a -- ostaje nedefinisan
};

void section1_defaultInit() {
    std::cout << "-- 1. default-initialization --\n";
    // Ako pročitaš x pre nego što mu nešto dodeliš (NIJE DOBRO) jer je
    // vrednost NEDEFINISANA (garbage) -- ovo je UB, ne "verovatno nula".
    DefaultInitDemo d;
    d.x = 42; // moraš eksplicitno dodeliti pre čitanja
    std::cout << "d.x (posle eksplicitne dodele) = " << d.x << "\n";

    std::string s; // za klase, default-init ZOVE default ctor -- string je prazan, NE garbage
    std::cout << "std::string s; -> \"" << s << "\" (prazan, ne garbage -- string ima default ctor)\n";
}

void section2_valueInit() {
    std::cout << "-- 2. value-initialization --\n";
    // Treba da koristiš {} (bez argumenata) kad želiš GARANTOVANU
    // nula/prazno stanje za primitivan tip -- za razliku od
    // default-init iznad, ovo NIJE nedefinisano.
    int x{};
    double d{};
    bool b{};
    std::cout << "int x{}=" << x << " double d{}=" << d << " bool b{}=" << b << "\n";
}

void section3_directInit() {
    std::cout << "-- 3. direct-initialization --\n";
    int x(42);
    std::string s("hello");
    std::vector<int> v(10); // 10 elemenata, vrednost 0
    std::cout << "x=" << x << " s=" << s << " v.size()=" << v.size() << "\n";
}

class ExplicitOnly {
public:
    explicit ExplicitOnly(int v) : v_(v) { std::cout << "ExplicitOnly(int)\n"; }
    int v_;
};

void section4_copyInit() {
    std::cout << "-- 4. copy-initialization --\n";
    int x = 42;
    std::string s = "hello";
    std::cout << "x=" << x << " s=" << s << "\n";

    // Ako pozoveš explicit ctor preko copy-init sintakse (=) (NIJE DOBRO,
    // ne kompajlira) jer explicit BAŠ ZATO postoji -- da isključi ctor iz
    // razmatranja kod copy-init, sprečava "tihu" implicitnu konverziju.
    // ExplicitOnly bad = 5; // TODO: otkomentariši -- compile error (copy-init, explicit isključen)
    // Treba da koristiš direct-init kad je ctor explicit.
    ExplicitOnly good(5); // direct-init -- radi, explicit ctor SE razmatra
    std::cout << "good.v_=" << good.v_ << "\n";
}

void section5_uniformInit() {
    std::cout << "-- 5. uniform (brace) initialization --\n";
    int x{42};
    std::string s{"hello"};
    std::cout << "x=" << x << " s=" << s << "\n";
}

class MemberDefaults {
public:
    void print() const { std::cout << "a=" << a_ << " b=" << b_ << "\n"; }

private:
    // Ako pokušaš int c_(3); ovde (NIJE DOBRO, ne kompajlira) jer se to
    // parsira kao DEKLARACIJA FUNKCIJE c_ koja vraća int, ne kao default
    // vrednost člana -- () sintaksa nije dozvoljena za default vrednosti
    // non-static članova.
    // Treba da koristiš {} ili = za default vrednost člana -- oba rade
    // identično za ovaj slučaj.
    int a_{1};  // {} -- radi
    int b_ = 2; // = -- radi
    // int c_(3); // TODO: otkomentariši -- compile error, izgleda kao funkcija
};

void nonCopyableTrap() {
    std::cout << "-- non-copyable objekat (std::atomic) --\n";
    std::atomic<int> ai1{0}; // {} -- radi
    std::atomic<int> ai2(0); // () -- radi
    // Ako pokušaš std::atomic<int> ai3 = 0; (NIJE DOBRO, ne kompajlira)
    // jer copy-initialization (=) implicitno pretpostavlja da je tip
    // kopirljiv, a std::atomic je NAMERNO nekopirljiv (thread-safety
    // garancije bi bile narušene kopiranjem).
    // Treba da koristiš () ili {} za tipove koji nisu kopirljivi.
    // std::atomic<int> ai3 = 0; // TODO: otkomentariši -- compile error
    std::cout << "ai1=" << ai1.load() << " ai2=" << ai2.load() << "\n";
}

void narrowingCheck() {
    std::cout << "-- narrowing: samo {} odbija --\n";
    double d = 3.14;
    // Ako pokušaš int n1{d}; (NIJE DOBRO, ne kompajlira) jer bi se
    // izgubila preciznost (double -> int) -- {} EKSPLICITNO zabranjuje
    // narrowing conversions, bez obzira da li je vrednost u datom
    // trenutku "slučajno" tačna.
    // int n1{d}; // TODO: otkomentariši -- compile error
    // Treba da koristiš () ili = kad namerno želiš konverziju sa gubitkom
    // preciznosti -- ili bolje, static_cast<int>(d) da bude eksplicitno.
    int n2(d);  // () -- prolazi (uz warning)
    int n3 = d; // = -- prolazi (uz warning)
    std::cout << "n2=" << n2 << " n3=" << n3 << "\n";
}

struct VexingParse {
    VexingParse() { std::cout << "VexingParse()\n"; }
};

void mostVexingParse() {
    std::cout << "-- most vexing parse: {} je imun --\n";
    // Ako napišeš VexingParse v1(); misleći da praviš objekat (NIJE
    // DOBRO) jer se ovo parsira kao DEKLARACIJA FUNKCIJE v1 koja ne prima
    // ništa i vraća VexingParse -- objekat se NIKAD ne konstruiše.
    VexingParse v1(); // TODO: ovo je funkcija, ne objekat! (probaj typeid(v1) -- ne kompajlira)
    // Treba da koristiš {} kad želiš default-konstruisan objekat sa
    // praznim argumentima -- {} nema tu dvosmislenost.
    VexingParse v2{}; // OVO jeste objekat
}

class Widget {
public:
    Widget(int, bool) { std::cout << "Widget(int, bool)\n"; }
    Widget(int, double) { std::cout << "Widget(int, double)\n"; }
    Widget(std::initializer_list<long double> il) {
        std::cout << "Widget(initializer_list<long double>), size=" << il.size() << "\n";
    }
    Widget(const Widget&) { std::cout << "Widget(copy ctor)\n"; }
    Widget(Widget&&) noexcept { std::cout << "Widget(move ctor)\n"; }
    operator float() const {
        std::cout << "  [Widget -> float konverzija]\n";
        return 0.0f;
    }
};

void initializerListHijack() {
    std::cout << "-- initializer_list 'otmica' overload resolution-a --\n";
    // Ako klasa ima BILO KOJI ctor koji prima initializer_list<T> (MOŽE
    // BITI IZNENAĐENJE) jer {} sintaksa UVEK preferira taj ctor ako je
    // konverzija moguća -- čak i kad postoji "bolji" match među ostalim
    // konstruktorima.
    Widget w1(10, true); // () -- normalan overload resolution
    Widget w2{10, true}; // {} -- OTETO! poziva initializer_list ctor (int,bool -> long double)
    Widget w3(10, 5.0);  // () -- normalan overload resolution
    Widget w4{10, 5.0};  // {} -- OTETO! isto

    std::cout << "-- otmica hvata čak i copy/move konstrukciju --\n";
    // Treba da budeš svestan da čak i "očigledno" copy/move konstruisanje
    // preko {} može biti oteto -- ako klasa ima operator konverzije (kao
    // Widget::operator float() ovde) koji otvara put ka initializer_list
    // tipu.
    Widget w5(w4);             // () -- copy ctor
    Widget w6{w4};             // {} -- OTETO! w4 -> float -> long double
    Widget w7(std::move(w4));  // () -- move ctor
    Widget w8{std::move(w4)};  // {} -- OTETO! isto
}

struct WidgetStrict {
    WidgetStrict(int, bool) { std::cout << "WidgetStrict(int, bool)\n"; }
    WidgetStrict(int, double) { std::cout << "WidgetStrict(int, double)\n"; }
    WidgetStrict(std::initializer_list<bool>) {
        std::cout << "WidgetStrict(initializer_list<bool>)\n";
    }
};

void initializerListNarrowingError() {
    std::cout << "-- otmica + narrowing = compile error --\n";
    // Ako initializer_list ctor postoji ali bi konverzija argumenata u
    // njegov element-tip zahtevala NARROWING (NIJE DOBRO, ne kompajlira)
    // jer kompajler I DALJE insistira da proba initializer_list ctor prvi
    // -- a narrowing je zabranjen u {} -- rezultat je COMPILE ERROR, čak i
    // kad postoji savršen match među OSTALIM konstruktorima
    // (WidgetStrict(int, double) bi savršeno odgovarao za {10, 5.0}, ali
    // se nikad ne razmatra jer initializer_list<bool> ctor "blokira put").
    // WidgetStrict w{10, 5.0}; // TODO: otkomentariši -- compile error (int/double -> bool je narrowing)
}

struct WidgetFallback {
    WidgetFallback(int, bool) { std::cout << "WidgetFallback(int, bool)\n"; }
    WidgetFallback(int, double) { std::cout << "WidgetFallback(int, double)\n"; }
    WidgetFallback(std::initializer_list<std::string>) {
        std::cout << "WidgetFallback(initializer_list<string>)\n";
    }
};

void initializerListFallback() {
    std::cout << "-- fallback: kad init-list ctor NIJE moguć, koristi se normalan overload --\n";
    // Treba da znaš da se kompajler VRAĆA na normalan overload resolution
    // SAMO kad NEMA NIKAKVOG načina da se argumenti konvertuju u tip
    // initializer_list elementa (ovde: int/bool/double se ne mogu
    // implicitno konvertovati u std::string, pa initializer_list<string>
    // ctor uopšte nije viable kandidat).
    WidgetFallback w1(10, true); // () -- normalan
    WidgetFallback w2{10, true}; // {} -- I OVDE normalan! nema puta ka initializer_list<string>
}

class WidgetEmpty {
public:
    WidgetEmpty() { std::cout << "WidgetEmpty() -- default ctor\n"; }
    WidgetEmpty(std::initializer_list<int> il) {
        std::cout << "WidgetEmpty(initializer_list<int>), size=" << il.size() << "\n";
    }
};

void emptyBracesMeaning() {
    std::cout << "-- prazne {} = 'bez argumenata', NE prazan initializer_list --\n";
    WidgetEmpty w1;   // default ctor
    WidgetEmpty w2{}; // I OVO je default ctor, ne initializer_list sa 0 elemenata!
    // Treba da eksplicitno staviš PRAZNE {} UNUTAR () poziva da bi pozvao
    // initializer_list ctor sa STVARNO praznom listom -- ovo je jedini
    // pouzdan način.
    WidgetEmpty w3({}); // initializer_list ctor, size=0 -- ISPRAVAN način za praznu listu

    // Ako pomisliš da je WidgetEmpty w4{{}}; TAKOĐE prazna lista (NIJE
    // TAČNO, proverio sam ovo uživo jer sam prvobitno pogrešno napisao da
    // JESTE) jer spoljašnje {} formira initializer_list, a UNUTRAŠNJE {}
    // je NJEGOV JEDINI element -- taj element se VALUE-INICIJALIZUJE u
    // int (postaje 0), pa dobijaš listu sa JEDNIM elementom (vrednosti
    // 0), NE praznu listu.
    // Treba da koristiš ISKLJUČIVO W({}) (paren oko prazne {}) kad ti
    // treba GARANTOVANO prazan initializer_list -- W{{}} je druga stvar.
    WidgetEmpty w4{{}}; // iznenađenje: initializer_list ctor, size=1 (NE 0!)
}

void autoAndBraces() {
    std::cout << "-- auto + {} (C++17 je promenio pravilo) --\n";
    // Pre C++17, auto x{5}; je dedukovao std::initializer_list<int> --
    // poznat izvor zabune. Treba da znaš da OD C++17 auto x{5}; dedukuje
    // OBIČAN int (jednoelementni direct-list-init), dok auto y = {5};
    // I DALJE dedukuje std::initializer_list<int> (copy-list-init sa
    // jednim elementom -- druga grana pravila).
    auto x{5};    // C++17+: x je int
    auto y = {5}; // uvek: y je std::initializer_list<int>
    std::cout << "typeid(x).name()=" << typeid(x).name()
              << " typeid(y).name()=" << typeid(y).name() << "\n";
    // Možeš i da izbegneš celu ovu zabunu tako što koristiš auto x = 5;
    // (copy-init, bez {}) kad ti treba prost tip -- nema dvosmislenosti.
}

void newWithBraces() {
    std::cout << "-- new sa {} --\n";
    auto p1 = new int(42); // radi
    auto p2 = new int{42}; // radi, isti rezultat za proste tipove
    std::cout << "*p1=" << *p1 << " *p2=" << *p2 << "\n";
    delete p1;
    delete p2;
    // Za tipove sa initializer_list ctor-om, new T{args} podleže ISTOJ
    // "otmici" kao i obično T{args} -- pravilo se ne menja zbog new-a.
}

struct AggPoint {
    int x;
    int y;
};

void aggregateInit() {
    std::cout << "-- aggregate initialization (struct i niz) --\n";
    // AggPoint NEMA user-deklarisan ctor, nema private članove -- to je
    // AGREGAT, pa {} direktno puni članove REDOSLEDOM DEKLARACIJE, bez
    // poziva bilo kog konstruktora.
    AggPoint p{1, 2};
    std::cout << "p.x=" << p.x << " p.y=" << p.y << "\n";

    int arr[]{1, 2, 3, 4}; // isto pravilo važi za obične nizove
    std::cout << "arr[0..3] = " << arr[0] << " " << arr[1] << " " << arr[2] << " " << arr[3] << "\n";
}

#if __cplusplus >= 202002L
struct DesignatedPoint {
    int x;
    int y;
};

void designatedInitializers() {
    std::cout << "-- designated initializers (C++20) --\n";
    // Treba da designatori BUDU U ISTOM REDOSLEDU kao deklaracija članova
    // -- DesignatedPoint p{.y = 20, .x = 10}; (obrnut redosled) je GREŠKA,
    // ne samo neuobičajeno.
    DesignatedPoint p{.x = 10, .y = 20};
    std::cout << "p.x=" << p.x << " p.y=" << p.y << "\n";
}
#else
void designatedInitializers() {
    std::cout << "-- designated initializers (C++20) -- PRESKOČENO, treba -std=c++20 --\n";
    std::cout << "   pokreni: ./build.sh main.cpp -std=c++20 (ili build.ps1 isto)\n";
}
#endif

class ConstAndRefMembers {
public:
    // Ako pokušaš da dodeliš const_/ref_ U TELU konstruktora (NIJE DOBRO,
    // ne kompajlira) jer const član i referenca MORAJU biti inicijalizovani
    // pre nego što telo ctor-a uopšte počne da se izvršava -- do tada su
    // "gotovi" (const se ne može menjati, referenca se ne može rebindovati).
    // Treba da koristiš INIT LISTU -- ovo nije stilska preporuka nego
    // JEDINI način da se ovakvi članovi uopšte inicijalizuju.
    ConstAndRefMembers(int v, int& ref) : const_(v), ref_(ref) {
        // const_ = v; // TODO: otkomentariši -- compile error, const_ je već inicijalizovan
        std::cout << "ConstAndRefMembers ctor, const_=" << const_ << " ref_=" << ref_ << "\n";
    }

private:
    const int const_;
    int& ref_;
};

void constAndRefMembers() {
    std::cout << "-- const i reference članovi: init lista je OBAVEZNA --\n";
    int x = 7;
    ConstAndRefMembers c(5, x);
    (void)c;
}

void vectorClassic() {
    std::cout << "-- std::vector(10, 20) vs std::vector{10, 20} --\n";
    std::vector<int> v1(10, 20); // () -- normalan ctor: 10 elemenata, svi = 20
    std::vector<int> v2{10, 20}; // {} -- initializer_list ctor: 2 elementa, [10, 20]
    std::cout << "v1.size()=" << v1.size() << " v2.size()=" << v2.size() << "\n";
}

// Zašto std::make_unique/std::make_shared INTERNO koriste () a ne {}:
// autor generičke funkcije ne može unapred znati da li pozivalac očekuje
// "() ponašanje" ili "{} ponašanje" za dati tip -- zato standardna
// biblioteka BIRA () i to DOKUMENTUJE kao deo interfejsa.
template <typename T, typename... Ts>
T makeWithParens(Ts&&... params) {
    return T(std::forward<Ts>(params)...); // () -- kao std::make_unique/make_shared
}

template <typename T, typename... Ts>
T makeWithBraces(Ts&&... params) {
    return T{std::forward<Ts>(params)...}; // {} -- drugačiji rezultat za neke tipove!
}

void genericCodeProblem() {
    std::cout << "-- generički kod: () vs {} daju RAZLIČIT rezultat za isti poziv --\n";
    // Treba da autor generičke funkcije SVESNO odabere i DOKUMENTUJE koju
    // sintaksu koristi -- pozivalac ne može da pogodi bez da pogleda
    // implementaciju (ili dokumentaciju).
    auto v1 = makeWithParens<std::vector<int>>(10, 20);
    auto v2 = makeWithBraces<std::vector<int>>(10, 20);
    std::cout << "makeWithParens<vector<int>>(10,20) -> size=" << v1.size()
              << ", makeWithBraces<vector<int>>(10,20) -> size=" << v2.size() << "\n";
}

int main() {
    section1_defaultInit();
    section2_valueInit();
    section3_directInit();
    section4_copyInit();
    section5_uniformInit();

    std::cout << "-- default vrednost člana klase (samo {} i =, ne ()) --\n";
    MemberDefaults md;
    md.print();

    nonCopyableTrap();
    narrowingCheck();
    mostVexingParse();
    initializerListHijack();
    initializerListNarrowingError();
    initializerListFallback();
    emptyBracesMeaning();
    autoAndBraces();
    newWithBraces();
    aggregateInit();
    designatedInitializers();
    constAndRefMembers();
    vectorClassic();
    genericCodeProblem();
}
