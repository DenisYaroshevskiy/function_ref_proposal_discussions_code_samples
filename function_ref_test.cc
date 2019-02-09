#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <cinttypes>
#include <iostream>

#include "function_ref.h"

namespace {

struct no_default {
  no_default(int x) : data{x} {}

  int data;
};

no_default plain_function(int x) { return no_default{x}; }

struct static_function {
  static no_default function(int x) { return no_default(x); }
};


}  // namespace

namespace tools {

TEST_CASE("function_ref, empty") {
  function_ref<no_default(int)> f;
  try {
    f(3);
  } catch (const std::runtime_error&) {
  }
}

TEST_CASE("function_ref, function") {
  function_ref<no_default(int)> f{plain_function};

  CHECK(f(5).data == 5);

  f = static_function::function;
  CHECK(f(4).data == 4);
}

TEST_CASE("funciton_ref, member + this no virtual") {
  struct base_class_A {
    int dataA = 1;
    no_default functionA(int x) { return no_default(dataA * x); }
  };

  struct base_class_B {
    int dataB = 2;
    no_default functionB(int x) { return no_default(dataB * 3); }
  };

  struct multiple_baseclasses : base_class_A, base_class_B {};

  multiple_baseclasses object;

  function_ref<no_default(int)> f{&multiple_baseclasses::functionA, object};
  CHECK(f(3).data == 3);
  f = function_ref<no_default(int)>{&multiple_baseclasses::functionB, object};
  CHECK(f(3).data == 6);
}

}  // namespace tools