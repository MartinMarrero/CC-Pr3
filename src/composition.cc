#include "composition.h"

#include <stdexcept>
#include <utility>

namespace prf {

Composition::Composition(
    std::shared_ptr<const Function> outer,
    std::vector<std::shared_ptr<const Function>> inner)
    : Function(0), outer_(std::move(outer)), inner_(std::move(inner)) {
  if (outer_ == nullptr) {
    throw std::invalid_argument("composition outer function cannot be null");
  }
  if (inner_.empty()) {
    throw std::invalid_argument("composition requires at least one inner function");
  }
  if (outer_->Arity() != inner_.size()) {
    throw std::invalid_argument("outer function arity must match inner function count");
  }
  if (inner_[0] == nullptr) {
    throw std::invalid_argument("composition inner function cannot be null");
  }
  arity_ = inner_[0]->Arity();
  for (const auto& function : inner_) {
    if (function == nullptr) {
      throw std::invalid_argument("composition inner function cannot be null");
    }
    if (function->Arity() != arity_) {
      throw std::invalid_argument("all inner functions must have the same arity");
    }
  }
}

Natural Composition::Compute(const Arguments& args, CallCounter& counter) const {
  Arguments intermediate;
  intermediate.reserve(inner_.size());
  for (const auto& function : inner_) {
    intermediate.push_back(function->Evaluate(args, counter));
  }
  return outer_->Evaluate(intermediate, counter);
}

}  // namespace prf
