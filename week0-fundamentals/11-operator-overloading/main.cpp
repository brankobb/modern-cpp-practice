#include <iostream>

class Vector2D {
public:
    Vector2D(double x, double y) : x_(x), y_(y) {}

    // member: levi operand je Vector2D
    Vector2D operator+(const Vector2D& rhs) const {
        return Vector2D(x_ + rhs.x_, y_ + rhs.y_);
    }

    double& operator[](int i) { // referenca -- dozvoljava i čitanje i pisanje
        // Ako dodaš bounds check koji BACA izuzetak ovde (MOŽE BITI
        // preterano za operator[]) jer je konvencija u STL-u (npr.
        // std::vector::operator[]) da je BRZ i BEZ provere -- provera ide
        // u .at().
        // Treba da NAMERNO ostaviš operator[] bez provere ako pratiš tu
        // konvenciju -- ali ONDA moraš jasno dokumentovati da pozivalac
        // garantuje validan indeks.
        // Možeš i dodati zaseban at(int i) sa bounds check (baca
        // std::out_of_range) za slučajeve kad je sigurnost bitnija od
        // brzine.
        return i == 0 ? x_ : y_;
    }

    double x() const { return x_; }
    double y() const { return y_; }

private:
    double x_, y_;
};

// Ako pišeš operator* KAO MEMBER Vector2D-a (NIJE DOBRO za ovaj slučaj)
// jer bi radio samo "vec * 2.0", NE "2.0 * vec" -- levi operand mora biti
// tvoje klase da bi member funkcija uopšte bila kandidat.
// Treba da napišeš SLOBODNU (free) funkciju kad levi operand NIJE tvoje
// klase -- kompajler tad traži operator* i van klase.
// Možeš i dodati i member i free verziju ako želiš da podržiš OBA
// redosleda (vec * 2.0 I 2.0 * vec) -- česta praksa za komutativne
// operacije.
Vector2D operator*(double scalar, const Vector2D& v) {
    return Vector2D(scalar * v.x(), scalar * v.y());
}

bool operator==(const Vector2D& a, const Vector2D& b) {
    return a.x() == b.x() && a.y() == b.y();
}

std::ostream& operator<<(std::ostream& os, const Vector2D& v) {
    return os << "(" << v.x() << ", " << v.y() << ")";
}

int main() {
    Vector2D a(1, 2), b(3, 4);

    std::cout << "-- operator+ (member) --\n";
    std::cout << "a + b = " << (a + b) << "\n";

    std::cout << "-- operator* (free function) --\n";
    std::cout << "2 * a = " << (2.0 * a) << "\n";

    std::cout << "-- operator== --\n";
    std::cout << "a == a: " << (a == a) << ", a == b: " << (a == b) << "\n";

    std::cout << "-- operator[] (bez bounds check) --\n";
    a[0] = 99; // operator[] vraća referencu -- ovo menja x_
    std::cout << "posle a[0]=99: " << a << "\n";

    // TODO: probaj a[5] -- nema bounds check, UB (undefined koje polje se menja)
}
