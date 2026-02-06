#include <jage/containers/spmc/internal/ring_buffer.hpp>

#include <jage/test/fakes/concurrency/atomic.hpp>
#include <jage/test/fakes/concurrency/double_buffer.hpp>
#include <jage/test/mocks/concurrency/atomic.hpp>

#include <GUnit.h>

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

GTEST("spmc internal ring buffer") {
  SHOULD("Provide capacity access") {
    EXPECT_EQ(10, (ring_buffer<foo, 10, fakes::atomic,
                               fakes::double_buffer>::capacity()));
    EXPECT_EQ(100, (ring_buffer<foo, 100, fakes::atomic,
                                fakes::double_buffer>::capacity()));
  }

  SHOULD("Default write_head to 0") {
    auto buffer = ring_buffer<foo, 3, fakes::atomic, fakes::double_buffer>{};
    EXPECT_EQ(0UZ, buffer.write_head());
  }
  SHOULD("Increment write_head after push") {
    auto buffer = ring_buffer<foo, 3, fakes::atomic, fakes::double_buffer>{};
    buffer.push(foo{});
    EXPECT_EQ(1UZ, buffer.write_head());
  }

  SHOULD("Continue incrementing after capacity is reached") {
    auto buffer = ring_buffer<foo, 2, fakes::atomic, fakes::double_buffer>{};
    buffer.push(foo{});
    buffer.push(foo{});
    buffer.push(foo{});
    EXPECT_EQ(3UZ, buffer.write_head());
  }

  SHOULD("Return element by index") {
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

  SHOULD("Evict oldest") {
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

  SHOULD("Access write_head atomically") {
    auto buffer = ring_buffer<foo, 2, mocks::atomic, fakes::double_buffer>{};
    auto &mock = *mocks::atomic<std::size_t>::get_instance();
    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(0UZ));
    std::ignore = buffer.write_head();
  }

  SHOULD("Push atomically") {
    auto buffer = ring_buffer<foo, 2, mocks::atomic, fakes::double_buffer>{};
    auto &mock = *mocks::atomic<std::size_t>::get_instance();
    EXPECT_CALL(mock, mock_load(std::memory_order::relaxed))
        .WillOnce(Return(0UZ));
    EXPECT_CALL(mock, mock_store(1UZ, std::memory_order::release)).Times(1);
    buffer.push(foo{});
  }

#if defined(JAGE_ENABLE_SANITY_CHECKS) and JAGE_ENABLE_SANITY_CHECKS == 1
  SHOULD("Throw exception if read attempts to read an out-of-bounds index") {
    auto buffer = ring_buffer<foo, 2, fakes::atomic, fakes::double_buffer>{};
    EXPECT_THROW(std::ignore = buffer.read(99), std::invalid_argument);
  }
#endif
  mocks::atomic<std::size_t>::instance.reset();
}