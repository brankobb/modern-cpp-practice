// EXPECT-RUN: std::future_error: Broken promise
// POGREŠNO: promise je uništen bez set_value/set_exception. Da future ne
// bi čekao zauvek, destruktor promise-a upiše izuzetak future_error sa
// kodom broken_promise -- i get() ga baci (main.cpp, sekcija 5, ga hvata).
// Ispravno: postavi vrednost ili izuzetak na SVAKOJ putanji (i kad
// posao ne uspe: set_exception(std::current_exception())).
#include <future>
int main() {
    std::future<int> f;
    {
        std::promise<int> p;
        f = p.get_future();
    }
    return f.get();
}
