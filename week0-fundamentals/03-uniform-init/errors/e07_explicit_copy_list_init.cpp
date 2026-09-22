// STD: c++17
// EXPECT-GCC: would use explicit constructor
// EXPECT-CLANG: chosen constructor is explicit
// POGREŠNO: explicit konstruktor kroz copy-list-initialization (= {}).
// Razlika u odnosu na e06: ovde se explicit konstruktor RAZMATRA, pobedi
// (tačan match za int), i tek onda je program neispravan. Zato
// "S s = 1;" bira S(long), a "S s = {1};" je greška.
struct S {
    explicit S(int) {}
    S(long) {}
};
int main() {
    S s = {1};
    (void)s;
}
