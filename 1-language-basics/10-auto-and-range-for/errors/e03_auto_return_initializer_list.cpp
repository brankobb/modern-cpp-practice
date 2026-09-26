// STD: c++17
// EXPECT-GCC: returning initializer list
// EXPECT-CLANG: cannot deduce return type from initializer list
// POGREŠNO (EMC Item 2): auto kao povratni tip koristi pravila za TEMPLATE,
// ne pravila za auto promenljive -- a template ne dedukuje iz {1, 2, 3}.
// Ispravno: std::initializer_list<int> f() { ... } ili std::vector<int> f()
auto makeList() {
    return {1, 2, 3};
}
int main() {
    makeList();
}
