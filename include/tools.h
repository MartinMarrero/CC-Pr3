#ifndef PRF_TOOLS_H_
#define PRF_TOOLS_H_

#include <string>

#include "function.h"

namespace prf {

struct Options {
  Natural x = 0;
  Natural y = 0;
  bool help = false;
};

struct PowerResult {
  Natural value;
  Natural calls;
};

void PrintUsage(const std::string& program_name);
Options ParseArguments(int argc, char* argv[]);
Natural ParseNatural(const std::string& text, const std::string& option_name);
PowerResult RunPower(Natural x, Natural y);
void PrintResult(const PowerResult& result);

}  // namespace prf

#endif  // PRF_TOOLS_H_
