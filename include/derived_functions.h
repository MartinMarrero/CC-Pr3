#ifndef PRF_DERIVED_FUNCTIONS_H_
#define PRF_DERIVED_FUNCTIONS_H_

#include <memory>

#include "function.h"

namespace prf {

std::shared_ptr<const Function> MakeSum();
std::shared_ptr<const Function> MakeProduct();
std::shared_ptr<const Function> MakePower();

}  // namespace prf

#endif  // PRF_DERIVED_FUNCTIONS_H_
