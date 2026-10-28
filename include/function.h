#ifndef PRF_FUNCTION_H_
#define PRF_FUNCTION_H_

#include <cstddef>
#include <string>
#include <vector>

namespace prf {

using Natural = unsigned long long;
using Arguments = std::vector<Natural>;

class CallCounter {
 public:
  void Increment() noexcept { ++count_; }
  Natural Count() const noexcept { return count_; }
  void Reset() noexcept { count_ = 0; }

 private:
  Natural count_ = 0;
};

class Function {
 public:
  explicit Function(std::size_t arity) : arity_(arity) {}
  virtual ~Function() = default;

  Natural Evaluate(const Arguments& args, CallCounter& counter) const;
  std::size_t Arity() const noexcept { return arity_; }

 protected:
  virtual Natural Compute(const Arguments& args, CallCounter& counter) const = 0;

  std::size_t arity_;
};

}  // namespace prf

#endif  // PRF_FUNCTION_H_
