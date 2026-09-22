// STD: c++17
// EXPECT-GCC: use of deleted function 'InterruptGuard::InterruptGuard(const InterruptGuard&)'
// EXPECT-CLANG: call to deleted constructor of 'InterruptGuard'
// POGREŠNO: kopija RAII guard-a (prekidi, lock).
// Zašto: kopija bi imala isto zapamćeno stanje, pa bi se prekidi vratili
//   dvaput, i to u pogrešnom trenutku (kad nestane prva kopija, dok druga
//   još "štiti" kritičnu sekciju). Isto važi za std::lock_guard: dva
//   unlock-a istog mutex-a su UB.
// Ispravno: guard se ne kopira ni ne pomera; živi tačno u bloku koji štiti.
bool interruptsEnabled = true;

class InterruptGuard {
public:
    InterruptGuard() noexcept : wasEnabled_(interruptsEnabled) { interruptsEnabled = false; }
    ~InterruptGuard() { interruptsEnabled = wasEnabled_; }
    InterruptGuard(const InterruptGuard&) = delete;
    InterruptGuard& operator=(const InterruptGuard&) = delete;

private:
    bool wasEnabled_;
};

int main() {
    InterruptGuard guard;
    InterruptGuard copy = guard;
    (void)copy;
}
