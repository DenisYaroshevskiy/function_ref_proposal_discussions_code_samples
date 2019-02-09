#pragma once

#include <type_traits>
#include <utility>
#include <exception>

template <typename R, typename... Args>
class function_ref;

template <typename R, typename... Args>
class function_ref<R(Args...)> {
  using no_first_parametr_ptr_t = R (*)(Args...);

  union erased_data {
    no_first_parametr_ptr_t function_without_first_parametr;
    void* ptr;
  };
  using erased_fn_t = R (*)(erased_data, Args...);

  // Type erasure tags
  static constexpr inline erased_fn_t null = [](erased_data, Args...) -> R {
    throw std::runtime_error(
        "attemtp to call empty function_ref");  // Probably want to help
                                                // non-throwing envirenmbents
                                                // here.
  };

  static constexpr inline erased_fn_t plain_function = [](erased_data data, Args... args) -> R {
    return data.function_without_first_parametr(args...);
  };

  erased_fn_t indirect_ = null;
  erased_data data_;

 public:
  constexpr function_ref() = default;
  constexpr function_ref(no_first_parametr_ptr_t func)
      : indirect_(plain_function) {
    data_.function_without_first_parametr = func;
  }

  template <typename T>
  /*constexpr*/ function_ref(R(T::*method)(Args...), std::remove_reference_t<T>& object) {  // object should match qualifiers
    // an intrinsic can do this.
    struct split_memptr {
        erased_fn_t fn;
        std::ptrdiff_t offset;
    };

    // This should be an intensic:
    split_memptr split = *reinterpret_cast<split_memptr*>(&method);
    if (split.offset) {
        throw std::runtime_error("Compiler should have removed the offset");
    }

    indirect_ = split.fn;
    data_.ptr = reinterpret_cast<void*>(&object);
    /*unsigned char* as_raw_bytes = reinterpret_cast<unsigned char*>(&object);
    as_raw_bytes += split.offset;
    data_.ptr = as_raw_bytes;
    */
  }

  constexpr R operator()(Args... args) const {
    return indirect_(data_, std::forward<Args>(args)...);
  }
};