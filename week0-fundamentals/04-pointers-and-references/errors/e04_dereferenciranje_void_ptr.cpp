// STD: c++17
// EXPECT-GCC: 'void*' is not a pointer-to-object type
// EXPECT-CLANG: indirection not permitted on operand of type 'void *'
// POGREŠNO: void* se ne može dereferencirati -- kompajler ne zna tip ni veličinu.
int main() {
    int x = 1;
    void* vp = &x;
    return *vp;
}
