# Sesija 10 — Vežba: sopstveni UniquePtr, Buffer u rule of 0, embedded RAII

## Deo 1 — `UniquePtr<T>`
Implementiraj minimalni move-only smart pointer: ctor(T*), dtor (delete),
move ctor/assignment, `= delete` na copy, `operator*`, `operator->`, `get()`,
`release()`, `reset()`.

## Deo 2 — Buffer u rule of 0
Prepiši `Buffer` iz s05 da umesto sirovog `int*` koristi
`std::unique_ptr<int[]>` (ili `std::vector<int>`) -- tada ti NIJE potreban
nijedan specijalni član, kompajler generiše ispravne move/copy/dtor sam
(rule of 0).

## Deo 3 — Embedded RAII
- lock guard: klasa čiji ctor zaključava mutex/spinlock, dtor otključava
- IRQ guard: ctor isključi prekide, dtor ih vrati (čuvaj prethodno stanje!)
- placement new na statičkom baferu: `alignas(T) unsigned char buf[sizeof(T)];`
  pa `T* p = new (buf) T(...);` i ručni `p->~T();` (bez heap alokacije --
  bitno za embedded gde heap možda ne postoji ili je zabranjen)

## Zapažanja posle vežbe
