#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "function_ref.h"

namespace {

struct no_default {
  no_default(int) {}
};

}  // namespace

namespace tools {

TEST_CASE("function_ref, empty") {
    function_ref<no_default(int)> f;
    try {
      f(3);
    } catch(const std::runtime_error&) {}
}

}  // namespace tools