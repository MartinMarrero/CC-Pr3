#include "function.h"

#include <stdexcept>

namespace prf {

Natural Function::Evaluate(const Arguments& args, CallCounter& counter) const {
  if (args.size() != arity_) {
    throw std::invalid_argument("wrong number of arguments: expected " +
                                std::to_string(arity_) + ", got " +
                                std::to_string(args.size()));
  }
  counter.Increment();
  return Compute(args, counter);
}

}  // namespace prf
