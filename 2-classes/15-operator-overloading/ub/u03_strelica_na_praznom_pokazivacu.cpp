// EXPECT-UB: member access within null pointer
// POGREŠNO: ptr->value na praznom (nullptr) pametnom pokazivaču.
// Zašto: operator-> samo vrati sačuvani pokazivač, pa ptr->value postaje
//   nullptr->value. Isto važi za std::unique_ptr: ni on ne proverava.
// Ispravno: proveri pre upotrebe (if (ptr) ...), ili napravi objekat tako
//   da nikad nije prazan.
#include <cstdio>

struct Widget {
    int value = 7;
};

template <typename T>
class ScopedPtr {
public:
    explicit ScopedPtr(T* p = nullptr) : p_(p) {}
    ~ScopedPtr() { delete p_; }
    ScopedPtr(const ScopedPtr&) = delete;
    ScopedPtr& operator=(const ScopedPtr&) = delete;
    T* operator->() const { return p_; }

private:
    T* p_;
};

int main() {
    ScopedPtr<Widget> w;
    std::printf("%d\n", w->value);
}
