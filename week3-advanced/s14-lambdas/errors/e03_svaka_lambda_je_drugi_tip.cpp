// EXPECT-GCC: no match for 'operator='
// EXPECT-CLANG: no viable overloaded '='
// POGREŠNO: svaka lambda ima SVOJ, jedinstven tip (closure type), čak i
// kad su dve lambde napisane slovo po slovo isto. a i b su dva različita
// tipa, pa se jedna ne može dodeliti drugoj.
// Ispravno: kad promenljiva treba da drži "bilo koju funkciju ovog
// potpisa", std::function<int()> f = a; f = b;  (sekcija 9), ili pokazivač
// na funkciju za lambde bez capture-a.
int main() {
    auto a = [] { return 1; };
    auto b = [] { return 1; };
    a = b;
    return a();
}
