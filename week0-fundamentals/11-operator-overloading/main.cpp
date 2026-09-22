#include <iostream>

class Vector2D {
public:
    Vector2D(double x, double y) : x_(x), y_(y) {}

    // member: levi operand je Vector2D
    Vector2D operator+(const Vector2D& rhs) const {
        return Vector2D(x_ + rhs.x_, y_ + rhs.y_);
    }

    double& operator[](int i) { // referenca -- dozvoljava i čitanje i pisanje
        // NAMERNO bez bounds check (kao std::vector::operator[]) -- uporedi
        // sa .at() koje bi bacilo izuzetak
        return i == 0 ? x_ : y_;
    }

    double x() const { return x_; }
    double y() const { return y_; }

private:
    double x_, y_;
};

// free function: levi operand (scalar) NIJE Vector2D
Vector2D operator*(double scalar, const Vector2D& v) {
    return Vector2D(scalar * v.x(), scalar * v.y());
}

// free function: poredi sve relevantne članove
bool operator==(const Vector2D& a, const Vector2D& b) {
    return a.x() == b.x() && a.y() == b.y();
}

// free function: levi operand je ostream, ne Vector2D
std::ostream& operator<<(std::ostream& os, const Vector2D& v) {
    return os << "(" << v.x() << ", " << v.y() << ")";
}

int main() {
    Vector2D a(1, 2), b(3, 4);

    std::cout << "a + b = " << (a + b) << "\n";
    std::cout << "2 * a = " << (2.0 * a) << "\n";
    std::cout << "a == a: " << (a == a) << ", a == b: " << (a == b) << "\n";

    a[0] = 99; // operator[] vraća referencu -- ovo menja x_
    std::cout << "posle a[0]=99: " << a << "\n";

    // TODO: probaj a[5] -- nema bounds check, UB (undefined koje polje se menja)
}
