// EXPECT-GCC: cannot convert 'std::future<int>' to 'int' in initialization
// EXPECT-CLANG: no viable conversion from 'future<__async_result_of<int (&)()>>' (aka 'future<int>') to 'int'
// POGREŠNO: std::async ne vraća rezultat, nego OBEĆANJE rezultata --
// std::future<int>. Vrednost se dobija tek sa get(), koji po potrebi čeka.
// Ispravno: int x = std::async(racunaj).get();
// ili sačuvaj future i pozovi get() kad ti vrednost zaista treba.
#include <future>
int racunaj() { return 42; }
int main() {
    int x = std::async(racunaj);
    return x;
}
