// EXPECT-UB: alloc-dealloc-mismatch
// UB (EC++ Item 16): new[] mora da se oslobodi sa delete[], a new sa delete.
// Ispravno: delete[] p;  -- ili std::vector<int> / std::unique_ptr<int[]>
int main() {
    int* p = new int[5];
    delete p;
}
