// EXPECT-UB: stack-use-after-scope
// POGREŠNO: klasa čuva pokazivač na SOPSTVENI član, a kopija je kompajlerova.
// Zašto: cursor_ pokazuje u buf_ istog objekta. Kopija član po član kopira
//   i cursor_, pa kopija pokazuje u buf_ ORIGINALA, a ne u svoj. Dok
//   original živi, greška se ne vidi (samo čita pogrešan bafer); kad nestane,
//   kopija čita memoriju koja više ne pripada nikome.
// Ispravno: čuvaj INDEKS umesto pokazivača (std::size_t pos_), pa kopija
//   radi sama; ili napiši copy ctor koji preračuna cursor_ za novi buf_.
#include <cstdio>
#include <cstring>

class Parser {
public:
    explicit Parser(const char* text) { std::strncpy(buf_, text, sizeof buf_ - 1); }
    char next() { return *cursor_++; }

private:
    char buf_[16] {};
    char* cursor_ = buf_;
};

int main() {
    Parser* copy = nullptr;
    {
        Parser original("xyz");
        copy = new Parser(original);
    }
    std::printf("%c\n", copy->next());
    delete copy;
}
