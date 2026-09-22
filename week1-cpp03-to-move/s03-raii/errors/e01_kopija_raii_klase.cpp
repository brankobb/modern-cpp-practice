// STD: c++17
// EXPECT-GCC: use of deleted function 'File::File(const File&)'
// EXPECT-CLANG: call to deleted constructor of 'File'
// POGREŠNO: kopija RAII klase koja drži jedinstven resurs (FILE*).
// Zašto: dve kopije bi imale isti FILE*, pa bi dva destruktora pozvala
//   fclose na istom fajlu (EC++ Item 14: odluči šta znači kopija RAII
//   objekta). Zato je kopija obrisana, i greška je pri kompajliranju.
// Ispravno: prosledi po referenci (void use(File& f)), ili prebaci
//   vlasništvo move-om (s04), ili deli resurs sa std::shared_ptr (week2 s08).
#include <cstdio>

class File {
public:
    File() : handle_(std::tmpfile()) {}
    ~File() {
        if (handle_ != nullptr) std::fclose(handle_);
    }
    File(const File&) = delete;
    File& operator=(const File&) = delete;

private:
    std::FILE* handle_;
};

int main() {
    File a;
    File b = a;
    (void)b;
}
