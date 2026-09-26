// Rešenje zadatka ex1_new_and_single_block.

#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

// Korak 1: new[] se oslobađa sa delete[] (ne delete, ne free -- ub/u01..u03).
// Vraćanje sirovog vlasničkog pokazivača prebacuje odgovornost na
// pozivaoca, i ništa ga ne tera da je ispuni.
int* squares(std::size_t n) {
    int* p = new int[n];
    for (std::size_t i = 0; i < n; ++i) p[i] = static_cast<int>(i * i);
    return p;
}

// Korak 2: vlasništvo je u tipu -- pozivalac ne može da zaboravi delete[].
std::unique_ptr<int[]> squaresUnique(std::size_t n) {
    auto p = std::make_unique<int[]>(n);   // value-init: sve nule
    for (std::size_t i = 0; i < n; ++i) p[i] = static_cast<int>(i * i);
    return p;
}

// Korak 3: jedan blok umesto niza redova -- jedna alokacija, elementi jedan
// do drugog u memoriji (dobro za keš), i nema ručnog oslobađanja redova.
class Matrix {
public:
    Matrix(std::size_t rows, std::size_t cols)
        : rows_(rows), cols_(cols), data_(rows * cols) {}

    double& at(std::size_t r, std::size_t c) { return data_.at(r * cols_ + c); }
    const double& at(std::size_t r, std::size_t c) const { return data_.at(r * cols_ + c); }

    void print() const {
        for (std::size_t r = 0; r < rows_; ++r) {
            for (std::size_t c = 0; c < cols_; ++c) std::cout << (c ? " " : "") << at(r, c);
            std::cout << '\n';
        }
    }

private:
    std::size_t rows_;
    std::size_t cols_;
    std::vector<double> data_;
};

int main() {
    int* a = squares(5);
    for (std::size_t i = 0; i < 5; ++i) std::cout << (i ? " " : "") << a[i];
    std::cout << '\n';
    delete[] a;

    std::unique_ptr<int[]> b = squaresUnique(5);
    for (std::size_t i = 0; i < 5; ++i) std::cout << (i ? " " : "") << b[i];
    std::cout << '\n';

    Matrix m(2, 3);
    for (std::size_t r = 0; r < 2; ++r)
        for (std::size_t c = 0; c < 3; ++c) m.at(r, c) = static_cast<double>(10 * r + c);
    m.print();
}
