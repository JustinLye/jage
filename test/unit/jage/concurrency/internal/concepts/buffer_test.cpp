#include <jage/concurrency/double_buffer.hpp>

#include <jage/concurrency/internal/concepts/buffer.hpp>

#include <jage/test/fakes/concurrency/atomic.hpp>

#include <GUnit.h>

struct foo {};

using jage::concurrency::double_buffer;
using jage::concurrency::internal::concepts::buffer;
using jage::test::fakes::concurrency::atomic;

GTEST("internal buffer concept: accept double_buffer") {
  SHOULD("Accept double_buffer") {
    EXPECT_TRUE((buffer<double_buffer<foo, atomic>>));
  }
}

template <class...> struct missing_read {};
GTEST("internal buffer concept: reject missing read") {

  SHOULD("Reject type that does not have read method") {
    EXPECT_FALSE((buffer<missing_read<>>));
  }
}

template <class T, template <class> class> struct read_wrong_return_type {
  auto read() -> double;
};

GTEST("internal buffer concept: reject read that does not wrong return type") {

  SHOULD("Reject type with read method that does not return event type") {
    EXPECT_FALSE((buffer<read_wrong_return_type<foo, atomic>>));
  }
}

template <class T, template <class> class> struct missing_write {
  auto read() -> T;
};

GTEST("internal buffer concept: missing write method") {

  SHOULD("Reject type with missing write method") {
    EXPECT_FALSE((buffer<missing_write<foo, atomic>>));
  }
}
