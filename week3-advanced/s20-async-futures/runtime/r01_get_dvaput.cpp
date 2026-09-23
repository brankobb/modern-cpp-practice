// EXPECT-RUN: terminate called after throwing an instance of 'std::future_error'
// POGREŠNO: get() preuzme rezultat; posle toga future.valid() == false.
// Standard: get() na future-u koji nije valid() je UB (prekršen
// preduslov) i preporučuje da biblioteka baci future_error. libstdc++ to
// radi ("No associated state") -- neuhvaćen: terminate.
// Ispravno: jedan get(); ako rezultat treba više puta, sačuvaj ga u
// promenljivu ili koristi shared_future.
#include <future>
int main() {
    auto f = std::async([] { return 1; });
    f.get();
    f.get();
}
