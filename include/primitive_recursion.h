#ifndef PRF_PRIMITIVE_RECURSION_H_
#define PRF_PRIMITIVE_RECURSION_H_

#include <memory>
#include <utility>

#include "function.h"

namespace prf {

class PrimitiveRecursion final : public Function {
 public:
  PrimitiveRecursion(std::shared_ptr<const Function> base,
                     std::shared_ptr<const Function> step);

 protected:
  Natural Compute(const Arguments& args, CallCounter& counter) const override;

 private:
  std::shared_ptr<const Function> base_;
  std::shared_ptr<const Function> step_;
};

}  // namespace prf

#endif  // PRF_PRIMITIVE_RECURSION_H_
