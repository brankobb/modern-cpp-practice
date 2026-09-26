// EXPECT-GCC: no match for 'operator<' (operand types are 'Point' and 'const std::array<Point, 2>::value_type'
// EXPECT-CLANG: invalid operands to binary expression ('Point' and 'const value_type'
// POGREŠNO TEK PRI POZIVU: metode klasnog šablona se instanciraju tek kad
// se POZOVU ([temp.inst]). Stack<Point> je ispravan tip -- push i pop rade
// (main.cpp, sekcija 3). Greška nastaje tek kad se pozove largest(), jer
// ona traži operator<, a Point ga nema.
// Ispravno: ne zovi largest() za tip bez poređenja, ili daj Point
// operator< (ako ima smisla), ili largest(poredi) koji prima funkciju
// poređenja, kao std::max_element.
#include <array>
#include <cstddef>
template <typename T, std::size_t N>
class Stack {
public:
    void push(const T& v) { data_[size_++] = v; }
    T largest() const {
        T m = data_[0];
        for (std::size_t i = 1; i < size_; ++i)
            if (m < data_[i]) m = data_[i];
        return m;
    }

private:
    std::array<T, N> data_{};
    std::size_t size_ = 0;
};
struct Point {
    int x = 0, y = 0;
};
int main() {
    Stack<Point, 2> s;
    s.push({1, 2});
    return s.largest().x;
}
