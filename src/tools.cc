#include "tools.h"

#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "derived_functions.h"

namespace prf {

void PrintUsage(const std::string& program_name) {
  std::cout << "Usage: " << program_name << " -x NATURAL -y NATURAL\n"
            << "Compute x^y using primitive recursive functions.\n"
            << "Options:\n"
            << "  -x VALUE  Base natural number\n"
            << "  -y VALUE  Exponent natural number\n"
            << "  -h        Show this help\n";
}

Natural ParseNatural(const std::string& text, const std::string& option_name) {
  if (text.empty() || text[0] == '-') {
    throw std::invalid_argument("invalid value for " + option_name +
                                ": expected a non-negative integer");
  }
  for (const char character : text) {
    if (character < '0' || character > '9') {
      throw std::invalid_argument("invalid value for " + option_name +
                                  ": expected a non-negative integer");
    }
  }

  errno = 0;
  char* end = nullptr;
  const unsigned long long parsed = std::strtoull(text.c_str(), &end, 10);
  if (errno == ERANGE || end == nullptr || *end != '\0' ||
      parsed > std::numeric_limits<Natural>::max()) {
    throw std::invalid_argument("value for " + option_name + " is out of range");
  }
  return parsed;
}

Options ParseArguments(int argc, char* argv[]) {
  Options options;
  bool has_x = false;
  bool has_y = false;
  for (int i = 1; i < argc; ++i) {
    const std::string argument(argv[i]);
    if (argument == "-h" || argument == "--help") {
      if (options.help) {
        throw std::invalid_argument("option -h was provided more than once");
      }
      options.help = true;
      continue;
    }
    if (argument != "-x" && argument != "-y") {
      throw std::invalid_argument("unknown option: " + argument);
    }
    if (i + 1 >= argc) {
      throw std::invalid_argument("missing value for option " + argument);
    }
    const std::string value(argv[++i]);
    if (argument == "-x") {
      if (has_x) {
        throw std::invalid_argument("option -x was provided more than once");
      }
      options.x = ParseNatural(value, "-x");
      has_x = true;
    } else {
      if (has_y) {
        throw std::invalid_argument("option -y was provided more than once");
      }
      options.y = ParseNatural(value, "-y");
      has_y = true;
    }
  }
  if (options.help) {
    if (has_x || has_y) {
      throw std::invalid_argument("help cannot be combined with computation options");
    }
    return options;
  }
  if (!has_x || !has_y) {
    throw std::invalid_argument("both -x and -y are required");
  }
  return options;
}

PowerResult RunPower(Natural x, Natural y) {
  const auto power = MakePower();
  CallCounter counter;
  const Natural value = power->Evaluate({x, y}, counter);
  return {value, counter.Count()};
}

void PrintResult(const PowerResult& result) {
  std::cout << "Result: " << result.value << '\n'
            << "Function calls: " << result.calls << '\n';
}

}  // namespace prf
