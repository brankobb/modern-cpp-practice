#include <iostream>

void constPointerVariants() {
    std::cout << "-- constPointerVariants --\n";
    int a = 1, b = 2;

    const int* p1 = &a; // pokazivač na const int
    // Ako pokušaš da promeniš *p1 (NIJE DOBRO, ne kompajlira) jer je p1
    // "pokazivač na const int" -- vrednost na koju pokazuje se ne sme
    // menjati KROZ p1 (sama promenljiva a i dalje MOŽE biti promenjena na
    // drugi način, npr. direktno a = 5;).
    // *p1 = 5;          // TODO: otkomentariši -- compile error
    // Treba da koristiš const int* kad ti FUNKCIJA samo ČITA podatke
    // preko pokazivača (npr. parametar koji ne treba da menja ulaz).
    p1 = &b;             // OK -- pokazivač sam nije const

    int* const p2 = &a;  // const pokazivač na int
    // Možeš i int* const kad ti treba da UVEK pokazuje na isti objekat
    // (adresa se ne menja), ali smeš da menjaš vrednost preko njega.
    *p2 = 5;              // OK -- vrednost nije const
    // p2 = &b;           // TODO: otkomentariši -- compile error

    const int* const p3 = &a; // oboje const -- ni adresa ni vrednost se ne menjaju
    std::cout << "*p1=" << *p1 << " *p2=" << *p2 << " *p3=" << *p3 << "\n";
    (void)p1; (void)p2; (void)p3;
}

class Cache {
public:
    // Ako pokušaš da promeniš cached_/value_ u const funkciji BEZ mutable
    // (NIJE DOBRO, ne kompajlira) jer const member funkcija obećava da
    // NEĆE menjati stanje objekta.
    // Treba da označiš članove koji predstavljaju "interni keš, ne
    // logičko stanje objekta" kao mutable -- to je legitiman izuzetak od
    // const pravila, ne rupa u sistemu.
    // Možeš i izbeći mutable tako što keš držiš POTPUNO odvojeno (npr.
    // spoljni std::unordered_map<const Cache*, int>), ali to je obično
    // komplikovanije bez stvarne koristi.
    int getExpensiveValue() const {
        if (!cached_) {
            cached_ = true;
            value_ = 42; // dozvoljeno jer je mutable, iako je funkcija const
        }
        return value_;
    }

private:
    mutable bool cached_ = false;
    mutable int value_ = 0;
};

int main() {
    constPointerVariants();

    std::cout << "-- Cache (mutable) --\n";
    Cache c;
    std::cout << c.getExpensiveValue() << "\n"; // radi iako je getter const
}
