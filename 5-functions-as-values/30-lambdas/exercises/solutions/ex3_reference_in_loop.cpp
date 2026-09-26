// Rešenje zadatka ex3_reference_in_loop.

#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

int main() {
    std::vector<std::function<void()>> tasks;
    for (int i = 0; i < 3; ++i) {
        int id = 100 + i;
        // Ako lambda koja se čuva za kasnije hvata [&] (nije dobro):
        // reference na promenljive iteracije vise čim iteracija završi.
        // Treba ovako: zadatak nosi svoju kopiju.
        tasks.push_back([id] { std::cout << "task " << id << '\n'; });
    }
    for (const auto& z : tasks) z();
}
