// STD: c++17
// EXPECT-GCC: cannot declare variable 's' to be of abstract type 'Shape'
// EXPECT-CLANG: variable type 'Shape' is an abstract class
// POGREŠNO: objekat klase sa pure virtual funkcijom.
// Zašto: "= 0" znači "nema implementacije, izvedena klasa MORA da je da".
//   Shape s; bi bio objekat čiji s.area() nema šta da pozove. Klasa sa bar
//   jednom pure virtual funkcijom je apstraktna ([class.abstract]); izvedena
//   klasa koja ne implementira SVE takve funkcije je takođe apstraktna.
// Ispravno: objekat konkretne izvedene klase (Circle c(2);), a Shape samo
//   kroz referencu ili pokazivač (const Shape& s = c;).
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
};

int main() {
    Shape s;
    return static_cast<int>(s.area());
}
