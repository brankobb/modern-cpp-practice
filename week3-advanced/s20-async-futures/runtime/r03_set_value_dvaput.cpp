// EXPECT-RUN: std::future_error: Promise already satisfied
// POGREŠNO: promise je JEDNOKRATAN -- jedna vrednost ili jedan izuzetak.
// Drugi set_value baci future_error (promise_already_satisfied).
// Ispravno: za niz vrednosti treba red (queue) sa mutex-om i
// condition_variable, ili novi promise/future za svaku vrednost.
#include <future>
int main() {
    std::promise<int> p;
    p.set_value(1);
    p.set_value(2);
}
