// Korak 2 -- BONUS 3 (embedded): RingBuffer bez heap-a, trivially copyable.
//   ./build.sh roadmap/step-2-object-lifecycle/solutions/bonus3_ring_buffer.cpp
//
// Zašto "trivially copyable" znači nešto u embedded-u: takav objekat sme da
// se kopira sa memcpy (DMA, deljena memorija, snimanje u flash/EEPROM,
// slanje preko UART-a kao niz bajtova). Uslov (pojednostavljeno): nijedna
// specijalna funkcija kopiranja/pomeranja/destrukcije NIJE korisnička,
// nema virtual funkcija ni virtual baze, i to važi za sve članove.
// Dakle: Rule of 0 + članovi koji su i sami trivijalni.
//
// Šta bi pokvarilo trivially copyable (probaj, static_assert ispod pukne):
//   - ~RingBuffer() {}                       -- čak i prazan, korisnički destruktor
//   - RingBuffer(const RingBuffer&) {...}
//   - virtual bilo šta                       -- vptr ne sme da se memcpy-uje
//   - član std::string / std::vector         -- oni nisu trivijalni

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <optional>
#include <string>
#include <type_traits>

template <typename T, std::size_t N>
class RingBuffer {
    static_assert(N > 0 && (N & (N - 1)) == 0, "N mora da bude stepen dvojke (indeks preko maske, bez %)");
    static_assert(std::is_trivially_copyable_v<T>, "elementi moraju da budu trivially copyable");

public:
    // Nijedan konstruktor nije napisan: NSDMI ispod daje početno stanje,
    // a kompajler generiše sve ostalo (Rule of 0).

    bool push(const T& value) noexcept {
        if (full()) return false;            // embedded: bez izuzetaka, vraćamo status
        data_[(head_ + count_) & kMask] = value;
        ++count_;
        return true;
    }

    std::optional<T> pop() noexcept {
        if (empty()) return std::nullopt;
        T value = data_[head_];
        head_ = (head_ + 1) & kMask;
        --count_;
        return value;
    }

    std::size_t size() const noexcept { return count_; }
    bool empty() const noexcept { return count_ == 0; }
    bool full() const noexcept { return count_ == N; }
    static constexpr std::size_t capacity() noexcept { return N; }

private:
    static constexpr std::size_t kMask = N - 1;
    std::array<T, N> data_{};                // statički: deo objekta, nema new
    std::size_t head_ = 0;
    std::size_t count_ = 0;
};

using UartRx = RingBuffer<std::uint8_t, 8>;

static_assert(std::is_trivially_copyable_v<UartRx>, "sme memcpy");
static_assert(sizeof(UartRx) == 8 + 2 * sizeof(std::size_t), "nema skrivenog heap-a ni vptr-a");
static_assert(UartRx::capacity() == 8);
// static_assert(std::is_trivially_copyable_v<RingBuffer<std::string, 4>>);  // ne kompajlira se: string nije trivijalan

template <typename T, std::size_t N>
void drain(const char* name, RingBuffer<T, N>& rb) {
    std::printf("  %s:", name);
    while (auto v = rb.pop()) std::printf(" %u", static_cast<unsigned>(*v));
    std::printf("\n");
}

int main() {
    std::printf("== push until full\n");
    UartRx rx;
    for (std::uint8_t b = 1; b <= 10; ++b) {
        if (!rx.push(b)) std::printf("  overflow, dropped %u\n", static_cast<unsigned>(b));
    }
    std::printf("  size %zu, full %s\n", rx.size(), rx.full() ? "yes" : "no");

    std::printf("== wrap-around\n");
    rx.pop();
    rx.pop();
    rx.push(11);
    rx.push(12);                             // upisano na početak niza (indeksi 0 i 1)

    std::printf("== memcpy copy (like a DMA or a flash snapshot)\n");
    UartRx snapshot;
    std::memcpy(&snapshot, &rx, sizeof rx);  // dozvoljeno samo zato što je trivially copyable
    UartRx copy = rx;                        // obična kopija: kompajler je generisao isto to
    drain("original", rx);
    drain("memcpy  ", snapshot);
    drain("copy    ", copy);
    std::printf("  original empty after drain: %s, snapshot unaffected\n", rx.empty() ? "yes" : "no");
}

/* EXPECTED OUTPUT
== push until full
  overflow, dropped 9
  overflow, dropped 10
  size 8, full yes
== wrap-around
== memcpy copy (like a DMA or a flash snapshot)
  original: 3 4 5 6 7 8 11 12
  memcpy  : 3 4 5 6 7 8 11 12
  copy    : 3 4 5 6 7 8 11 12
  original empty after drain: yes, snapshot unaffected
*/
