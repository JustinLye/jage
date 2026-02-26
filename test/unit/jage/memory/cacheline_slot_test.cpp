#include <jage/memory/cacheline_size.hpp>
#include <jage/memory/cacheline_slot.hpp>

#include <jage/test/construction_tracker.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

struct [[gnu::packed]] unaligned {
  std::uint8_t bits;
  std::uint64_t payload;
};

template <class T, auto DesiredSize> struct padded_value;

template <class T, auto DesiredSize>
  requires(sizeof(T) == DesiredSize)
struct padded_value<T, DesiredSize> {
  T value;
};

template <class T, auto DesiredSize>
  requires(sizeof(T) < DesiredSize)
struct padded_value<T, DesiredSize> {
  T value;
  std::array<std::byte, DesiredSize - sizeof(T)> padding;
};

template <auto Size = 0UZ> struct move_only;

template <auto Size>
  requires(Size > 8)
struct move_only<Size> {

  std::uint64_t value{};
  std::array<std::byte, Size - 8> padding{};

  move_only() = default;
  move_only(const move_only &) = delete;
  move_only(move_only &&other) noexcept : value{std::move(other.value)} {
    other.value = 42UZ;
  }

  auto operator=(const move_only &) -> move_only & = delete;
  auto operator=(move_only &&other) noexcept -> move_only & {
    value = std::move(other.value);
    other.value = 42UZ;
    return *this;
  }
};

template <auto Size>
  requires(Size == 0UZ)
struct move_only<Size> {

  std::uint64_t value{};

  move_only() = default;
  move_only(const move_only &) = delete;
  move_only(move_only &&other) noexcept : value{std::move(other.value)} {
    other.value = 42UZ;
  }

  auto operator=(const move_only &) -> move_only & = delete;
  auto operator=(move_only &&other) noexcept -> move_only & {
    value = std::move(other.value);
    other.value = 42UZ;
    return *this;
  }
};

class blood_pressure {
  std::uint16_t over_;
  std::uint16_t under_;

public:
  blood_pressure(std::uint16_t o, std::uint16_t u) : over_(o), under_(u) {}

  explicit operator std::string() const {
    return std::to_string(over_) + " / " + std::to_string(under_);
  }

  operator std::uint16_t() const { return over_; }
};

using jage::memory::cacheline_size;
using jage::memory::cacheline_slot;

using one_byte_over_cacheline_size =
    std::array<std::byte, cacheline_size + 1UZ>;
using equal_to_cacheline_size = std::array<std::byte, cacheline_size>;
using two_times_cacheline_size = std::array<std::byte, cacheline_size * 2UZ>;
using jage::test::construction_tracker;

TEST(memory_cacheline_slot, Pad_to_fit_on_cacheline) {
  EXPECT_EQ(cacheline_size, sizeof(cacheline_slot<unaligned>));
  EXPECT_EQ(cacheline_size * 2UZ,
            sizeof(cacheline_slot<two_times_cacheline_size>));
}

TEST(
    memory_cacheline_slot,
    Pad_to_additional_cacheline_when_size_of_type_is_greater_than_cacheline_size) {
  EXPECT_EQ(cacheline_size * 2UZ,
            sizeof(cacheline_slot<one_byte_over_cacheline_size>));
}

TEST(memory_cacheline_slot, Act_like_underlying_value) {
  auto value = cacheline_slot<unaligned>{unaligned{.bits = 1, .payload = 2}};
  EXPECT_EQ(1, value.bits);
  const auto payload = value.payload;
  EXPECT_EQ(2, payload);

  auto test = [&](const unaligned &f) {
    EXPECT_EQ(1, f.bits);
    const auto payload = f.payload;
    EXPECT_EQ(2, payload);
  };
  test(value);
}

TEST(memory_cacheline_slot,
     Handle_padding_correctly_when_payload_is_size_of_cacheline) {
  EXPECT_EQ(cacheline_size, sizeof(cacheline_slot<equal_to_cacheline_size>));
}

TEST(memory_cacheline_slot, Align_correctly) {
  EXPECT_EQ(alignof(cacheline_slot<unaligned>), cacheline_size);
  EXPECT_EQ(alignof(cacheline_slot<one_byte_over_cacheline_size>),
            cacheline_size);
  EXPECT_EQ(alignof(cacheline_slot<equal_to_cacheline_size>), cacheline_size);
  EXPECT_EQ(alignof(cacheline_slot<two_times_cacheline_size>), cacheline_size);
}

TEST(memory_cacheline_slot, Forward_constructor_arguments) {
  auto tracker = construction_tracker{};
  {
    cacheline_slot<construction_tracker> slot{tracker};
    EXPECT_EQ(construction_tracker::construction_type::copy,
              slot.construction_method());
  }
  {
    cacheline_slot<construction_tracker> slot{std::move(tracker)};
    EXPECT_EQ(construction_tracker::construction_type::move,
              slot.construction_method());
  }
  {
    move_only foo{};
    foo.value = 1888;
    cacheline_slot<move_only<>> slot{std::move(foo)};
    EXPECT_EQ(1888, slot.value);
    EXPECT_EQ(42, foo.value);
  }
  {
    move_only<cacheline_size> foo{};
    foo.value = 1984;
    cacheline_slot<move_only<cacheline_size>> slot{std::move(foo)};
    EXPECT_EQ(1984, slot.value);
    EXPECT_EQ(42, foo.value);
  }
  {
    const auto slot =
        cacheline_slot<blood_pressure>{std::uint16_t{120}, std::uint16_t{80}};
    EXPECT_EQ("120 / 80", static_cast<std::string>(slot));
  }
  {
    const padded_value<std::uint16_t, cacheline_size + 1UZ> foo{
        .value = 4615,
        .padding = {},
    };
    auto slot = cacheline_slot{foo};
    EXPECT_EQ(4615, slot.value);
  }

  {
    move_only<cacheline_size + 1UZ> foo{};
    foo.value = 1605;
    auto slot = cacheline_slot{std::move(foo)};
    EXPECT_EQ(1605UZ, slot.value);
    EXPECT_EQ(42UZ, foo.value);
  }
}

TEST(memory_cacheline_slot,
     Have_deduction_guides_for_constructing_with_the_underlying_type) {
  {
    move_only foo{};
    foo.value = 1999;
    auto slot = cacheline_slot{std::move(foo)};
    EXPECT_EQ(1999, slot.value);
    EXPECT_EQ(42, foo.value);
  }
  {
    padded_value<std::uint8_t, sizeof(std::uint8_t)> foo{};
    foo.value = 255;
    auto slot = cacheline_slot{foo};
    EXPECT_EQ(255, slot.value);
  }
  {
    const padded_value<std::uint8_t, sizeof(std::uint8_t)> foo{.value = 254};
    const auto slot = cacheline_slot{foo};
    EXPECT_EQ(254, slot.value);
  }
  {
    const auto tracker = construction_tracker{};
    auto slot = cacheline_slot{tracker};
    EXPECT_EQ(construction_tracker::construction_type::copy,
              slot.construction_method());
  }

  {
    const padded_value<std::uint16_t, cacheline_size> foo{.value = 65535,
                                                          .padding = {}};
    auto slot = cacheline_slot{foo};
    EXPECT_EQ(65535, slot.value);
  }
  {
    padded_value<std::uint16_t, cacheline_size> foo{};
    foo.value = 9;
    auto slot = cacheline_slot{foo};
    EXPECT_EQ(9, slot.value);
  }
  {
    padded_value<std::uint16_t, cacheline_size> foo{};
    foo.value = 10;
    auto slot = cacheline_slot{std::move(foo)};
    EXPECT_EQ(10, slot.value);
  }

  {
    const padded_value<std::uint16_t, cacheline_size + 1UZ> foo{.value = 77,
                                                                .padding = {}};
    auto slot = cacheline_slot{foo};
    EXPECT_EQ(77, slot.value);
  }
  {
    padded_value<std::uint16_t, cacheline_size + 1UZ> foo{};
    foo.value = 22;
    auto slot = cacheline_slot{foo};
    EXPECT_EQ(22, slot.value);
  }
  {
    padded_value<std::uint16_t, cacheline_size + 1UZ> foo{};
    foo.value = 111;
    auto slot = cacheline_slot{std::move(foo)};
    EXPECT_EQ(111, slot.value);
  }
}

TEST(primitive_types, Behave_like_primitive_type) {
  {
    auto slot = cacheline_slot<int>{42};
    EXPECT_EQ(42, slot);
  }
  {
    auto slot = cacheline_slot{std::numeric_limits<std::uint64_t>::max()};
    EXPECT_EQ(std::numeric_limits<std::uint64_t>::max(), slot);
  }
}
