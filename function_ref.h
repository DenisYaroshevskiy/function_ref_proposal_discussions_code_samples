#pragma once

#include <exception>

namespace tools {

template <typename R, typename... Args>
class function_ref;

template <typename R, typename... Args>
class function_ref<R(Args...)> {
  using no_first_parametr_ptr_t = R (*)(Args...);

  union erased_data {
    no_first_parametr_ptr_t function_without_first_parametr;
    void* data_;
  };
  using erased_fn_t = R (*)(const function_ref&, Args...);

  // Type erasure tags
  static constexpr inline erased_fn_t null = [](const function_ref&, Args...) -> R {
    throw std::runtime_error("attemtp to call empty function_ref");  // Probably want to help non-throwing
                                                                     // envirenmbents here.
  };

  erased_fn_t indirect_ = null;
  erased_data data_;

 public:

   function_ref() = default;
   function_ref(const function_ref&) noexcept = default;
   function_ref(function_ref&&) noexcept = default;

   function_ref& operator=(const function_ref&) noexcept = default;
   function_ref& operator=(function_ref&&) noexcept = default;

   ~function_ref() noexcept = default;

   R operator()(Args ... args) const {
       return indirect_(*this, std::forward<Args>(args)...);
   }

};

}  // namespace tools