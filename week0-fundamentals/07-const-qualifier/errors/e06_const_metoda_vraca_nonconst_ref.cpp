// STD: c++17
// EXPECT-GCC: to 'const int' discards qualifiers
// EXPECT-CLANG: drops 'const' qualifier
// POGREŠNO: u const funkciji je value_ const int, pa se ne može vezati za int&.
// Inače bi pozivalac kroz vraćenu referencu menjao const objekat.
// Ispravno: const int& get() const { return value_; }
class Box {
public:
    int& get() const { return value_; }
private:
    int value_ = 0;
};
int main() {
    const Box b;
    return b.get();
}
