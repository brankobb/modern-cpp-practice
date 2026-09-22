# Sesija 2 — Kopiranje (C++03)

Izvori: Effective C++ (3. izd.) stavke 5–14, 29; learncpp.com (kopiranje)

- šta kompajler generiše sam od sebe (default ctor, copy ctor, copy assignment,
  dtor) i pod kojim uslovima to NE generiše
- shallow copy (kopira pokazivač) vs deep copy (kopira ono na šta pokazivač
  pokazuje) — zašto je shallow copy opasna kad klasa poseduje resurs
- self-assignment (`a = a;`) — zašto naivna implementacija puca
- rule of 3: ako pišeš jedno od {destructor, copy ctor, copy assignment},
  verovatno ti trebaju sva tri

## API korišćen u vežbi

- `std::strlen(s)` (header `<cstring>`) — vraća dužinu C-stil stringa (do
  prvog `\0`), NE uključujući sam terminator
- `std::strcpy(dst, src)` — kopira C-stil string uključujući terminator;
  NE proverava veličinu `dst` bafera — klasičan izvor buffer overflow-a
  ako `dst` nije dovoljno veliki (zato postoji `strncpy`, i zato moderni
  kod skoro uvek koristi `std::string` umesto sirovih C stringova)
- `new char[n]` / `delete[] data_` — dinamička alokacija NIZA; MORA se
  osloboditi sa `delete[]`, NE običnim `delete` (mismatch je UB)

## Zapažanja posle vežbe
