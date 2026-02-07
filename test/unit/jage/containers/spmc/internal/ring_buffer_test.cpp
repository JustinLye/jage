#include <jage/containers/spmc/internal/ring_buffer.hpp>

#include <jage/test/fakes/concurrency/atomic.hpp>
#include <jage/test/fakes/concurrency/double_buffer.hpp>
#include <jage/test/mocks/concurrency/atomic.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <utility>

using jage::containers::spmc::internal::ring_buffer;

namespace mocks {
using jage::test::mocks::concurrency::atomic;
}

namespace fakes {
using jage::test::fakes::concurrency::atomic;
using jage::test::fakes::concurrency::double_buffer;
} // namespace fakes

struct foo {
  std::uint32_t value;
};

using testing::Return;

TEST(spmc_internal_ring_buffer, Provide_capacity_access) {
  EXPECT_EQ(10, (ring_buffer<foo, 10, fakes::atomic,
                             fakes::double_buffer>::capacity()));
  EXPECT_EQ(100, (ring_buffer<foo, 100, fakes::atomic,
                              fakes::double_buffer>::capacity()));
}

TEST(spmc_internal_ring_buffer, Default_write_head_to_0) {
  auto buffer = ring_buffer<foo, 3, fakes::atomic, fakes::double_buffer>{};
  EXPECT_EQ(0UZ, buffer.write_head());
}

TEST(spmc_internal_ring_buffer, Increment_write_head_after_push) {
  auto buffer = ring_buffer<foo, 3, fakes::atomic, fakes::double_buffer>{};
  buffer.push(foo{});
  EXPECT_EQ(1UZ, buffer.write_head());
}

TEST(spmc_internal_ring_buffer,
     Continue_incrementing_after_capacity_is_reached) {
  auto buffer = ring_buffer<foo, 2, fakes::atomic, fakes::double_buffer>{};
  buffer.push(foo{});
  buffer.push(foo{});
  buffer.push(foo{});
  EXPECT_EQ(3UZ, buffer.write_head());
}

TEST(spmc_internal_ring_buffer, Return_element_by_index) {
  auto buffer = ring_buffer<foo, 2, fakes::atomic, fakes::double_buffer>{};
  buffer.push(foo{
      .value = 42,
  });
  EXPECT_EQ(42, buffer.read(0).value);
  buffer.push(foo{
      .value = 99,
  });
  EXPECT_EQ(99, buffer.read(1).value);
  EXPECT_EQ(42, buffer.read(0).value);
}

TEST(spmc_internal_ring_buffer, Evict_oldest) {
  auto buffer = ring_buffer<foo, 2, fakes::atomic, fakes::double_buffer>{};
  buffer.push(foo{
      .value = 10,
  });
  buffer.push(foo{
      .value = 20,
  });
  EXPECT_EQ(10, buffer.read(0).value);
  EXPECT_EQ(20, buffer.read(1).value);
  buffer.push(foo{
      .value = 30,
  });
  EXPECT_EQ(30, buffer.read(0).value);
  EXPECT_EQ(20, buffer.read(1).value);
}

TEST(spmc_internal_ring_buffer, Access_write_head_atomically) {
  auto buffer = ring_buffer<foo, 2, mocks::atomic, fakes::double_buffer>{};
  auto &mock = *mocks::atomic<std::size_t>::get_instance();
  EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
      .WillOnce(Return(0UZ));
  std::ignore = buffer.write_head();
  mocks::atomic<std::size_t>::instance.reset();
}

TEST(spmc_internal_ring_buffer, Push_atomically) {
  auto buffer = ring_buffer<foo, 2, mocks::atomic, fakes::double_buffer>{};
  auto &mock = *mocks::atomic<std::size_t>::get_instance();
  EXPECT_CALL(mock, mock_load(std::memory_order::relaxed))
      .WillOnce(Return(0UZ));
  EXPECT_CALL(mock, mock_store(1UZ, std::memory_order::release)).Times(1);
  buffer.push(foo{});
  mocks::atomic<std::size_t>::instance.reset();
}

#if defined(JAGE_ENABLE_SANITY_CHECKS) and JAGE_ENABLE_SANITY_CHECKS == 1
TEST(spmc_internal_ring_buffer,
     Throw_exception_if_read_attempts_to_read_an_out_of_bounds_index) {
  auto buffer = ring_buffer<foo, 2, fakes::atomic, fakes::double_buffer>{};
  EXPECT_THROW(std::ignore = buffer.read(99), std::invalid_argument);
}
#endif
