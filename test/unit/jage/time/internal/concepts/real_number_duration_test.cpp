#include <jage/time/internal/concepts/real_number_time_source.hpp>

#include <GUnit.h>

#include <chrono>
#include <cstdint>

GTEST("time internal real number duration") {

  SHOULD("fit concept if duration representation is real number") {
    EXPECT(jage::time::internal::concepts::real_number_duration<
           std::chrono::duration<float, std::milli>>);
  }

  SHOULD("not fit concept if duration representation is not a real "
         "number") {
    EXPECT(not jage::time::internal::concepts::real_number_duration<
           std::chrono::duration<std::uint64_t, std::milli>>);
  }
}