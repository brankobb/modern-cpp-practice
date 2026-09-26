// EXPECT-GCC: looser exception specification on overriding virtual function
// EXPECT-CLANG: exception specification of overriding function is more lax than base version
// POGREŠNO: bazna funkcija obećava noexcept, a pozivalac preko Sensor& se
// na to oslanja. Izvedena klasa ne sme da prekrši obećanje baze.
// Ispravno: double read() const noexcept override -- i u telu ne bacati
// (grešku vrati kao vrednost, ili je obradi unutra).
struct Sensor {
    virtual ~Sensor() = default;
    virtual double read() const noexcept { return 0.0; }
};
struct ThermoSensor : Sensor {
    double read() const override { return 21.5; }
};
int main() {
    ThermoSensor t;
    return static_cast<int>(t.read());
}
