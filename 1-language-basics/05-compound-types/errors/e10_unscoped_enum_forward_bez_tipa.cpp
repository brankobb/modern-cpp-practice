// STD: c++17
// EXPECT-GCC: use of enum 'Status' without previous declaration
// EXPECT-CLANG: ISO C++ forbids forward references to 'enum' types
// POGREŠNO: obična enum bez navedenog tipa ne može da se unapred deklariše --
// kompajler ne zna koliko je velika dok ne vidi sve elemente.
// Ispravno: enum Status : int;  ili  enum class Status;  (enum class je podrazumevano int)
enum Status;
int main() {}
