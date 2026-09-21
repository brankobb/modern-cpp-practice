#include <iostream>
#include <new>
#include <utility>

// Deo 1: sopstveni move-only smart pointer.
template <typename T>
class UniquePtr {
public:
    UniquePtr() noexcept = default;
    explicit UniquePtr(T* p) noexcept : ptr_(p) {}

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    ~UniquePtr() { delete ptr_; }

    T& operator*() const { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }
    T* get() const noexcept { return ptr_; }

    T* release() noexcept {
        T* p = ptr_;
        ptr_ = nullptr;
        return p;
    }

    void reset(T* p = nullptr) noexcept {
        delete ptr_;
        ptr_ = p;
    }

private:
    T* ptr_ = nullptr;
};

// Deo 3: RAII guard bez heap-a (npr. embedded lock guard).
class FakeLock {
public:
    void lock() { std::cout << "lock\n"; }
    void unlock() { std::cout << "unlock\n"; }
};

class LockGuard {
public:
    explicit LockGuard(FakeLock& lock) : lock_(lock) { lock_.lock(); }
    ~LockGuard() { lock_.unlock(); }
    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

private:
    FakeLock& lock_;
};

struct Widget {
    Widget(int id) : id_(id) { std::cout << "Widget(" << id_ << ")\n"; }
    ~Widget() { std::cout << "~Widget(" << id_ << ")\n"; }
    int id_;
};

int main() {
    UniquePtr<int> p(new int(42));
    std::cout << *p << "\n";

    {
        FakeLock lk;
        LockGuard guard(lk);
        std::cout << "critical section\n";
    } // unlock ovde automatski

    // placement new na statičkom baferu -- bez heap alokacije
    alignas(Widget) unsigned char buf[sizeof(Widget)];
    Widget* w = new (buf) Widget(7);
    w->~Widget(); // ručno pozvan destruktor, obavezno za placement new
}
