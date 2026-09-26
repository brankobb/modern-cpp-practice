// KIND: usage
//
// Zadatak 1 -- interfejs, override, final, virtualni destruktor i clone()
// (sekcije 4, 5, 9, 12)
//   ./build.sh 2-classes/16-inheritance-and-polymorphism/exercises/ex1_shapes.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_shapes.cpp
//
// Korak 1: apstraktna klasa Shape: čiste virtualne funkcije
//   double area() const i std::string name() const, i virtualni
//   destruktor (= default). Kopiranje zabrani (C.67: polimorfna klasa se
//   ne kopira direktno -- zato postoji clone() u koraku 3).
// Korak 2: Circle(double r) i Rectangle(double a, double b) nasleđuju
//   Shape (public). Square(double a) nasleđuje Rectangle i on je final.
//   Svaka funkcija koja nadjačava ima override. Square menja samo name().
//   Za pi koristi 3.14159.
// Korak 3: virtual std::unique_ptr<Shape> clone() const = 0; u Shape, i
//   implementacija u svakoj klasi (return std::make_unique<Circle>(*this);
//   -- izvedena klasa SME da se kopira, pa joj treba protected copy
//   konstruktor u Shape umesto = delete).

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 i 2 -- otkomentariši:
    // std::vector<std::unique_ptr<Shape>> shapes;
    // shapes.push_back(std::make_unique<Circle>(1.0));
    // shapes.push_back(std::make_unique<Rectangle>(2.0, 3.0));
    // shapes.push_back(std::make_unique<Square>(4.0));
    // double total = 0;
    // for (const auto& s : shapes) {
    //     std::cout << s->name() << ": " << s->area() << '\n';
    //     total += s->area();
    // }
    // std::cout << "total: " << total << '\n';

    // Korak 3 -- otkomentariši:
    // std::unique_ptr<Shape> copy = shapes[2]->clone();
    // std::cout << "copy: " << copy->name() << ' ' << copy->area() << '\n';
}

/* EXPECTED OUTPUT
circle: 3.14159
rectangle: 6
square: 16
total: 25.1416
copy: square 16
*/
