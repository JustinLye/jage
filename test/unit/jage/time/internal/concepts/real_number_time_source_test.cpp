#include <jage/time/internal/concepts/real_number_time_source.hpp>

#include <jage/test/fakes/time/source.hpp>

#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>

TEST(time_internal_real_number_time_source,
     Fit_concept_if_time_source_duration_representation_is_real_number) {
  EXPECT_TRUE((jage::time::internal::concepts::real_number_time_source<
               jage::test::fakes::time::source<
                   std::chrono::duration<double, std::nano>>>));
}

TEST(time_internal_real_number_time_source,
     Not_fit_concept_if_time_source_duration_representation_is_not_a_real_number) {
  EXPECT_FALSE((jage::time::internal::concepts::real_number_time_source<
                std::chrono::time_point<
                    std::chrono::steady_clock,
                    std::chrono::duration<std::uint64_t, std::milli>>>));
  EXPECT_FALSE((jage::time::internal::concepts::real_number_time_source<
                jage::test::fakes::time::source<
                    std::chrono::duration<std::uint32_t, std::nano>>>));
}
