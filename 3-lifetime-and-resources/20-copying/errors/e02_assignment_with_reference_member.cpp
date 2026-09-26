// STD: c++17
// EXPECT-GCC: use of deleted function 'View& View::operator=(const View&)'
// EXPECT-CLANG: copy assignment operator is implicitly deleted
// POGREŠNO: dodela objekta koji ima referencu kao član.
// Zašto: referenca se ne može "preusmeriti" na drugi objekat. Dodela
//   a.target = b.target bi promenila VREDNOST x, a ne na šta a pokazuje, pa
//   kompajler radije obriše operator=.
// Ispravno: pokazivač (int* target) ili std::reference_wrapper<int> kao
//   član, ako objekat treba da se dodeljuje.
struct View {
    int& target;
};

int main() {
    int x = 1;
    int y = 2;
    View a{x};
    View b{y};
    a = b;
    return a.target;
}
