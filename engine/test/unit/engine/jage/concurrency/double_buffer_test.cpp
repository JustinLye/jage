#include <jage/concurrency/double_buffer.hpp>
#include <jage/memory/cacheline_size.hpp>
#include <jage/time/durations.hpp>

#include <jage/test/mocks/concurrency/atomic.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

using jage::concurrency::double_buffer;
using jage::memory::cacheline_size;
using jage::test::mocks::concurrency::atomic;

using one_byte_over_cache_line = std::array<std::byte, cacheline_size + 1UZ>;
using equal_to_cache_line = std::array<std::byte, cacheline_size>;
using twice_the_cache_line = std::array<std::byte, cacheline_size * 2UZ>;

struct [[gnu::packed]] unaligned {
  std::uint64_t value{42};
  std::uint8_t padding{};
};

static_assert(sizeof(double_buffer<one_byte_over_cache_line, atomic>) %
                  cacheline_size ==
              0);
static_assert(sizeof(double_buffer<one_byte_over_cache_line, std::atomic>) %
                  cacheline_size ==
              0);
static_assert(sizeof(double_buffer<unaligned, atomic>) % cacheline_size == 0);
static_assert(sizeof(double_buffer<unaligned, std::atomic>) % cacheline_size ==
              0);
static_assert(sizeof(double_buffer<equal_to_cache_line, atomic>) %
                  cacheline_size ==
              0);
static_assert(sizeof(double_buffer<equal_to_cache_line, std::atomic>) %
                  cacheline_size ==
              0);
static_assert(sizeof(double_buffer<twice_the_cache_line, atomic>) %
                  cacheline_size ==
              0);
static_assert(sizeof(double_buffer<twice_the_cache_line, std::atomic>) %
                  cacheline_size ==
              0);

TEST(concurrency_double_buffer,
     Have_default_constructed_value_when_initialized) {
  auto &mock = *atomic<std::uint8_t>::get_instance();
  auto buffer = double_buffer<unaligned, atomic>{};
  static_assert(sizeof(buffer) % cacheline_size == 0);

  EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
      .WillOnce(testing::Return(0U));
  const auto payload = buffer.read();
  const auto value = payload.value;
  EXPECT_EQ(42UZ, value);
  atomic<std::uint8_t>::instance.reset();
}

TEST(concurrency_double_buffer, Update_inactive_buffer) {
  auto &mock = *atomic<std::uint8_t>::get_instance();
  auto buffer = double_buffer<unaligned, atomic>{};

  testing::InSequence sequence{};
  EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
      .WillOnce(testing::Return(0U));
  EXPECT_CALL(mock, mock_store(1U, std::memory_order::release)).Times(1);
  EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
      .WillOnce(testing::Return(1U));
  buffer.write(unaligned{.value = 99UZ});
  const auto payload = buffer.read();
  const auto value = payload.value;
  EXPECT_EQ(99UZ, value);
  atomic<std::uint8_t>::instance.reset();
}