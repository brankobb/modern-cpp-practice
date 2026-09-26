// EXPECT-UB: heap-use-after-free
// POGREŠNO: operator= koji prvo obriše staru memoriju, pa kopira iz other.
// Zašto: kod a = a (ili a = alias, gde je alias referenca na a) other i
//   *this su ISTI objekat. delete[] data_ obriše i other.data_, pa se
//   zatim čita oslobođena memorija (EC++ Item 11). Dodela samom sebi
//   retko izgleda kao "a = a"; češće je v[i] = v[j] kad je i == j.
// Ispravno: copy-and-swap (main.cpp, sekcija 4): prvo napravi kopiju, pa
//   zameni. Radi i za a = a, i ostavlja objekat netaknut ako new baci.
#include <cstdio>
#include <cstring>

class Name {
public:
    explicit Name(const char* s) : data_(new char[std::strlen(s) + 1]) { std::strcpy(data_, s); }
    Name(const Name& o) : Name(o.data_) {}
    ~Name() { delete[] data_; }
    Name& operator=(const Name& other) {
        delete[] data_;
        data_ = new char[std::strlen(other.data_) + 1];
        std::strcpy(data_, other.data_);
        return *this;
    }
    const char* c_str() const { return data_; }

private:
    char* data_;
};

int main() {
    Name a("Ana");
    Name& alias = a;
    a = alias;
    std::printf("%s\n", a.c_str());
}
