#include <jage/time/durations.hpp>
#include <jage/time/events/snapshot.hpp>

#include <jage/time/internal/concepts/cache_snapshot.hpp>

#include <GUnit.h>

using jage::time::durations::seconds;
using jage::time::events::snapshot;

template <typename T>
concept snapshot_concept = jage::time::internal::concepts::cache_snapshot<T>;

GTEST("cache snapshot concept: accepts events::snapshot") {
  SHOULD("accept snapshot with real_number_duration") {
    EXPECT_TRUE(snapshot_concept<snapshot<seconds>>);
  }
}

GTEST("cache snapshot concept: rejects missing real_time") {
  struct missing_real_time {
    std::uint64_t frame{};
  };
  static_assert(std::is_trivially_copyable_v<missing_real_time>);

  SHOULD("reject type without real_time member") {
    EXPECT_FALSE(snapshot_concept<missing_real_time>);
  }
}

GTEST("cache snapshot concept: rejects wrong real_time type") {
  struct wrong_real_time_type {
    int real_time{};
    std::uint64_t frame{};
  };
  static_assert(std::is_trivially_copyable_v<wrong_real_time_type>);

  SHOULD("reject when real_time is not a duration type") {
    EXPECT_FALSE(snapshot_concept<wrong_real_time_type>);
  }
}

GTEST("cache snapshot concept: rejects missing frame") {
  struct missing_frame {
    seconds real_time{};
  };
  static_assert(std::is_trivially_copyable_v<missing_frame>);

  SHOULD("reject type without frame member") {
    EXPECT_FALSE(snapshot_concept<missing_frame>);
  }
}

GTEST("cache snapshot concept: rejects wrong frame type") {
  struct wrong_frame_type {
    seconds real_time{};
    double frame{};
  };
  static_assert(std::is_trivially_copyable_v<wrong_frame_type>);

  SHOULD("reject when frame is not integral") {
    EXPECT_FALSE(snapshot_concept<wrong_frame_type>);
  }
}

GTEST("cache snapshot concept: rejects non-trivially-copyable") {
  struct not_trivially_copyable {
    seconds real_time{};
    std::uint64_t frame{};
    std::string data;
  };
  static_assert(not std::is_trivially_copyable_v<not_trivially_copyable>);

  SHOULD("reject when type is not trivially copyable") {
    EXPECT_FALSE(snapshot_concept<not_trivially_copyable>);
  }
}

GTEST("cache snapshot concept: rejects missing duration typedef") {
  struct missing_duration_typedef {
    seconds real_time{};
    std::uint64_t frame{};
  };
  static_assert(std::is_trivially_copyable_v<missing_duration_typedef>);
  SHOULD("reject when duration typedef is not defined") {
    EXPECT_FALSE(snapshot_concept<missing_duration_typedef>);
  }
}

GTEST("cache snapshot concept: rejects wrong duration duration type") {
  struct wrong_duration_duration_typedef {
    seconds real_time{};
    std::uint64_t frame{};
    using duration = jage::time::durations::nanoseconds;
  };
  static_assert(std::is_trivially_copyable_v<wrong_duration_duration_typedef>);
  SHOULD("reject when duration typedef does not match real_time type") {
    EXPECT_FALSE(snapshot_concept<wrong_duration_duration_typedef>);
  }
}