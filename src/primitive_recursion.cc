#include "primitive_recursion.h"

#include <stdexcept>
#include <utility>

namespace prf {

PrimitiveRecursion::PrimitiveRecursion(
    std::shared_ptr<const Function> base,
    std::shared_ptr<const Function> step)
    : Function(0), base_(std::move(base)), step_(std::move(step)) {
  if (base_ == nullptr || step_ == nullptr) {
    throw std::invalid_argument("primitive recursion functions cannot be null");
  }
  if (step_->Arity() != base_->Arity() + 2) {
    throw std::invalid_argument(
        "step arity must equal base arity plus two");
  }
  arity_ = base_->Arity() + 1;
}

Natural PrimitiveRecursion::Compute(const Arguments& args,
                                    CallCounter& counter) const {
  Arguments fixed_arguments(args.begin(), args.end() - 1);
  const Natural recursion_argument = args.back();
  if (recursion_argument == 0) {
    return base_->Evaluate(fixed_arguments, counter);
  }

  Arguments previous_arguments = fixed_arguments;
  previous_arguments.push_back(recursion_argument - 1);
  const Natural previous = Evaluate(previous_arguments, counter);

  Arguments step_arguments = fixed_arguments;
  step_arguments.push_back(recursion_argument - 1);
  step_arguments.push_back(previous);
  return step_->Evaluate(step_arguments, counter);
}

}  // namespace prf
