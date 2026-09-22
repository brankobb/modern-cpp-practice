#include <cstddef>
#include <iostream>

void section1_pointerBasics() {
    std::cout << "-- 1. pokazivač: osnove (&, *, void*) --\n";
    int x = 5;
    int* p = &x;   // & -- uzmi adresu
    int y = *p;    // * -- dereferenciraj
    void* vp = p;  // generički pokazivač -- mora se cast-ovati pre dereferenciranja
    std::cout << "x=" << x << " y (kopija preko *p)=" << y
              << " vp=" << vp << "\n";
}

void fInt(int) { std::cout << "f(int)\n"; }
void fPtr(char*) { std::cout << "f(char*)\n"; }

void section2_nullptrVsNullVsZero() {
    std::cout << "-- 2. nullptr vs NULL vs 0 --\n";
    fInt(0);       // 0 je int literal -> f(int)
    fPtr(nullptr); // nullptr ima sopstveni tip (std::nullptr_t) -> f(char*)
    // Ako imaš OBA overload-a f(int) i f(char*) u ISTOM scope-u i pozoveš
    // f(NULL) (NIJE DOBRO, ne kompajlira) jer NULL nije garantovano
    // pokazivačkog tipa (obično je makro za 0 ili 0L) -- kompajler ne zna
    // koji overload da izabere, AMBIGUOUS COMPILE ERROR (testirano uživo).
    // Treba da koristiš nullptr za pokazivače u NOVOM kodu -- uvek bira
    // ispravan overload, bez dvosmislenosti.
    std::cout << "sizeof(nullptr)=" << sizeof(nullptr) << "\n";
}

void section3_pointerArithmetic() {
    std::cout << "-- 3. pointer arithmetic --\n";
    int arr[5] = {1, 2, 3, 4, 5};
    int* p = arr;
    std::cout << "*p=" << *p << " *(p+1)=" << *(p + 1) << " *(p+4)=" << *(p + 4) << "\n";
    std::ptrdiff_t diff = (p + 4) - p;
    std::cout << "(p+4) - p = " << diff << " (razlika u ELEMENTIMA, ne bajtovima)\n";
    // Ako dereferenciraš arr + 5 (jedan iza kraja) (NIJE DOBRO) jer je to
    // UB -- p == arr+5 je validno poređenje (npr. za end() iterator stil),
    // ali *(arr+5) čita van niza.
    // Treba da koristiš poređenje sa "one-past-end" pokazivačem samo za
    // proveru granice (kao std::vector::end()), nikad za dereferenciranje.
}

void section4_pointerToPointerAndRefToPointer() {
    std::cout << "-- 4. pokazivač na pokazivač, referenca na pokazivač --\n";
    int x = 5;
    int* p = &x;
    int** pp = &p;   // pokazivač na pokazivač
    int*& rp = p;    // referenca na pokazivač
    std::cout << "**pp=" << **pp << " *rp=" << *rp << "\n";
    // Ako pokušaš int& arr[3]; (NIJE DOBRO, ne kompajlira) jer niz
    // referenci NE POSTOJI kao tip -- svaki slot niza mora imati
    // nezavisnu adresu/identitet za dodelu, a referenca nema sopstvenu
    // adresu odvojenu od objekta na koji referiše.
    // int& arr[3]; // TODO: otkomentariši -- compile error
}

void arrayDecayTrap(int arr[]) {
    // Ako se osloniš na sizeof(arr) unutar funkcije da saznaš dužinu niza
    // (NIJE DOBRO) jer je niz "decay-ovao" u pokazivač čim je prosleđen --
    // dobićeš veličinu POKAZIVAČA (8 bajtova na 64-bit), ne veličinu
    // originalnog niza.
    // Treba da prosledis dužinu EKSPLICITNO kao poseban parametar
    // (void f(int* arr, size_t n)), ili koristiš std::array/std::vector
    // koji nose svoju veličinu sa sobom.
    // Možeš i proslediti referencu na niz FIKSNE veličine
    // (void f(int (&arr)[5])) -- tad sizeof radi ispravno, ali funkcija
    // onda radi SAMO za nizove te tačne veličine.
    std::cout << "sizeof(arr) unutar funkcije = " << sizeof(arr) << "\n";
}

int* danglingPointer() {
    int local = 42;
    // Ako vratiš adresu lokalne promenljive (NIJE DOBRO) jer local izlazi
    // iz scope-a čim se funkcija završi -- pokazivač koji vraćaš pokazuje
    // na memoriju koja više "nije tvoja" (stack frame je uništen).
    // Treba da vratiš PO VREDNOSTI (return local;) ako želiš kopiju, ili
    // alociraš na heap-u (new int(42)) ako objekat mora da preživi poziv
    // funkcije -- ali onda si ti odgovoran za delete.
    // Možeš i koristiti static lokalnu promenljivu (static int local =
    // 42;) ako ti treba da "preživi" pozive, ali onda je DELJENA između
    // svih poziva funkcije -- retko je to ono što stvarno želiš.
    return &local; // BUG: vraća adresu lokalne promenljive koja izlazi iz scope-a
}

struct Big {
    char buf[100];
};

void printRefSize(Big& r) {
    // sizeof(r) OVDE daje veličinu Big (100), ne veličinu pokazivača --
    // referenca se "ponaša" kao sam objekat za sizeof/typeid/&, iako je
    // ispod haube često implementirana kao pokazivač.
    std::cout << "sizeof(Big&) unutar funkcije = " << sizeof(r) << " (veličina REFERISANOG tipa, ne pokazivača)\n";
}

void referenceBasics() {
    std::cout << "-- 5. referenceBasics --\n";
    int x = 10;
    int& ref = x; // MORA se inicijalizovati ovde
    ref = 20;      // menja x, jer je ref alias za x
    std::cout << "x = " << x << " (posle ref = 20)\n";
    std::cout << "&x == &ref? " << (&x == &ref ? "DA" : "NE") << " (referenca deli adresu sa objektom)\n";

    int y = 99;
    // Ako pokušaš ref = y; misleći da REBINDUJEŠ ref na y (NIJE DOBRO, to
    // ne radi to) jer se reference u C++-u NE MOGU rebindovati posle
    // inicijalizacije -- ovo je ASSIGNMENT, kopira vrednost y (99) u x
    // (na koji ref i dalje pokazuje).
    // Treba da koristiš POKAZIVAČ ako ti treba promenljiva koja može da
    // "promeni metu" tokom vremena (int* p = &x; p = &y;).
    // Možeš i napraviti NOVU referencu na y ako ti to zapravo treba (int&
    // ref2 = y;) -- ref i ref2 su dve odvojene, nezavisne reference.
    // ref = y;    // OVO NE "rebinduje" ref na y -- ovo je ASSIGNMENT, kopira 99 u x!
    // ref sada i dalje referiše x, samo je x sad 99

    Big big;
    printRefSize(big);
}

int& danglingRef() {
    int local = 42;
    // Ista greška kao danglingPointer(), samo preko reference -- rezultat
    // je isti (UB), samo je sintaksa "tiša" jer nema eksplicitnog *.
    return local; // BUG: dangling referenca
}

struct CopyCounter {
    CopyCounter() = default;
    CopyCounter(const CopyCounter&) { ++copies; }
    static inline int copies = 0;
};

void byValue(CopyCounter) {}       // kopira na svaki poziv
void byConstRef(const CopyCounter&) {} // ne kopira

void section6_passByValueVsRef() {
    std::cout << "-- 6. prosleđivanje: vrednost vs const referenca --\n";
    CopyCounter c;
    CopyCounter::copies = 0;
    byValue(c);
    std::cout << "posle byValue(c): copies=" << CopyCounter::copies << " (kopirano)\n";
    CopyCounter::copies = 0;
    byConstRef(c);
    std::cout << "posle byConstRef(c): copies=" << CopyCounter::copies << " (NIJE kopirano)\n";
    // Ako prosleđuješ veliki objekat PO VREDNOSTI kad ga funkcija samo
    // ČITA (NIJE DOBRO za performanse) jer se ceo objekat kopira na SVAKI
    // poziv, čak i ako se ništa ne menja.
    // Treba da koristiš const T& kao DEFAULT za veće objekte koje ne
    // menjaš -- izbegava kopiranje, i dalje garantuje da funkcija ne
    // menja original.
    // Možeš i proslediti po vrednosti kad je tip JEFTIN za kopiranje
    // (int, iterator) -- kopija je tad jeftinija ili ista cena kao
    // indirection kroz referencu.
}

class Container {
public:
    int& at(std::size_t i) { return data_[i]; } // OK -- data_ živi duže od poziva at()

private:
    int data_[3] = {1, 2, 3};
};

void section7_returningReferences() {
    std::cout << "-- 7. vraćanje referenci: OK vs NIKAD --\n";
    Container c;
    c.at(0) = 99; // OK -- referenca na ČLAN objekta, ne na lokalnu promenljivu
    std::cout << "c.at(0)=" << c.at(0) << " (izmenjeno kroz vraćenu referencu)\n";

    std::cout << "-- danglingPointer() (namerni bag, ASan treba da uhvati) --\n";
    int* dp = danglingPointer();
    std::cout << *dp << "\n"; // UB

    std::cout << "-- danglingRef() (namerni bag, ista greška preko reference) --\n";
    int& dr = danglingRef();
    std::cout << dr << "\n"; // UB
}

int main() {
    // unitbuf -- auto-flush posle svake cout operacije. Bez ovoga, kad
    // program pukne (ASan/UBSan abort), sav ispis do tog trenutka može
    // biti IZGUBLJEN ako stdout nije vezan za terminal (npr. kad
    // preusmeriš u fajl) -- baferovanje tad postaje "full buffered"
    // umesto "line buffered", i abort() ne flush-uje bafer. Testirano
    // uživo: bez ovoga, ceo ispis pre crasha nestane pri `./run > out.txt`.
    std::cout.setf(std::ios::unitbuf);

    section1_pointerBasics();
    section2_nullptrVsNullVsZero();
    section3_pointerArithmetic();
    section4_pointerToPointerAndRefToPointer();

    std::cout << "-- arrayDecayTrap --\n";
    int arr[5] = {1, 2, 3, 4, 5};
    std::cout << "sizeof(arr) u main = " << sizeof(arr) << " (ceo niz)\n";
    arrayDecayTrap(arr);

    referenceBasics();
    section6_passByValueVsRef();
    section7_returningReferences();
}
