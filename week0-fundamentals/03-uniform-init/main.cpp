#include <atomic>
#include <initializer_list>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

// Effective Modern C++, Item 7: "Distinguish between () and {} when
// creating objects." Svaka funkcija ispod demonstrira jednu tačku iz tog
// item-a, redosledom kojim se pojavljuju u knjizi.

void threeSyntaxes() {
    std::cout << "-- tri sintakse inicijalizacije (ekvivalentne za int) --\n";
    int x(0);    // zagrade
    int y = 0;   // '='
    int z{0};    // vitičaste zagrade
    int w = {0}; // '=' + vitičaste
    std::cout << "x=" << x << " y=" << y << " z=" << z << " w=" << w << "\n";
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
    threeSyntaxes();

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
    vectorClassic();
    genericCodeProblem();
}
