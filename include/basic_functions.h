#ifndef PRF_BASIC_FUNCTIONS_H_
#define PRF_BASIC_FUNCTIONS_H_

#include "function.h"

namespace prf {

class Zero final : public Function {
 public:
  explicit Zero(std::size_t arity) : Function(arity) {}

 protected:
  Natural Compute(const Arguments& args, CallCounter& counter) const override;
};

class Successor final : public Function {
 public:
  Successor() : Function(1) {}

 protected:
  Natural Compute(const Arguments& args, CallCounter& counter) const override;
};

class Projection final : public Function {
 public:
  Projection(std::size_t index, std::size_t arity);

 protected:
  Natural Compute(const Arguments& args, CallCounter& counter) const override;

 private:
  std::size_t index_;
};

}  // namespace prf

#endif  // PRF_BASIC_FUNCTIONS_H_
