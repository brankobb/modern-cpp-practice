// EXPECT-GCC: 'class std::lock_guard<std::mutex>' has no member named 'unlock'
// EXPECT-CLANG: no member named 'unlock' in 'std::lock_guard<std::mutex>'
// POGREŠNO: lock_guard je najprostiji RAII omotač: zaključa u
// konstruktoru, otključa u destruktoru, i ništa drugo.
// Ispravno: kraći opseg ({ std::lock_guard<std::mutex> g(m); ... }), ili
// std::unique_lock<std::mutex> ul(m); ul.unlock(); -- ume i unlock/lock.
#include <mutex>
std::mutex m;
int main() {
    std::lock_guard<std::mutex> g(m);
    g.unlock();
}
