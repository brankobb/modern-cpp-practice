// EXPECT-UB: ThreadSanitizer: lock-order-inversion \(potential deadlock\)
// SANITIZER: thread
// POGREŠNO (deadlock, ne UB u užem smislu): jedna funkcija zaključa a pa
// b, druga b pa a. Ako se niti prepletu -- prva drži a i čeka b, druga
// drži b i čeka a -- obe čekaju zauvek. Ovde se niti izvršavaju jedna
// posle druge, pa se program ZAVRŠI; TSan ipak prijavi opasnost jer vidi
// suprotan redosled zaključavanja istih mutex-a.
// Ispravno: uvek isti redosled, ili std::scoped_lock sa oba mutex-a
// odjednom (C++17, sekcija 6): std::scoped_lock l(a, b);
#include <iostream>
#include <mutex>
#include <thread>
std::mutex a, b;
int stanjeA = 100, stanjeB = 100;
void prenesiAuB() {
    std::lock_guard<std::mutex> ga(a);
    std::lock_guard<std::mutex> gb(b);
    --stanjeA;
    ++stanjeB;
}
void prenesiBuA() {
    std::lock_guard<std::mutex> gb(b);
    std::lock_guard<std::mutex> ga(a);
    --stanjeB;
    ++stanjeA;
}
int main() {
    std::thread t1(prenesiAuB);
    t1.join();
    std::thread t2(prenesiBuA);
    t2.join();
    std::cout << stanjeA << ' ' << stanjeB << '\n';
}
