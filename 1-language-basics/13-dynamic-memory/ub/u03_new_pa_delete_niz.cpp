// EXPECT-UB: alloc-dealloc-mismatch \(operator new vs operator delete \[\]\)
// POGREŠNO: jedan objekat od new oslobođen sa delete[] (EC++ Item 16).
// Zašto: delete[] očekuje niz. Za klase sa destruktorom čita broj elemenata
//   koji je new[] sačuvao ispred niza. Kod new tog broja nema, pa delete[]
//   čita smeće i poziva destruktor nepoznat broj puta. Obrnuti slučaj (new[] + delete) je
//   u lekciji 04, ub/u09. g++ i clang sa -Wall ovo vide i pri kompajliranju.
// Ispravno: isti oblik: new/delete, new[]/delete[].
int main() {
    int* p = new int(5);
    delete[] p;
}
