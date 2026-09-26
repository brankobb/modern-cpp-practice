// STD: c++17
// EXPECT-GCC: use of deleted function 'UniquePtr<T>::UniquePtr(const UniquePtr<T>&) [with T = int]'
// EXPECT-CLANG: call to deleted constructor of 'UniquePtr<int>'
// POGREŠNO: kopija sopstvenog UniquePtr-a.
// Zašto: kopija je namerno obrisana -- dva UniquePtr-a sa istim pokazivačem
//   bi ga obrisala dvaput. To je cela razlika između "pametnog" pokazivača
//   i omotača oko sirovog: kompajler brani invarijantu "tačno jedan vlasnik".
// Ispravno: UniquePtr<int> b = std::move(a); (a postaje prazan).
#include <utility>

template <typename T>
class UniquePtr {
public:
    explicit UniquePtr(T* p = nullptr) noexcept : ptr_(p) {}
    ~UniquePtr() { delete ptr_; }
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;
    UniquePtr(UniquePtr&& o) noexcept : ptr_(std::exchange(o.ptr_, nullptr)) {}

private:
    T* ptr_;
};

int main() {
    UniquePtr<int> a(new int(1));
    UniquePtr<int> b = a;
    (void)b;
}
