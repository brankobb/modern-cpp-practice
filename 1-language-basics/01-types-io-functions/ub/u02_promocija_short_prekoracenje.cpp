// EXPECT-UB: signed integer overflow: 65535 \* 65535 cannot be represented in type 'int'
// UB iz promocije: oba operanda su unsigned short, ali se pre množenja
// promovišu u INT ([conv.prom]). 65535 * 65535 = 4294836225 ne staje u
// int -- signed prekoračenje, iako nigde nema signed tipa u kodu.
// Ispravno: pomnoži u tipu dovoljne širine, i unsigned:
//   static_cast<std::uint32_t>(a) * b   (ili unsigned long long)
#include <iostream>
int main(int argc, char**) {
    unsigned short a = static_cast<unsigned short>(65534 + argc);   // 65535
    unsigned short b = a;
    std::cout << a * b << '\n';
}
