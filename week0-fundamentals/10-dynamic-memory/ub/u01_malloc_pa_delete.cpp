// EXPECT-UB: alloc-dealloc-mismatch \(malloc vs operator delete\)
// POGREŠNO: memorija od malloc-a oslobođena sa delete.
// Zašto: malloc/free i new/delete su dva RAZLIČITA alokatora. delete pozove
//   destruktor i operator delete, koji nije obavezan da zna za malloc memoriju
//   ([expr.delete]: pokazivač mora doći od new). Kod int-a često "prođe",
//   pa se greška ne primeti dok se alokator ne promeni.
//   g++ -Wall ovo vidi i pri kompajliranju (-Wmismatched-new-delete).
// Ispravno: malloc ide sa free, new sa delete, new[] sa delete[].
#include <cstdlib>

int main() {
    int* p = static_cast<int*>(std::malloc(sizeof(int)));
    delete p;
}
