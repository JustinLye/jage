#include <jage/containers/spsc/queue.hpp>

#include <jage/test/fakes/concurrency/atomic.hpp>
#include <jage/test/mocks/concurrency/atomic.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

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

TEST(queue_initialization, Return_capacity) {
  auto sut = queue<foo, 10UZ>{};
  static_assert(10UZ == sut.capacity());
  auto alt_sut = queue<foo, 100UZ>{};
  static_assert(100UZ == alt_sut.capacity());
}

TEST(queue_initialization, Have_value_type) {
  EXPECT_TRUE((std::same_as<queue<foo, 10UZ>::value_type, foo>));
  EXPECT_TRUE(
      (std::same_as<queue<std::int32_t, 10UZ>::value_type, std::int32_t>));
}

TEST(queue_happy_path, Push_and_pop_operations) {
  using jage::test::fakes::concurrency::atomic;
  auto sut = queue<foo, 3UZ, atomic>{};

  ASSERT_TRUE(std::empty(sut));
  ASSERT_EQ(0, std::size(sut));

  sut.push(foo{
      .value = 42,
  });

  EXPECT_EQ(42, sut.front().value);
  EXPECT_FALSE(std::empty(sut));
  EXPECT_EQ(1UZ, std::size(sut));

  sut.push(foo{
      .value = 101,
  });

  EXPECT_EQ(42, sut.front().value);
  EXPECT_FALSE(std::empty(sut));
  EXPECT_EQ(2UZ, std::size(sut));

  sut.push(foo{
      .value = 202,
  });

  EXPECT_EQ(42, sut.front().value);
  EXPECT_FALSE(std::empty(sut));
  EXPECT_EQ(3UZ, std::size(sut));

  sut.pop();

  EXPECT_EQ(101, sut.front().value);
  EXPECT_FALSE(std::empty(sut));
  EXPECT_EQ(2UZ, std::size(sut));

  sut.pop();

  EXPECT_EQ(202, sut.front().value);
  EXPECT_FALSE(std::empty(sut));
  EXPECT_EQ(1UZ, std::size(sut));

  sut.pop();

  EXPECT_TRUE(std::empty(sut));
  EXPECT_EQ(0UZ, std::size(sut));
}

TEST(queue_empty_queue,
     Return_default_constructed_element_when_front_is_called_on_empty_queue) {
  using jage::test::fakes::concurrency::atomic;
  auto sut = queue<bar, 3UZ, atomic>{};
  EXPECT_EQ(bar{}.value, sut.front().value);
}

TEST(queue_empty_queue, Not_move_head_after_pop_on_empty_queue) {
  using jage::test::fakes::concurrency::atomic;
  auto sut = queue<bar, 3UZ, atomic>{};
  ASSERT_TRUE(std::empty(sut));
  sut.pop();
  EXPECT_TRUE(std::empty(sut));
  EXPECT_EQ(0UZ, std::size(sut));
}

TEST(queue_full_queue, Roll_over_operations) {
  using jage::test::fakes::concurrency::atomic;
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

  EXPECT_FALSE(std::empty(sut));
  EXPECT_EQ(1UZ, std::size(sut));
  EXPECT_EQ(4UZ, sut.front().value);

  sut.push(foo{
      .value = 5,
  });

  EXPECT_FALSE(std::empty(sut));
  EXPECT_EQ(2UZ, std::size(sut));
  EXPECT_EQ(4UZ, sut.front().value);

  sut.push(foo{
      .value = 6,
  });

  EXPECT_FALSE(std::empty(sut));
  EXPECT_EQ(3UZ, std::size(sut));
  EXPECT_EQ(4UZ, sut.front().value);

  sut.pop();

  EXPECT_FALSE(std::empty(sut));
  EXPECT_EQ(2UZ, std::size(sut));
  EXPECT_EQ(5UZ, sut.front().value);

  sut.pop();

  EXPECT_FALSE(std::empty(sut));
  EXPECT_EQ(1UZ, std::size(sut));
  EXPECT_EQ(6UZ, sut.front().value);

  sut.pop();

  EXPECT_TRUE(std::empty(sut));
  EXPECT_EQ(0UZ, std::size(sut));
}

TEST(queue_roll_over, Head_index_can_be_exchanged_on_first_try) {
  using jage::test::fakes::concurrency::atomic;
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

  EXPECT_FALSE(std::empty(sut));
  EXPECT_EQ(3UZ, std::size(sut));

  EXPECT_EQ(2, sut.front().value);
  sut.pop();
  EXPECT_EQ(3, sut.front().value);
  sut.pop();
  EXPECT_EQ(4, sut.front().value);
  sut.pop();
  EXPECT_TRUE(std::empty(sut));
  EXPECT_EQ(0UZ, std::size(sut));
}

TEST(queue_size_limits, Never_exceeds_capacity_with_racy_load_order) {
  using ::testing::InSequence;
  using ::testing::Return;

  InSequence in_seq{};

  auto &mock =
      *jage::test::mocks::concurrency::atomic<std::uint64_t>::get_instance();
  auto sut = queue<foo, 3UZ, jage::test::mocks::concurrency::atomic>{};

  EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
      .WillOnce(Return(0UZ));
  EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
      .WillOnce(Return(5UZ));
  EXPECT_LE(std::size(sut), sut.capacity());

  jage::test::mocks::concurrency::atomic<std::uint64_t>::instance.reset();
}

TEST(queue_roll_over_corner_cases, Compare_and_swap_loop_edge_cases) {
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

  {
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

  {
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

  {
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
