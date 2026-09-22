// STD: c++17
// EXPECT-GCC: redefinition of 'void f(Id)'
// EXPECT-CLANG: redefinition of 'f'
// POGREŠNO: using/typedef ne pravi novi tip -- Id JE int, pa su ovo dve
// definicije iste funkcije. (Za pravi novi tip: struct Id { int value; };
// ili enum class Id : int {};)
using Id = int;
void f(int) {}
void f(Id) {}
int main() {}
