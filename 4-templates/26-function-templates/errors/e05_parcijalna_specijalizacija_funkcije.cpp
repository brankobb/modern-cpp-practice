// EXPECT-GCC: non-class, non-variable partial specialization 'ispisi<T*>' is not allowed
// EXPECT-CLANG: function template partial specialization is not allowed
// POGREŠNO: funkcijski šablon se ne može DELIMIČNO specijalizovati (samo
// klasni i promenljivi šabloni, lekcija 28). Za funkcije postoji samo potpuna
// specijalizacija (template<>) -- a i nju treba izbegavati (zadatak ex3).
// Ispravno: OVERLOAD -- template <typename T> void ispisi(T*) je poseban
// šablon, i overload resolution bira "specijalniji" (main.cpp, sekcija 5).
template <typename T>
void ispisi(T) {}
template <typename T>
void ispisi<T*>(T*) {}
int main() {}
