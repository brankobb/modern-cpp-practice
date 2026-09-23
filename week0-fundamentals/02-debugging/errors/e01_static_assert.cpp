// EXPECT-GCC: static assertion failed: kod pretpostavlja 64-bitni int
// EXPECT-CLANG: static assertion failed due to requirement 'sizeof(int) == 8': kod pretpostavlja 64-bitni int
// POGREŠNA PRETPOSTAVKA uhvaćena pri kompajliranju: static_assert je
// provera koja NE postoji u programu -- ako ne važi, programa ni nema.
// Ovde je pretpostavka pogrešna (int je 4 bajta), pa build pada sa
// porukom koju si sam napisao. To je poenta: bolje odmah greška pri
// kompajliranju nego tihi pogrešan rezultat na drugoj platformi.
// Ispravno: std::int64_t ako ti treba 64 bita (lekcija 01, sekcija 1).
static_assert(sizeof(int) == 8, "kod pretpostavlja 64-bitni int");
int main() {}
