// Drugi TU za errors/e08: helper() ima internal linkage.
namespace {
int helper(int x) { return x; }
} // namespace
int useHelper() { return helper(1); }
