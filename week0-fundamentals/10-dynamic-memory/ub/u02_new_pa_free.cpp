// EXPECT-UB: alloc-dealloc-mismatch \(operator new vs free\)
// POGREŠNO: memorija od new oslobođena sa free.
// Zašto: free ne poziva destruktor (za klasu sa std::string to je i curenje),
//   i ne zna kako operator new čuva memoriju. Obrnuti slučaj od u01.
//   g++ -Wall ovo vidi i pri kompajliranju (-Wmismatched-new-delete).
// Ispravno: delete p;
#include <cstdlib>

int main() {
    int* p = new int(5);
    std::free(p);
}
