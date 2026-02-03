#pragma once

namespace jage::test::fakes::concurrency {
template <class T, template <class> class TAtomic> class double_buffer {
  T value_;

public:
  [[nodiscard]] auto read() const -> T { return value_; }
  auto write(const T &desired) -> void { value_ = desired; }
};
} // namespace jage::test::fakes::concurrency