// EXPECT-UB: stack-use-after-scope
// POGREŠNO: član-referenca vezan za privremeni objekat.
// Zašto: Greeter g("Ana") napravi privremeni std::string iz "Ana" i veže
//   name_ za njega. Privremeni nestaje na kraju te linije, a name_ ostaje
//   da pokazuje na njega. Pravilo o produženju života (lekcija 04,
//   sekcija 8) ne važi za reference koje se vežu kroz konstruktor.
//   Ni g++ ni clang sa -Wall ovde ne upozoravaju.
// Ispravno: član po vrednosti (std::string name_;). Referencu kao član
//   koristi samo kad je jasno da objekat na koji pokazuje živi duže.
#include <cstdio>
#include <string>

class Greeter {
public:
    explicit Greeter(const std::string& name) : name_(name) {}
    void greet() const { std::printf("Zdravo, %s\n", name_.c_str()); }

private:
    const std::string& name_;
};

int main() {
    Greeter g("Ana");
    g.greet();
}
