// Rešenje zadatka ex2_bind_copies.

#include <functional>
#include <iostream>

void send(int& counter, const char* message) {
    std::cout << "sending: " << message << '\n';
    ++counter;
}

int main() {
    int sent = 0;
    // Ako bind-u daš promenljivu direktno (nije dobro kad funkcija treba da
    // je menja): bind čuva kopiju, pa se menja kopija.
    // Treba ovako: std::ref -- bind tada čuva referencu.
    //   std::function<void()> onClick = std::bind(send, std::ref(sent), "ping");
    // Možeš i ovako (bolje, EMC Item 34): lambda, gde se vidi šta je referenca.
    std::function<void()> onClick = [&sent] { send(sent, "ping"); };
    onClick();
    onClick();
    onClick();
    std::cout << "messages sent: " << sent << '\n';
}
