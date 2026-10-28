#include <exception>
#include <iostream>
#include <stdexcept>

#include "tools.h"

int main(int argc, char* argv[]) {
  try {
    const prf::Options options = prf::ParseArguments(argc, argv);
    if (options.help) {
      prf::PrintUsage(argv[0]);
      return 0;
    }
    prf::PrintResult(prf::RunPower(options.x, options.y));
    return 0;
  } catch (const std::invalid_argument& error) {
    std::cerr << "Usage error: " << error.what() << '\n';
    prf::PrintUsage(argv[0]);
    return 1;
  } catch (const std::overflow_error& error) {
    std::cerr << "Overflow error: " << error.what() << '\n';
    return 2;
  } catch (const std::exception& error) {
    std::cerr << "Error: " << error.what() << '\n';
    return 3;
  }
}
