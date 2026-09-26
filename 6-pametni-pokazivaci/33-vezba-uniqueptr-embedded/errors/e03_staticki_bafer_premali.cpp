// STD: c++17
// EXPECT-GCC: FixedStorage: objekat ne staje u bafer
// EXPECT-CLANG: FixedStorage: objekat ne staje u bafer
// POGREŠNO: bafer fiksne veličine (npr. određen konfiguracijom firmvera) je
//   premali za tip koji se u njega stavlja.
// Zašto: placement new ne proverava veličinu: napravio bi objekat preko
//   kraja bafera (ASan: global-buffer-overflow). static_assert pretvara tu
//   grešku u grešku pri KOMPAJLIRANJU, sa porukom koju sam napišeš. U
//   embedded kodu je to najbolje mesto za takve provere: ništa ne košta pri
//   izvršavanju.
// Ispravno: veći bafer (FixedStorage<Packet, sizeof(Packet)>), ili manji tip.
#include <cstddef>
#include <new>

template <typename T, std::size_t Capacity>
class FixedStorage {
    static_assert(sizeof(T) <= Capacity, "FixedStorage: objekat ne staje u bafer");
    static_assert(alignof(T) <= alignof(std::max_align_t), "FixedStorage: poravnanje nije podržano");

public:
    T* create() { return new (buffer_) T(); }

private:
    alignas(std::max_align_t) unsigned char buffer_[Capacity];
};

struct Packet {
    unsigned char payload[64];
};

int main() {
    FixedStorage<Packet, 32> slot;
    slot.create()->~Packet();
}
