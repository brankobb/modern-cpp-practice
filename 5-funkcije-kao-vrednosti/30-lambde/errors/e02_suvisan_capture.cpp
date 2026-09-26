// EXPECT-GCC: explicit by-copy capture of 'x' redundant with by-copy capture default
// EXPECT-CLANG: '&' must precede a capture when the capture default is '='
// POGREŠNO: posle [=] se sme navesti samo ono što se zarobljava DRUGAČIJE
// (po referenci: &x). [=, x] ponavlja podrazumevano, i to standard ne
// dozvoljava. (g++ bez -pedantic-errors samo upozori; ovde build koristi
// -pedantic-errors, pa je greška na oba.) Isto važi obrnuto: [&, &x].
// Ispravno: [=] ili [x]; za mešavinu [=, &y] ili [&, x].
int main() {
    int x = 1;
    auto f = [=, x] { return x; };
    return f();
}
