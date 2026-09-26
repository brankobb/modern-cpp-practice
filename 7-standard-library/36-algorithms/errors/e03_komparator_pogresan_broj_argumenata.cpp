// EXPECT-GCC: no match for call to '(main()::<lambda(int)>) (int&, int&)'
// EXPECT-CLANG: no matching function for call to object of type '(lambda at
// POGREŠNO: komparator za sort prima DVA elementa i vraća "a ide pre b".
// Lambda sa jednim parametrom je predikat (za find_if, count_if...), ne
// komparator. Kompajler to otkrije tek kad sort pokuša da je pozove sa
// dva argumenta, pa poruka pokazuje na <bits/predefined_ops.h>.
// Ispravno: std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
#include <algorithm>
#include <vector>
int main() {
    std::vector<int> v{3, 1, 2};
    std::sort(v.begin(), v.end(), [](int a) { return a > 0; });
    return v[0];
}
