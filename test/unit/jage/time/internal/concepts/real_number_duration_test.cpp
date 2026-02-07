#include <jage/time/internal/concepts/real_number_time_source.hpp>

#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>

TEST(time_internal_real_number_duration,
     Fit_concept_if_duration_representation_is_real_number) {
  EXPECT_TRUE((jage::time::internal::concepts::real_number_duration<
               std::chrono::duration<float, std::milli>>));
}

TEST(time_internal_real_number_duration,
     Not_fit_concept_if_duration_representation_is_not_a_real_number) {
  EXPECT_FALSE((jage::time::internal::concepts::real_number_duration<
                std::chrono::duration<std::uint64_t, std::milli>>));
}
