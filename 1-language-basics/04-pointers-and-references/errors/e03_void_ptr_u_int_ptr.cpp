// STD: c++17
// EXPECT-GCC: invalid conversion from 'void*' to 'int*'
// EXPECT-CLANG: of type 'int *' with an lvalue of type 'void *'
// POGREŠNO u C++ (ispravno u C-u): void* -> int* NIJE implicitna konverzija.
// Ispravno: int* ip = static_cast<int*>(vp);
int main() {
    int x = 1;
    void* vp = &x;
    int* ip = vp;
    return *ip;
}
