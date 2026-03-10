#pragma once

#include <bitset>
#include <cstdint>
#include <utility>

namespace jage::test {
class construction_tracker {
  enum class construction_type_ : std::uint8_t {
    by_default,
    copy,
    move,
    copy_assigned = 4,
    move_assigned = 8
  };

  std::bitset<4> method_of_construction_{};

public:
  using construction_type = construction_type_;

  construction_tracker() = default;
  construction_tracker(const construction_tracker &)
      : method_of_construction_{std::to_underlying(construction_type::copy)} {}
  construction_tracker(construction_tracker &&) noexcept
      : method_of_construction_{std::to_underlying(construction_type::move)} {}

  auto operator=(const construction_tracker &) -> construction_tracker & {
    method_of_construction_ =
        std::to_underlying(construction_type::copy_assigned);
    return *this;
  }

  auto operator=(construction_tracker &&) noexcept -> construction_tracker & {
    method_of_construction_ =
        std::to_underlying(construction_type::move_assigned);
    return *this;
  }

  [[nodiscard]] auto construction_method() const noexcept -> construction_type {
    return static_cast<construction_type>(method_of_construction_.to_ulong());
  }
};

} // namespace jage::test
