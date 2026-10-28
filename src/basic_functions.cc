#include "basic_functions.h"

#include <limits>
#include <stdexcept>

namespace prf {

Natural Zero::Compute(const Arguments& args, CallCounter& counter) const {
  (void)args;
  (void)counter;
  return 0;
}

Natural Successor::Compute(const Arguments& args, CallCounter& counter) const {
  (void)counter;
  if (args[0] == std::numeric_limits<Natural>::max()) {
    throw std::overflow_error("natural number overflow in successor");
  }
  return args[0] + 1;
}

Projection::Projection(std::size_t index, std::size_t arity)
    : Function(arity), index_(index) {
  if (index == 0 || index > arity) {
    throw std::invalid_argument("projection index must satisfy 1 <= index <= arity");
  }
}

Natural Projection::Compute(const Arguments& args, CallCounter& counter) const {
  (void)counter;
  return args[index_ - 1];
}

}  // namespace prf
