#include <jage/time/internal/concepts/real_number_time_source.hpp>

#include <jage/test/fakes/time/source.hpp>

#include <GUnit.h>

#include <chrono>
#include <cstdint>

GTEST("time internal real number time source") {

  SHOULD("fit concept if time source duration representation is real number") {
    EXPECT(jage::time::internal::concepts::real_number_time_source<
           jage::test::fakes::time::source<
               std::chrono::duration<double, std::nano>>>);
  }

  SHOULD("not fit concept if time source duration representation is not a real "
         "number") {
    EXPECT(not jage::time::internal::concepts::real_number_time_source<
           std::chrono::time_point<
               std::chrono::steady_clock,
               std::chrono::duration<std::uint64_t, std::milli>>>);
    EXPECT(not jage::time::internal::concepts::real_number_time_source<
           jage::test::fakes::time::source<
               std::chrono::duration<std::uint32_t, std::nano>>>);
  }
}