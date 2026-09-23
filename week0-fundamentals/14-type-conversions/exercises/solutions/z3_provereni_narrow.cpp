// Rešenje zadatka z3_provereni_narrow.

#include <iostream>
#include <limits>
#include <stdexcept>

long izKonfiguracije() { return 100000; }

// Ako pišeš static_cast<short>(v) za vrednost spolja (nije dobro): van
// opsega se tiho dobije druga vrednost (za double van opsega je čak UB,
// ub/u02). Treba ovako: konvertuj, pa proveri da li se vrednost sačuvala.
// (Isto radi gsl::narrow iz Core Guidelines support biblioteke.)
template <typename To, typename From>
To narrow(From v) {
    To r = static_cast<To>(v);
    if (static_cast<From>(r) != v || ((r < To{}) != (v < From{})))
        throw std::range_error("narrow");
    return r;
}

template <typename To, typename From>
void probaj(From v) {
    try {
        To r = narrow<To>(v);   // prvo konverzija: ako baci, ništa nije ispisano
        std::cout << v << " -> " << r << '\n';
    } catch (const std::range_error&) {
        std::cout << v << " -> range_error\n";
    }
}

int main() {
    probaj<short>(1234L);
    probaj<short>(izKonfiguracije());
    probaj<short>(static_cast<long>(std::numeric_limits<short>::max()));
    probaj<short>(static_cast<long>(std::numeric_limits<short>::max()) + 1);
    probaj<unsigned>(-1);
    probaj<int>(3.0);
    probaj<int>(3.5);
}
