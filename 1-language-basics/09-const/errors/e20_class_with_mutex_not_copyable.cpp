// STD: c++17
// EXPECT-GCC: use of deleted function 'Cache::Cache(const Cache&)'
// EXPECT-CLANG: call to implicitly-deleted copy constructor of 'Cache'
// POGREŠNO: std::mutex (i std::atomic) ne može ni da se kopira ni da se
// premesti. Klasa koja ga ima kao (mutable) član zato gubi implicitni copy
// i move konstruktor (EMC Item 16 -- cena thread-safe const funkcija).
// Ispravno: napiši sopstveni copy ctor koji kopira podatke, a mutex pravi nov;
// ili drži objekat preko std::unique_ptr / po referenci.
#include <mutex>
class Cache {
public:
    int get() const {
        std::lock_guard<std::mutex> lock(m_);
        return value_;
    }
private:
    mutable std::mutex m_;
    int value_ = 0;
};
int main() {
    Cache a;
    Cache b = a;
    return b.get();
}
