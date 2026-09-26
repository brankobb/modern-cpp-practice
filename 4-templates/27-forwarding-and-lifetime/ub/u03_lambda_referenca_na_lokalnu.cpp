// EXPECT-UB: stack-use-after-scope
// POGREŠNO: lambda hvata lokalnu promenljivu po referenci, a živi duže od nje.
// Zašto: [&counter] čuva referencu. getter nadživi blok u kom je counter,
//   pa poziv čita promenljivu koje više nema (EMC Item 31). Isto se desi kad
//   se takva lambda vrati iz funkcije ili preda niti/callback-u.
// Ispravno: capture po vrednosti ([counter]), ili init capture
//   ([c = std::move(x)]) za move-only tipove (main.cpp, sekcija 8).
#include <cstdio>
#include <functional>

int main() {
    std::function<int()> getter;
    {
        int counter = 42;
        getter = [&counter] { return counter; };
    }
    std::printf("%d\n", getter());
}
