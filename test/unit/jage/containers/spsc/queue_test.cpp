#include <jage/containers/spsc/queue.hpp>

#include <jage/test/fakes/concurrency/atomic.hpp>
#include <jage/test/mocks/concurrency/atomic.hpp>

#include <GUnit.h>

#include <atomic>
#include <concepts>
#include <cstdint>

struct foo {
  std::uint32_t value{};
};

struct bar {
  std::uint64_t value{42};
};

using jage::containers::spsc::queue;

GTEST("queue: initialization") {
  auto sut = queue<foo, 10UZ>{};
  SHOULD("return capacity") {
    static_assert(10UZ == sut.capacity());
    auto alt_sut = queue<foo, 100UZ>{};
    static_assert(100UZ == alt_sut.capacity());
  }
  SHOULD("have value_type") {
    EXPECT_TRUE((std::same_as<queue<foo, 10UZ>::value_type, foo>));
    EXPECT_TRUE(
        (std::same_as<queue<std::int32_t, 10UZ>::value_type, std::int32_t>));
  }
}

template <class T>
using atomic =
    jage::test::fakes::concurrency::atomic<T, std::memory_order::acquire,
                                           std::memory_order::release>;

GTEST("queue: happy path") {
  auto sut = queue<foo, 3UZ, atomic>{};

  ASSERT_TRUE(std::empty(sut));
  ASSERT_EQ(0, std::size(sut));

  sut.push(foo{
      .value = 42,
  });

  SHOULD("put first value in the front of the queue") {
    EXPECT_EQ(42, sut.front().value);
  }

  SHOULD("not be empty after queuing an item") {
    EXPECT_FALSE(std::empty(sut));
  }

  SHOULD("have size of one after queuing an item") {
    EXPECT_EQ(1UZ, std::size(sut));
  }

  sut.push(foo{
      .value = 101,
  });

  SHOULD("have same front value after pushing another item") {
    EXPECT_EQ(42, sut.front().value);
  }

  SHOULD("not be empty after queuing a second item") {
    EXPECT_FALSE(std::empty(sut));
  }

  SHOULD("have size of two after adding second item") {
    EXPECT_EQ(2UZ, std::size(sut));
  }

  sut.push(foo{
      .value = 202,
  });

  SHOULD("have same front value after pushing another item") {
    EXPECT_EQ(42, sut.front().value);
  }

  SHOULD("not be empty after queuing a second item") {
    EXPECT_FALSE(std::empty(sut));
  }

  SHOULD("have size of two after adding second item") {
    EXPECT_EQ(3UZ, std::size(sut));
  }

  sut.pop();

  SHOULD("update front value after popping an item") {
    EXPECT_EQ(101, sut.front().value);
  }

  SHOULD("not be empty after pop with item still in queue") {
    EXPECT_FALSE(std::empty(sut));
  }

  SHOULD("have correct size after popping item from queue") {
    EXPECT_EQ(2UZ, std::size(sut));
  }

  sut.pop();

  SHOULD("update front value after popping an item") {
    EXPECT_EQ(202, sut.front().value);
  }

  SHOULD("not be empty after pop with item still in queue") {
    EXPECT_FALSE(std::empty(sut));
  }

  SHOULD("have correct size after popping item from queue") {
    EXPECT_EQ(1UZ, std::size(sut));
  }

  sut.pop();

  SHOULD("be empty after popping last item from the queue") {
    EXPECT_TRUE(std::empty(sut));
  }

  SHOULD("have size of zero after popping last item from the queue") {
    EXPECT_EQ(0UZ, std::size(sut));
  }
}

GTEST("queue: empty queue") {
  auto sut = queue<bar, 3UZ, atomic>{};

  SHOULD("return default constructed element when front is called on empty "
         "queue") {
    EXPECT_EQ(bar{}.value, sut.front().value);
  }

  ASSERT_TRUE(std::empty(sut));
  SHOULD("not move head after pop on empty queue") {
    sut.pop();
    EXPECT_TRUE(std::empty(sut));
    EXPECT_EQ(0UZ, std::size(sut));
  }
}

GTEST("queue: full queue") {
  auto sut = queue<foo, 3UZ, atomic>{};
  sut.push(foo{
      .value = 1,
  });
  sut.push(foo{.value = 2});
  sut.push(foo{.value = 3});
  sut.pop();
  sut.pop();
  sut.pop();
  ASSERT_TRUE(std::empty(sut));
  ASSERT_EQ(0UZ, std::size(sut));

  sut.push(foo{.value = 4});

  SHOULD("roll over after filling and emptying queue") {
    EXPECT_FALSE(std::empty(sut));
    EXPECT_EQ(1UZ, std::size(sut));
    EXPECT_EQ(4UZ, sut.front().value);
  }

  sut.push(foo{
      .value = 5,
  });
  SHOULD("roll over after filling, emptying, and adding an single item to "
         "the queue") {
    EXPECT_FALSE(std::empty(sut));
    EXPECT_EQ(2UZ, std::size(sut));
    EXPECT_EQ(4UZ, sut.front().value);
  }

  sut.push(foo{
      .value = 6,
  });

  SHOULD("roll over after filling, emptying, and adding multiple items to the "
         "queue") {
    EXPECT_FALSE(std::empty(sut));
    EXPECT_EQ(3UZ, std::size(sut));
    EXPECT_EQ(4UZ, sut.front().value);
  }

  sut.pop();
  SHOULD("move to next value in queue after pop") {
    EXPECT_FALSE(std::empty(sut));
    EXPECT_EQ(2UZ, std::size(sut));
    EXPECT_EQ(5UZ, sut.front().value);
  }

  sut.pop();
  SHOULD("move to next value in queue after pop") {
    EXPECT_FALSE(std::empty(sut));
    EXPECT_EQ(1UZ, std::size(sut));
    EXPECT_EQ(6UZ, sut.front().value);
  }
  sut.pop();
  SHOULD("move to next value in queue after pop") {
    EXPECT_TRUE(std::empty(sut));
    EXPECT_EQ(0UZ, std::size(sut));
  }
}

GTEST("queue: roll over and head index can be exchange on first-try") {
  auto sut = queue<foo, 3UZ, atomic>{};
  sut.push(foo{
      .value = 1,
  });
  sut.push(foo{
      .value = 2,
  });
  sut.push(foo{
      .value = 3,
  });
  ASSERT_FALSE(std::empty(sut));
  ASSERT_EQ(3UZ, std::size(sut));
  ASSERT_EQ(1, sut.front().value);

  sut.push(foo{
      .value = 4,
  });

  SHOULD("still be non-empty had have correct size") {
    EXPECT_FALSE(std::empty(sut));
    EXPECT_EQ(3UZ, std::size(sut));
  }

  SHOULD("overwrite oldest") {
    EXPECT_EQ(2, sut.front().value);
    sut.pop();
    EXPECT_EQ(3, sut.front().value);
    sut.pop();
    EXPECT_EQ(4, sut.front().value);
    sut.pop();
    EXPECT_TRUE(std::empty(sut));
    EXPECT_EQ(0UZ, std::size(sut));
  }
}

GTEST("queue: size never exceeds capacity (racy load order)") {
  using ::testing::InSequence;
  using ::testing::Return;

  InSequence in_seq{};

  auto &mock =
      *jage::test::mocks::concurrency::atomic<std::uint64_t>::get_instance();
  auto sut = queue<foo, 3UZ, jage::test::mocks::concurrency::atomic>{};

  SHOULD("not report size larger than capacity") {
    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(0UZ));
    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(5UZ));
    EXPECT_LE(std::size(sut), sut.capacity());
  }

  jage::test::mocks::concurrency::atomic<std::uint64_t>::instance.reset();
}

GTEST("queue: roll-over corner cases") {
  using ::testing::DoAll;
  using ::testing::Eq;
  using ::testing::Return;
  using ::testing::SetArgReferee;

  ::testing::InSequence in_seq{};

  auto &mock =
      *jage::test::mocks::concurrency::atomic<std::uint64_t>::get_instance();
  auto sut = queue<foo, 3UZ, jage::test::mocks::concurrency::atomic>{};

  EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
      .WillOnce(Return(0UZ))
      .WillOnce(Return(0UZ));
  EXPECT_CALL(mock, mock_store(1UZ, std::memory_order::release)).Times(1);
  EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
      .WillOnce(Return(1UZ))
      .WillOnce(Return(0UZ));
  EXPECT_CALL(mock, mock_store(2UZ, std::memory_order::release)).Times(1);
  EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
      .WillOnce(Return(2UZ))
      .WillOnce(Return(0UZ));
  EXPECT_CALL(mock, mock_store(3UZ, std::memory_order::release)).Times(1);
  sut.push(foo{
      .value = 10,
  });
  sut.push(foo{
      .value = 20,
  });
  sut.push(foo{
      .value = 30,
  });

  SHOULD("Stop compare-and-stop loop when exchange fails, but loaded head "
         "value is now equal to the desired value (single pop occurred)") {
    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(3UZ));
    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(0UZ));
    std::uint64_t expected_after = 1UZ;
    EXPECT_CALL(mock, mock_compare_exchange_weak(Eq(0UZ), expected_after,
                                                 std::memory_order::release,
                                                 std::memory_order::acquire))
        .WillOnce(DoAll(SetArgReferee<0>(expected_after), Return(false)));
    EXPECT_CALL(mock, mock_store(4UZ, std::memory_order::release)).Times(1);
    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(expected_after));
    sut.push(foo{
        .value = 40,
    });
    EXPECT_EQ(20, sut.front().value);
  }

  SHOULD(
      "Stop compare-and-stop loop when exchange fails, but loaded head "
      "value is now greater than equal the desired value (two pops occurred)") {
    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(3UZ));
    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(0UZ));
    std::uint64_t expected_after = 2UZ;
    EXPECT_CALL(mock, mock_compare_exchange_weak(Eq(0UZ), 1UZ,
                                                 std::memory_order::release,
                                                 std::memory_order::acquire))
        .WillOnce(DoAll(SetArgReferee<0>(expected_after), Return(false)));
    EXPECT_CALL(mock, mock_store(4UZ, std::memory_order::release)).Times(1);
    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(expected_after));

    sut.push(foo{
        .value = 40,
    });
    EXPECT_EQ(30, sut.front().value);
  }

  SHOULD("Stop compare-and-stop loop when exchange fails, but the queue has "
         "been cleared from underneath us") {
    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(3UZ));
    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(0UZ));
    std::uint64_t expected_after = 3UZ;
    EXPECT_CALL(mock, mock_compare_exchange_weak(Eq(0UZ), 1UZ,
                                                 std::memory_order::release,
                                                 std::memory_order::acquire))
        .WillOnce(DoAll(SetArgReferee<0>(expected_after), Return(false)));
    EXPECT_CALL(mock, mock_store(4UZ, std::memory_order::release)).Times(1);
    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(expected_after));

    sut.push(foo{
        .value = 40,
    });
    EXPECT_EQ(40, sut.front().value);
  }

  jage::test::mocks::concurrency::atomic<std::uint64_t>::instance.reset();
}
