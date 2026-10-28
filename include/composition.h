#ifndef PRF_COMPOSITION_H_
#define PRF_COMPOSITION_H_

#include <memory>
#include <utility>
#include <vector>

#include "function.h"

namespace prf {

class Composition final : public Function {
 public:
  Composition(std::shared_ptr<const Function> outer,
              std::vector<std::shared_ptr<const Function>> inner);

 protected:
  Natural Compute(const Arguments& args, CallCounter& counter) const override;

 private:
  std::shared_ptr<const Function> outer_;
  std::vector<std::shared_ptr<const Function>> inner_;
};

}  // namespace prf

#endif  // PRF_COMPOSITION_H_
