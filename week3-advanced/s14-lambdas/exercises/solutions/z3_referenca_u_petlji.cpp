// Rešenje zadatka z3_referenca_u_petlji.

#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

int main() {
    std::vector<std::function<void()>> zadaci;
    for (int i = 0; i < 3; ++i) {
        int id = 100 + i;
        // Ako lambda koja se čuva za kasnije hvata [&] (nije dobro):
        // reference na promenljive iteracije vise čim iteracija završi.
        // Treba ovako: zadatak nosi svoju kopiju.
        zadaci.push_back([id] { std::cout << "zadatak " << id << '\n'; });
    }
    for (const auto& z : zadaci) z();
}
