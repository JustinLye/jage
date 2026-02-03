#include <jage/time/cache_match_status.hpp>
#include <jage/time/durations.hpp>
#include <jage/time/events/snapshot.hpp>

#include <jage/time/internal/snapshot_cache.hpp>

#include <jage/test/fakes/concurrency/atomic.hpp>
#include <jage/test/fakes/concurrency/double_buffer.hpp>
#include <jage/test/mocks/concurrency/atomic.hpp>

#include <GUnit.h>

#include <atomic>

using jage::time::durations::nanoseconds;
using jage::time::events::snapshot;
using jage::time::internal::snapshot_cache;
using jage::time::durations::operator""_ns;
using jage::time::cache_match_status;

namespace fakes {
using jage::test::fakes::concurrency::atomic;
using jage::test::fakes::concurrency::double_buffer;
} // namespace fakes

namespace mocks {
using jage::test::mocks::concurrency::atomic;
}

GTEST("snapshot: compile time queries") {

  SHOULD("have capacity query") {
    const auto cache = snapshot_cache<3UZ, snapshot<nanoseconds>,
                                      fakes::double_buffer, fakes::atomic>{};
    static_assert(3UZ == cache.capacity());
  }

  SHOULD("return capacity used to instantiate template") {
    const auto cache = snapshot_cache<121UZ, snapshot<nanoseconds>,
                                      fakes::double_buffer, fakes::atomic>{};
    static_assert(121UZ == cache.capacity());
  }
}

GTEST("snapshot: store and retrieve") {
  auto cache = snapshot_cache<3UZ, snapshot<nanoseconds>, fakes::double_buffer,
                              fakes::atomic>{};

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

  SHOULD("find snapshot, by timestamp, when multiple snapshots are in cache") {
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

  SHOULD(
      "find snapshot, by frame index, when multiple snapshots are in cache") {
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

  SHOULD("find snapshot, by timestamp, and return status of evicted") {
    const auto &[snap, status] = cache.find(99_ns);
    EXPECT_EQ(100_ns, snap.real_time);
    EXPECT_EQ(0, snap.frame);
    EXPECT_EQ(cache_match_status::evicted, status);
  }

  SHOULD("find snapshot, by frame index, and return status of ahead") {
    const auto &[snap, status] = cache.find(3);
    EXPECT_EQ(123_ns, snap.real_time);
    EXPECT_EQ(2, snap.frame);
    EXPECT_EQ(cache_match_status::ahead, status);
  }

  cache.push(snapshot<nanoseconds>{
      .real_time = 140_ns,
      .frame = 3,
  });

  SHOULD("find correct snapshot, by timestamp, after oldest snapshot has been "
         "dropped.") {
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

  SHOULD("find correct snapshot, frame index, after oldest snapshot has been "
         "dropped.") {
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
}

GTEST("snapshot: atomic operations") {
  using testing::Return;
  auto cache = snapshot_cache<3UZ, snapshot<nanoseconds>, fakes::double_buffer,
                              mocks::atomic>{};
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

  SHOULD("atomically load write index on find by timestamp") {
    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(3UZ));
    const auto &[snap, status] = cache.find(99_ns);
    EXPECT_EQ(99_ns, snap.real_time);
    EXPECT_EQ(0, snap.frame);
  }

  SHOULD("atomically load write index on find by index") {
    EXPECT_CALL(mock, mock_load(std::memory_order::acquire))
        .WillOnce(Return(3UZ));
    const auto &[snap, status] = cache.find(0);
    EXPECT_EQ(99_ns, snap.real_time);
    EXPECT_EQ(0, snap.frame);
  }
  mocks::atomic<std::uint64_t>::instance.reset();
}