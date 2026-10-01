// Korak 2 -- BONUS 2: zašto je virtual destruktor obavezan.
//   ./build.sh roadmap/step-2-object-lifecycle/solutions/bonus2_virtual_dtor.cpp
//   ./build.sh roadmap/step-2-object-lifecycle/solutions/bonus2_virtual_dtor.cpp -DNO_VIRTUAL
//       -- ~Buffer nije virtual: "delete base" je UB. U praksi se ~Derived
//          ne pozove (njegov resurs procuri), a ASan prijavi
//          new-delete-type-mismatch (oslobađa se sizeof(Buffer) bajtova
//          od objekta koji ima sizeof(Derived)). Kompajler upozori i pre
//          toga (deo -Wall): g++ -Wdelete-non-virtual-dtor, clang
//          -Wdelete-non-abstract-non-virtual-dtor.
//
// Buffer je ovde skraćen (copy zabranjen, bez move-a), da se vidi samo
// destruktor. Polimorfna klasa i javno kopiranje se ionako ne slažu --
// kopija preko Buffer& bi "odsekla" Derived deo (slicing), zato Core
// Guidelines C.67 kaže: polimorfna baza zabranjuje javnu kopiju.

#include <cstddef>
#include <cstdint>
#include <cstdio>

class Buffer {
public:
    explicit Buffer(std::size_t size) : size_{size}, data_{new std::uint8_t[size]{}} {
        std::printf("  Buffer ctor (%zu bytes)\n", size_);
    }
#ifdef NO_VIRTUAL
    ~Buffer() {
#else
    virtual ~Buffer() {
#endif
        std::printf("  Buffer dtor\n");
        delete[] data_;
    }
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    virtual const char* kind() const { return "plain buffer"; }
    std::size_t size() const noexcept { return size_; }

private:
    std::size_t size_;
    std::uint8_t* data_;
};

// Izvedena klasa ima SVOJ resurs: tabelu za CRC.
class CrcBuffer : public Buffer {
public:
    explicit CrcBuffer(std::size_t size) : Buffer{size}, table_{new std::uint32_t[256]{}} {
        std::printf("  CrcBuffer ctor (+256 entry table)\n");
    }
#ifdef NO_VIRTUAL
    ~CrcBuffer() {                           // sa "override" ovo se ne bi ni kompajliralo -- i to je poenta
#else
    ~CrcBuffer() override {                  // "override" na destruktoru: greška kompajlera ako baza nije virtual
#endif
        std::printf("  CrcBuffer dtor\n");
        delete[] table_;
    }
    const char* kind() const override { return "buffer with CRC table"; }

private:
    std::uint32_t* table_;
};

int main() {
    std::printf("== construction: base first, then derived\n");
    Buffer* b = new CrcBuffer{64};
    std::printf("== use through base pointer: %s, %zu bytes\n", b->kind(), b->size());
    std::printf("== delete through base pointer: derived first, then base\n");
    delete b;                                // bez virtual ~Buffer: UB
    std::printf("== done\n");
}

/* EXPECTED OUTPUT
== construction: base first, then derived
  Buffer ctor (64 bytes)
  CrcBuffer ctor (+256 entry table)
== use through base pointer: buffer with CRC table, 64 bytes
== delete through base pointer: derived first, then base
  CrcBuffer dtor
  Buffer dtor
== done
*/
