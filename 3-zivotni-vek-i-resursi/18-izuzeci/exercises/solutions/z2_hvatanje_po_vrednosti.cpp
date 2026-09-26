// Rešenje zadatka z2_hvatanje_po_vrednosti.

#include <iostream>
#include <stdexcept>
#include <string>

class GreskaSenzora : public std::runtime_error {
public:
    GreskaSenzora(int senzorId, const std::string& poruka)
        : std::runtime_error("senzor " + std::to_string(senzorId) + ": " + poruka), id_(senzorId) {}
    int id() const noexcept { return id_; }

private:
    int id_;
};

// Ako hvataš po vrednosti i prosleđuješ sa "throw e;" (nije dobro): obe
// operacije prave kopiju tipa runtime_error, pa pravi tip izuzetka nestane.
// Treba ovako: catch po const& (bez kopije, bez slicing-a) i "throw;"
// (isti objekat ide dalje).
void obradi() {
    try {
        throw GreskaSenzora(7, "timeout");
    } catch (const std::runtime_error& e) {
        std::cout << "log: " << e.what() << '\n';
        throw;
    }
}

int main() {
    try {
        obradi();
    } catch (const GreskaSenzora& e) {
        std::cout << "spolja: GreskaSenzora, id " << e.id() << '\n';
    } catch (const std::exception& e) {
        std::cout << "spolja: opšta greška (" << e.what() << ")\n";
    }
}
