// STD: c++17
// EXPECT-GCC: unable to deduce 'auto' from '& process'
// EXPECT-CLANG: <overloaded function type>
// POGREŠNO: &process ne kaže KOJI overload -- auto nema ciljni tip iz kog bi
// kompajler izabrao.
// Ispravno: void (*p)(int) = &process;  ili  static_cast<void (*)(int)>(&process)
void process(int) {}
void process(double) {}
int main() {
    auto p = &process;
    p(1);
}
