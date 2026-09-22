#include <iostream>
#include <vector>

// Vežba A: klasa move-only (kao unique_ptr) -- copy ctor/assignment = delete,
// move ctor/assignment = default (ili ručno, noexcept).
//
// Vežba B: klasa sa move ctor-om koji NIJE noexcept -- stavi je u
// std::vector, pushuj dovoljno elemenata da izazoveš realokaciju, i
// posmatraj (std::cout u copy i move ctor-u) da li vector koristi copy
// umesto move zbog exception-safety garancije. Zatim dodaj noexcept i
// uporedi.

class MoveOnly {
public:
    MoveOnly() = default;
    // Ako OSTAVIŠ copy ctor/assignment kompajlerski generisane na klasi
    // koja predstavlja "vlasništvo" (kao unique_ptr) (NIJE DOBRO) jer bi
    // dva objekta onda "delila" isti resurs bez tvog znanja -- ista
    // shallow-copy zamka kao u week1 s02, samo skrivenija jer ovde nemamo
    // čak ni sirov pokazivač da nas podseti.
    // Treba da EKSPLICITNO obrišeš copy (= delete) kad klasa treba da
    // bude move-only -- kompajler ti onda pomaže tako što odbija svaki
    // pokušaj kopiranja na kompajliranju, ne u runtime-u.
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&&) noexcept = default;
    MoveOnly& operator=(MoveOnly&&) noexcept = default;
};

class MaybeThrowingMove {
public:
    MaybeThrowingMove() { std::cout << "ctor\n"; }
    MaybeThrowingMove(const MaybeThrowingMove&) { std::cout << "copy ctor\n"; }
    // Ako move ctor NIJE noexcept (NIJE DOBRO ako ti je bitna performansa
    // u vector-u) jer std::vector pri realokaciji BIRA COPY umesto MOVE
    // kad move ctor može da baci -- vector mora da garantuje da ako
    // realokacija ne uspe (izuzetak), STARI elementi ostanu netaknuti
    // (strong exception guarantee), a to može da garantuje samo ako
    // kopira (original preživi) a ne premešta (original bi ostao
    // pola-ispražnjen da move baci na pola posla).
    // Treba da označiš move ctor kao noexcept KAD GOD MOŽEŠ da garantuješ
    // da neće baciti -- tad vector sigurno koristi move (brže).
    // Možeš i ostaviti bez noexcept ako move STVARNO može da baci (retko,
    // ali postoji) -- tad prihvataš sporiji copy fallback kao cenu
    // ispravnosti.
    MaybeThrowingMove(MaybeThrowingMove&&) /* namerno bez noexcept */ {
        std::cout << "move ctor\n";
    }
};

int main() {
    std::cout << "-- MoveOnly (copy = delete) --\n";
    MoveOnly m1;
    MoveOnly m2 = std::move(m1);
    (void)m2;

    std::cout << "-- vector realokacija, move ctor BEZ noexcept --\n";
    std::vector<MaybeThrowingMove> v;
    v.reserve(1);
    for (int i = 0; i < 4; ++i) {
        v.emplace_back();
    }
}
