#include <algorithm>
#include <iostream>
#include <stdexcept>

// Vežba: implementiraj copy-and-swap za klasu koja poseduje resurs.
//   1) operator= prima parametar PO VREDNOSTI (poziva copy ctor)
//   2) swap(*this, other)
//   3) other (lokalna kopija) se uništi na kraju funkcije, noseći stari resurs
//
// Zatim demonstriraj strong exception safety: napravi scenario gde bi
// "naivan" operator= ostavio objekat u polu-validnom stanju da je throw-ovao
// usred kopiranja, a copy-and-swap verzija ostane nepromenjena.

class Resource {
public:
    explicit Resource(int size) : data_(new int[size]), size_(size) {}
    Resource(const Resource& other) : data_(new int[other.size_]), size_(other.size_) {
        std::copy(other.data_, other.data_ + size_, data_);
    }
    ~Resource() { delete[] data_; }

    friend void swap(Resource& a, Resource& b) noexcept {
        std::swap(a.data_, b.data_);
        std::swap(a.size_, b.size_);
    }

    // Ako napišeš operator= koji NAJPRE delete[] data_ pa onda new
    // int[other.size_] i std::copy (NIJE DOBRO -- "naivna" verzija) jer
    // ako new ili copy baci izuzetak POSLE delete-a, *this ostaje sa
    // OBRISANIM podacima -- objekat je u polu-validnom, oštećenom stanju
    // (narušena je čak i basic exception guarantee).
    // Treba da koristiš copy-and-swap (kao ovde): parametar PO VREDNOSTI
    // pravi kopiju PRE nego što diramo *this, pa swap (koji ne baca)
    // zamenjuje sadržaj -- ako kopiranje baci, *this ostaje NETAKNUT
    // (strong exception guarantee, besplatno).
    // Možeš i napisati operator= koji PRVO alocira NOVU memoriju, PA TEK
    // ONDA oslobodi staru (bez swap-a) -- daje istu strong garanciju, ali
    // copy-and-swap je kraći i radi i za move (kad se parametar
    // konstruiše move-om umesto copy-em).
    Resource& operator=(Resource other) { // po vrednosti -> copy-and-swap
        swap(*this, other);
        return *this;
    }

private:
    int* data_;
    int size_;
};

int main() {
    std::cout << "-- copy-and-swap operator= --\n";
    Resource a(10);
    Resource b(20);
    a = b; // testiraj copy-and-swap
}
