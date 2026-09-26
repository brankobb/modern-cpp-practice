// Rešenje zadatka ex1_config_errors.

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Korak 1: nasledi runtime_error -- on čuva poruku (i kopira se bez
// bacanja), a what() je virtual, pa radi i preko std::exception&.
class ConfigError : public std::runtime_error {
public:
    ConfigError(int lineNumber, const std::string& message)
        : std::runtime_error("line " + std::to_string(lineNumber) + ": " + message), line_(lineNumber) {}
    int line() const noexcept { return line_; }

private:
    int line_;
};

// Korak 2: prevođenje -- detalj implementacije (stoi) ne curi napolje.
int readValue(const std::string& text, int lineNumber) {
    auto eq = text.find('=');
    if (eq == std::string::npos) throw ConfigError(lineNumber, "missing '='");
    std::string value = text.substr(eq + 1);
    try {
        return std::stoi(value);
    } catch (const std::logic_error&) {      // invalid_argument i out_of_range
        throw ConfigError(lineNumber, "value is not a number: '" + value + "'");
    }
}

// Korak 3: kontekst ("šta smo radili") dodaje se na svakom nivou, a
// originalni uzrok ostaje unutra.
int load(const std::vector<std::string>& lines) {
    try {
        int sum = 0;
        for (std::size_t i = 0; i < lines.size(); ++i)
            sum += readValue(lines[i], static_cast<int>(i) + 1);
        return sum;
    } catch (...) {
        std::throw_with_nested(std::runtime_error("configuration not loaded"));
    }
}

void printChain(const std::exception& e, int level = 0) {
    std::cout << std::string(static_cast<std::size_t>(level) * 2, ' ') << e.what() << '\n';
    try {
        std::rethrow_if_nested(e);
    } catch (const std::exception& cause) {
        printChain(cause, level + 1);
    }
}

int main() {
    std::cout << "sum: " << load({"a=10", "b=20", "c=30"}) << '\n';
    for (const std::vector<std::string>& config :
         {std::vector<std::string>{"a=10", "b=abc"}, std::vector<std::string>{"a=10", "b 20"}}) {
        try {
            load(config);
        } catch (const std::exception& e) {
            printChain(e);
        }
    }
}
