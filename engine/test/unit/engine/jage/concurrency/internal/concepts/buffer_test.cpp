#include <jage/engine/concurrency/double_buffer.hpp>
#include <jage/engine/test/fakes/concurrency/atomic.hpp>

#include <jage/engine/concurrency/internal/concepts/buffer.hpp>

#include <gtest/gtest.h>

struct foo {};

using jage::engine::concurrency::double_buffer;
using jage::engine::concurrency::internal::concepts::buffer;
using jage::engine::test::fakes::concurrency::atomic;

TEST(internal_buffer_concept, Accept_double_buffer) {
  EXPECT_TRUE((buffer<double_buffer<foo, atomic>>));
}

template <class...> struct missing_read {};

TEST(internal_buffer_concept, Reject_type_that_does_not_have_read_method) {
  EXPECT_FALSE((buffer<missing_read<>>));
}

template <class T, template <class> class> struct read_wrong_return_type {
  auto read() -> double;
};

TEST(internal_buffer_concept,
     Reject_type_with_read_method_that_does_not_return_event_type) {
  EXPECT_FALSE((buffer<read_wrong_return_type<foo, atomic>>));
}

template <class T, template <class> class> struct missing_write {
  auto read() -> T;
};

TEST(internal_buffer_concept, Reject_type_with_missing_write_method) {
  EXPECT_FALSE((buffer<missing_write<foo, atomic>>));
}
