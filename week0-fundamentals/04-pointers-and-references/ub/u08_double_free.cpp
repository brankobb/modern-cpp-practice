// EXPECT-UB: double-free
// UB: dvostruko oslobađanje. Tipično kad dva sirova pokazivača "misle" da
// poseduju isti objekat (week1 s02 -- shallow copy).
int main() {
    int* p = new int(42);
    int* q = p;
    delete p;
    delete q;
}
