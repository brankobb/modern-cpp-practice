// STD: c++20
// EXPECT-GCC: designator order
// EXPECT-CLANG: specified in declaration order
// POGREŠNO: designatori moraju ići redosledom deklaracije članova
// (za razliku od C-a, gde je bilo koji redosled dozvoljen).
// clang ovo podrazumevano prijavljuje samo kao warning (-Wreorder-init-list),
// zato build.ps1 za clang dodaje -Werror=reorder-init-list.
struct Point {
    int x;
    int y;
};
int main() {
    Point p{.y = 20, .x = 10};
    (void)p;
}
