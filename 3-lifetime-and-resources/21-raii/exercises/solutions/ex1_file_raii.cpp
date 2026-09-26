// Rešenje zadatka ex1_file_raii.

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

int closedCount = 0;
void closeFile(FILE* f) {
    if (f) {
        std::fclose(f);
        ++closedCount;
    }
}

// Korak 1: resurs se zauzme u konstruktoru, oslobodi u destruktoru. Ako
// konstruktor baci, resurs nije zauzet, pa nema šta da se oslobodi.
class TempFile {
public:
    TempFile() : f_(std::tmpfile()) {
        if (!f_) throw std::runtime_error("tmpfile failed");
    }
    ~TempFile() { closeFile(f_); }
    TempFile(const TempFile&) = delete;              // dva vlasnika = dva fclose
    TempFile& operator=(const TempFile&) = delete;

    void write(const std::string& s) { std::fputs(s.c_str(), f_); }
    std::string readAll() {
        std::rewind(f_);
        std::string r;
        for (int c = std::fgetc(f_); c != EOF; c = std::fgetc(f_)) r += static_cast<char>(c);
        return r;
    }

private:
    FILE* f_;
};

// Korak 2: unique_ptr sa deleter-om je gotov RAII vlasnik za C resurse.
// Deleter kao prazan struct ne povećava sizeof (za razliku od pokazivača
// na funkciju kao deleter-a).
struct Closer {
    void operator()(FILE* f) const { closeFile(f); }
};
using FilePtr = std::unique_ptr<FILE, Closer>;

// Korak 3: najopštiji RAII -- "uradi ovo na izlasku iz bloka".
// C++17: tip se izvede iz konstruktora (CTAD), pa ScopeGuard([]{...}) radi
// bez <...>.
template <typename F>
class ScopeGuard {
public:
    explicit ScopeGuard(F f) : f_(std::move(f)) {}
    ~ScopeGuard() { f_(); }
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

private:
    F f_;
};

int main() {
    {
        TempFile file;
        file.write("first line\n");
        file.write("second line");
        std::cout << "contents: [" << file.readAll() << "]\n";
    }
    std::cout << "closed: " << closedCount << '\n';

    {
        FilePtr f(std::tmpfile());
        std::fputs("x", f.get());
    }
    std::cout << "closed: " << closedCount << '\n';

    try {
        auto g = ScopeGuard([] { std::cout << "guard: end of block\n"; });
        throw std::runtime_error("error in block");
    } catch (const std::exception& e) {
        std::cout << "caught: " << e.what() << '\n';
    }
}
