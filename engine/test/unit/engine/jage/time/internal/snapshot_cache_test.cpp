#include <jage/time/cache_match_status.hpp>
#include <jage/time/durations.hpp>
#include <jage/time/events/snapshot.hpp>

#include <jage/time/internal/snapshot_cache.hpp>

#include <jage/test/fakes/concurrency/atomic.hpp>
#include <jage/test/fakes/concurrency/double_buffer.hpp>
#include <jage/test/mocks/concurrency/atomic.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <atomic>

using jage::time::cache_match_status;
using jage::time::durations::nanoseconds;
using jage::time::durations::operator""_ns;
using jage::time::events::snapshot;
using jage::time::internal::snapshot_cache;

namespace fakes {
using jage::test::fakes::concurrency::atomic;
using jage::test::fakes::concurrency::double_buffer;
} // namespace fakes

namespace mocks {
using jage::test::mocks::concurrency::atomic;
}

TEST(snapshot_compile_time_queries, Have_capacity_query) {
  const auto cache = snapshot_cache<3UZ, snapshot<nanoseconds>,
                                    fakes::double_buffer, fakes::atomic>{};
  static_assert(3UZ == cache.capacity());
}

TEST(snapshot_compile_time_queries,
     Return_capacity_used_to_instantiate_template) {
  const auto cache = snapshot_cache<121UZ, snapshot<nanoseconds>,
                                    fakes::double_buffer, fakes::atomic>{};
  static_assert(121UZ == cache.capacity());
}

class snapshot_store_and_retrieve : public ::testing::Test {
protected:
  snapshot_cache<3UZ, snapshot<nanoseconds>, fakes::double_buffer,
                 fakes::atomic>
      cache{};

  auto SetUp() -> void override {
    cache.push(snapshot<nanoseconds>{
        .real_time = 100_ns,
        .frame = 0,
    });
    cache.push(snapshot<nanoseconds>{
        .real_time = 110_ns,
        .frame = 1,
    });
    cache.push(snapshot<nanoseconds>{
        .real_time = 123_ns,
        .frame = 2,
    });
  }
};

TEST_F(snapshot_store_and_retrieve,
       Find_snapshot_by_timestamp_when_multiple_snapshots_are_in_cache) {
  {
    const auto &[snap, status] = cache.find(105_ns);
    EXPECT_EQ(100_ns, snap.real_time);
    EXPECT_EQ(0, snap.frame);
    EXPECT_EQ(cache_match_status::matched, status);
  }
  {
    const auto &[snap, status] = cache.find(112_ns);
    EXPECT_EQ(110_ns, snap.real_time);
    EXPECT_EQ(1, snap.frame);
    EXPECT_EQ(cache_match_status::matched, status);
  }
  {
    const auto &[snap, status] = cache.find(122_ns);
    EXPECT_EQ(110_ns, snap.real_time);
    EXPECT_EQ(1, snap.frame);
    EXPECT_EQ(cache_match_status::matched, status);
  }
  {
    const auto &[snap, status] = cache.find(124_ns);
    EXPECT_EQ(123_ns, snap.real_time);
    EXPECT_EQ(2, snap.frame);
    EXPECT_EQ(cache_match_status::matched, status);
  }
}

TEST_F(snapshot_store_and_retrieve,
       Find_snapshot_by_frame_index_when_multiple_snapshots_are_in_cache) {
  {
    const auto &[snap, status] = cache.find(0);
    EXPECT_EQ(100_ns, snap.real_time);
    EXPECT_EQ(0, snap.frame);
    EXPECT_EQ(cache_match_status::matched, status);
  }
  {
    const auto &[snap, status] = cache.find(1);
    EXPECT_EQ(110_ns, snap.real_time);
    EXPECT_EQ(1, snap.frame);
    EXPECT_EQ(cache_match_status::matched, status);
  }
  {
    const auto &[snap, status] = cache.find(2);
    EXPECT_EQ(123_ns, snap.real_time);
    EXPECT_EQ(2, snap.frame);
    EXPECT_EQ(cache_match_status::matched, status);
  }
}

TEST_F(snapshot_store_and_retrieve,
       Find_snapshot_by_timestamp_and_return_status_of_evicted) {
  const auto &[snap, status] = cache.find(99_ns);
  EXPECT_EQ(100_ns, snap.real_time);
  EXPECT_EQ(0, snap.frame);
  EXPECT_EQ(cache_match_status::evicted, status);
}

TEST_F(snapshot_store_and_retrieve,
       Find_snapshot_by_frame_index_and_return_status_of_ahead) {
  const auto &[snap, status] = cache.find(3);
  EXPECT_EQ(123_ns, snap.real_time);
  EXPECT_EQ(2, snap.frame);
  EXPECT_EQ(cache_match_status::ahead, status);
}

TEST_F(
    snapshot_store_and_retrieve,
    Find_correct_snapshot_by_timestamp_after_oldest_snapshot_has_been_dropped) {
  cache.push(snapshot<nanoseconds>{
      .real_time = 140_ns,
      .frame = 3,
  });
  {
    const auto &[snap, status] = cache.find(105_ns);
    EXPECT_EQ(110_ns, snap.real_time);
    EXPECT_EQ(1, snap.frame);
    EXPECT_EQ(cache_match_status::evicted, status);
  }
  {
    const auto &[snap, status] = cache.find(141_ns);
    EXPECT_EQ(140_ns, snap.real_time);
    EXPECT_EQ(3, snap.frame);
    EXPECT_EQ(cache_match_status::matched, status);
  }
}

TEST_F(
    snapshot_store_and_retrieve,
    Find_correct_snapshot_by_frame_index_after_oldest_snapshot_has_been_dropped) {
  cache.push(snapshot<nanoseconds>{
      .real_time = 140_ns,
      .frame = 3,
  });
  {
    const auto &[snap, status] = cache.find(0);
    EXPECT_EQ(110_ns, snap.real_time);
    EXPECT_EQ(1, snap.frame);
    EXPECT_EQ(cache_match_status::evicted, status);
  }
  {
    const auto &[snap, status] = cache.find(3);
    EXPECT_EQ(140_ns, snap.real_time);
    EXPECT_EQ(3, snap.frame);
    EXPECT_EQ(cache_match_status::matched, status);
  }
}

class snapshot_atomic_operations : public ::testing::Test {
protected:
  snapshot_cache<3UZ, snapshot<nanoseconds>, fakes::double_buffer,
                 mocks::atomic>
      cache{};

  auto SetUp() -> void override {
    using testing::Return;
    auto &mock = *mocks::atomic<std::uint64_t>::get_instance();

    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(0UZ))
        .WillOnce(Return(1UZ))
        .WillOnce(Return(2UZ));
    EXPECT_CALL(mock, mock_store(1UZ, std::memory_order::release)).Times(1);
    EXPECT_CALL(mock, mock_store(2UZ, std::memory_order::release)).Times(1);
    EXPECT_CALL(mock, mock_store(3UZ, std::memory_order::release)).Times(1);
    cache.push(snapshot<nanoseconds>{
        .real_time = 99_ns,
        .frame = 0,
    });
    cache.push(snapshot<nanoseconds>{
        .real_time = 101_ns,
        .frame = 1,
    });
    cache.push(snapshot<nanoseconds>{
        .real_time = 105_ns,
        .frame = 2,
    });
  }

  void TearDown() override { mocks::atomic<std::uint64_t>::instance.reset(); }
};

TEST_F(snapshot_atomic_operations,
       Atomically_load_write_index_on_find_by_timestamp) {
  using testing::Return;
  auto &mock = *mocks::atomic<std::uint64_t>::get_instance();
  EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
      .WillOnce(Return(3UZ));
  const auto &[snap, status] = cache.find(99_ns);
  EXPECT_EQ(99_ns, snap.real_time);
  EXPECT_EQ(0, snap.frame);
}

TEST_F(snapshot_atomic_operations,
       Atomically_load_write_index_on_find_by_index) {
  using testing::Return;
  auto &mock = *mocks::atomic<std::uint64_t>::get_instance();
  EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
      .WillOnce(Return(3UZ));
  const auto &[snap, status] = cache.find(0);
  EXPECT_EQ(99_ns, snap.real_time);
  EXPECT_EQ(0, snap.frame);
}
