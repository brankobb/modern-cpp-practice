// STD: c++17
// EXPECT-GCC: cannot convert 'std::nullptr_t' to 'int'
// EXPECT-CLANG: of type 'int' with an rvalue of type 'std::nullptr_t'
// POGREŠNO: nullptr se implicitno konvertuje SAMO u pokazivače (i bool u
// direktnoj inicijalizaciji), ne u int. Baš zato je bezbedniji od 0 i NULL.
int main() {
    int n = nullptr;
    return n;
}
