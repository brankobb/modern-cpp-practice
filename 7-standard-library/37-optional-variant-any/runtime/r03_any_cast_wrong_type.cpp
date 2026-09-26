// EXPECT-RUN: terminate called after throwing an instance of 'std::bad_any_cast'
// POGREŠNO: any_cast<T> traži TAČNO taj tip -- bez konverzija. U any je
// int, pa any_cast<long> baca std::bad_any_cast, iako se int inače
// konvertuje u long.
// Ispravno: any_cast<int>; ili any_cast<long>(&a) (pokazivač, nullptr
// ako tip nije tačan); ili a.type() == typeid(int) pre cast-a.
#include <any>
int main() {
    std::any a = 5;
    return static_cast<int>(std::any_cast<long>(a));
}
