// Rešenje zadatka z2_bind_kopira.

#include <functional>
#include <iostream>

void posalji(int& brojac, const char* poruka) {
    std::cout << "šaljem: " << poruka << '\n';
    ++brojac;
}

int main() {
    int poslato = 0;
    // Ako bind-u daš promenljivu direktno (nije dobro kad funkcija treba da
    // je menja): bind čuva kopiju, pa se menja kopija.
    // Treba ovako: std::ref -- bind tada čuva referencu.
    //   std::function<void()> naKlik = std::bind(posalji, std::ref(poslato), "ping");
    // Možeš i ovako (bolje, EMC Item 34): lambda, gde se vidi šta je referenca.
    std::function<void()> naKlik = [&poslato] { posalji(poslato, "ping"); };
    naKlik();
    naKlik();
    naKlik();
    std::cout << "poslato poruka: " << poslato << '\n';
}
