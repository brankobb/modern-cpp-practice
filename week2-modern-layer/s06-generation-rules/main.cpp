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
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&&) noexcept = default;
    MoveOnly& operator=(MoveOnly&&) noexcept = default;
};

class MaybeThrowingMove {
public:
    MaybeThrowingMove() { std::cout << "ctor\n"; }
    MaybeThrowingMove(const MaybeThrowingMove&) { std::cout << "copy ctor\n"; }
    MaybeThrowingMove(MaybeThrowingMove&&) /* namerno bez noexcept */ {
        std::cout << "move ctor\n";
    }
};

int main() {
    MoveOnly m1;
    MoveOnly m2 = std::move(m1);
    (void)m2;

    std::vector<MaybeThrowingMove> v;
    v.reserve(1);
    for (int i = 0; i < 4; ++i) {
        v.emplace_back();
    }
}
