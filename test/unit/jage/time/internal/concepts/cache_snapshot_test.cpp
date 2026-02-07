#include <jage/time/durations.hpp>
#include <jage/time/events/snapshot.hpp>

#include <jage/time/internal/concepts/cache_snapshot.hpp>

#include <gtest/gtest.h>

using jage::time::durations::seconds;
using jage::time::events::snapshot;

template <typename T>
concept snapshot_concept = jage::time::internal::concepts::cache_snapshot<T>;

TEST(cache_snapshot_concept, Accept_snapshot_with_real_number_duration) {
  EXPECT_TRUE(snapshot_concept<snapshot<seconds>>);
}

TEST(cache_snapshot_concept, Reject_type_without_real_time_member) {
  struct missing_real_time {
    std::uint64_t frame{};
  };
  static_assert(std::is_trivially_copyable_v<missing_real_time>);

  EXPECT_FALSE(snapshot_concept<missing_real_time>);
}

TEST(cache_snapshot_concept, Reject_when_real_time_is_not_a_duration_type) {
  struct wrong_real_time_type {
    int real_time{};
    std::uint64_t frame{};
  };
  static_assert(std::is_trivially_copyable_v<wrong_real_time_type>);

  EXPECT_FALSE(snapshot_concept<wrong_real_time_type>);
}

TEST(cache_snapshot_concept, Reject_type_without_frame_member) {
  struct missing_frame {
    seconds real_time{};
  };
  static_assert(std::is_trivially_copyable_v<missing_frame>);

  EXPECT_FALSE(snapshot_concept<missing_frame>);
}

TEST(cache_snapshot_concept, Reject_when_frame_is_not_integral) {
  struct wrong_frame_type {
    seconds real_time{};
    double frame{};
  };
  static_assert(std::is_trivially_copyable_v<wrong_frame_type>);

  EXPECT_FALSE(snapshot_concept<wrong_frame_type>);
}

TEST(cache_snapshot_concept, Reject_when_type_is_not_trivially_copyable) {
  struct not_trivially_copyable {
    seconds real_time{};
    std::uint64_t frame{};
    std::string data;
  };
  static_assert(not std::is_trivially_copyable_v<not_trivially_copyable>);

  EXPECT_FALSE(snapshot_concept<not_trivially_copyable>);
}

TEST(cache_snapshot_concept, Reject_when_duration_typedef_is_not_defined) {
  struct missing_duration_typedef {
    seconds real_time{};
    std::uint64_t frame{};
  };
  static_assert(std::is_trivially_copyable_v<missing_duration_typedef>);
  EXPECT_FALSE(snapshot_concept<missing_duration_typedef>);
}

TEST(cache_snapshot_concept,
     Reject_when_duration_typedef_does_not_match_real_time_type) {
  struct wrong_duration_duration_typedef {
    seconds real_time{};
    std::uint64_t frame{};
    using duration = jage::time::durations::nanoseconds;
  };
  static_assert(std::is_trivially_copyable_v<wrong_duration_duration_typedef>);
  EXPECT_FALSE(snapshot_concept<wrong_duration_duration_typedef>);
}
