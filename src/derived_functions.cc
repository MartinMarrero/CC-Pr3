#include "derived_functions.h"

#include <memory>
#include <vector>

#include "basic_functions.h"
#include "composition.h"
#include "primitive_recursion.h"

namespace prf {

std::shared_ptr<const Function> MakeSum() {
  auto base = std::make_shared<Projection>(1, 1);
  auto step = std::make_shared<Composition>(
      std::make_shared<Successor>(),
      std::vector<std::shared_ptr<const Function>>{
          std::make_shared<Projection>(3, 3)});
  return std::make_shared<PrimitiveRecursion>(base, step);
}

std::shared_ptr<const Function> MakeProduct() {
  auto sum = MakeSum();
  auto base = std::make_shared<Zero>(1);
  auto step = std::make_shared<Composition>(
      sum, std::vector<std::shared_ptr<const Function>>{
               std::make_shared<Projection>(3, 3),
               std::make_shared<Projection>(1, 3)});
  return std::make_shared<PrimitiveRecursion>(base, step);
}

std::shared_ptr<const Function> MakePower() {
  auto product = MakeProduct();
  auto base = std::make_shared<Composition>(
      std::make_shared<Successor>(),
      std::vector<std::shared_ptr<const Function>>{
          std::make_shared<Zero>(1)});
  auto step = std::make_shared<Composition>(
      product, std::vector<std::shared_ptr<const Function>>{
                   std::make_shared<Projection>(3, 3),
                   std::make_shared<Projection>(1, 3)});
  return std::make_shared<PrimitiveRecursion>(base, step);
}

}  // namespace prf
