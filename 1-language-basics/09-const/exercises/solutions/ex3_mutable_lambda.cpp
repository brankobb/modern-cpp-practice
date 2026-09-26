// Rešenje zadatka ex3_mutable_lambda.

#include <iostream>

template <typename F>
void callThreeTimes(F f) {
    for (int i = 0; i < 3; ++i) f();
}

// b) po referenci: radi na ISTOM objektu lambde, pa mutable stanje ostaje.
template <typename F>
void callThreeTimesRef(F& f) {
    for (int i = 0; i < 3; ++i) f();
}

int main() {
    // Ako pišeš [id]() { return ++id; } (ne kompajlira se): zarobljena
    // kopija je const. Ako dodaš mutable i proslediš lambdu po vrednosti
    // (nije dobro): brojač se uveća u kopiji, a original ostane na 0.
    //
    // Treba ovako: a) stanje drži van lambde, a lambda ga menja preko
    // reference. Kopije lambde dele istu referencu. (Pazi da promenljiva
    // živi duže od lambde -- lekcija 27.)
    int next = 0;
    auto genA = [&next] { return ++next; };
    callThreeTimes(genA);
    std::cout << "a) genA() after: " << genA() << '\n';

    // Možeš i ovako: b) mutable, ali lambdu prosleđuj po referenci.
    auto genB = [id = 0]() mutable { return ++id; };
    callThreeTimesRef(genB);
    std::cout << "b) genB() after: " << genB() << '\n';
}
