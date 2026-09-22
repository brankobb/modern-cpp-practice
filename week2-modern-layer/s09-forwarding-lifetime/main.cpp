#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// Vežba A: napiši template funkciju
//   template <typename T> void wrapper(T&& arg) { inner(std::forward<T>(arg)); }
// sa dva overload-a za inner (const std::string&, std::string&&) koji
// loguju koji je pozvan. Pozovi wrapper i sa lvalue i sa rvalue string-om.
//
// Vežba B: namerno napravi dangling string_view (uzmi ga iz privremenog
// std::string koji se odmah uništi) i pokreni pod ASan-om da vidiš
// use-after-free/stack-use-after-scope.
//
// Vežba C: invalidacija iteratora -- napravi vector, uzmi iterator, pa
// push_back dovoljno puta da izazove realokaciju, pa koristi stari iterator.

void inner(const std::string&) { std::cout << "inner(const&)\n"; }
void inner(std::string&&) { std::cout << "inner(&&)\n"; }

// Ako napišeš wrapper(T& arg) (obična referenca, NIJE DOBRO ovde) jer bi
// tad mogao da veže SAMO lvalue -- wrapper(std::string("temp")) uopšte ne
// bi kompajlirao (rvalue ne može da se veže za običnu non-const referencu).
// Treba da koristiš T&& u TEMPLATE kontekstu (forwarding/universal
// reference) -- tad T&& vezuje i lvalue i rvalue, a T se dedukuje da
// zapamti koje je bilo.
// Možeš i napraviti DVA overload-a wrapper-a (jedan sa const T&, jedan sa
// T&&) umesto template-a -- radi isto za JEDAN parametar, ali eksplodira
// kombinatorno (2^n overload-a) čim imaš više parametara -- zato se
// forwarding reference + std::forward koristi u praksi.
template <typename T>
void wrapper(T&& arg) {
    // Ako pozoveš inner(arg); BEZ std::forward (NIJE DOBRO) jer je arg,
    // kao IMENOVANA promenljiva unutar wrapper-a, UVEK lvalue -- čak i
    // ako je originalno bio pozvan sa rvalue-om spolja, ovde bi se uvek
    // pozvao inner(const&), NIKAD inner(&&). Izgubio bi se "move"
    // potencijal originalnog rvalue argumenta.
    // Treba da koristiš std::forward<T>(arg) da prosledis arg DALJE
    // zadržavajući NJEGOVU originalnu kategoriju (lvalue ostaje lvalue,
    // rvalue ostaje rvalue).
    inner(std::forward<T>(arg));
}

std::string_view danglingView() {
    std::string temp = "privremeni string koji ce nestati";
    // Ako vratiš string_view NA privremeni std::string (NIJE DOBRO) jer
    // string_view NE POSEDUJE podatke -- samo pokazuje na memoriju koju
    // temp koristi. Čim temp izađe iz scope-a (kraj funkcije), ta
    // memorija se oslobađa, a view i dalje "pokazuje" na nju.
    // Treba da vratiš std::string PO VREDNOSTI (ne view) kad izvor nije
    // dovoljno dugovečan -- kopija je sigurna, view nije.
    // Možeš i koristiti string_view kad je izvorni string GARANTOVANO
    // dugovečniji od view-a (npr. string_view na string_literal, ili na
    // string koji poseduje pozivalac i koji nadživljava poziv funkcije).
    return std::string_view(temp); // dangling čim funkcija vrati
}

void iteratorInvalidationTrap() {
    std::vector<int> v = {1, 2, 3};
    auto it = v.begin(); // pokazuje na prvi element
    std::cout << "*it pre push_back = " << *it << "\n";

    // Ako držiš iterator/pokazivač/referencu na element vector-a I
    // paralelno radiš push_back koji izazove realokaciju (NIJE DOBRO) jer
    // vector ALOCIRA NOVI, veći blok memorije i PREMESTI sve elemente
    // tamo kad kapacitet nije dovoljan -- stari blok se oslobađa, pa
    // svaki iterator/pokazivač/referenca ka STAROM bloku postaje dangling.
    for (int i = 0; i < 100; ++i) v.push_back(i); // skoro sigurno izaziva realokaciju

    // Treba da PONOVO uzmeš iterator POSLE svake operacije koja može da
    // invalidira (push_back/insert/erase iznad kapaciteta), ili koristiš
    // INDEKSE (v[i]) umesto iteratora kad kombinuješ čitanje i izmenu
    // veličine.
    // Možeš i pozvati v.reserve(dovoljno) UNAPRED da izbegneš realokaciju
    // tokom petlje -- iterator ostaje validan jer se memorija ne premešta
    // dok se ostaje unutar rezervisanog kapaciteta.
    std::cout << "*it posle push_back (UB, ASan treba da uhvati): " << *it << "\n";
}

int main() {
    // unitbuf -- auto-flush posle svake cout operacije, da ispis ne
    // ostane zaglavljen u baferu ako program pukne pre nego što se
    // isprazni (bitno kad je stdout preusmeren u fajl, ne terminal).
    std::cout.setf(std::ios::unitbuf);

    std::cout << "-- forwarding reference + std::forward --\n";
    std::string s = "hi";
    wrapper(s);            // treba: inner(const&)
    wrapper(std::string("temp")); // treba: inner(&&)

    std::cout << "-- dangling string_view --\n";
    std::string_view view = danglingView();
    std::cout << "view sadrzaj (UB, ASan treba da uhvati): " << view << "\n";

    std::cout << "-- invalidacija iteratora --\n";
    iteratorInvalidationTrap();
}
