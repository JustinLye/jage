
#include <jage/test/fixtures/window_test.hpp>

#include <GUnit.h>
#include <gmock/gmock.h>

using jage::test::fixtures::window_test;

GTEST(window_test) {
  SHOULD("Check driver if window should be closed") {
    EXPECT_CALL(driver, should_close()).WillOnce(testing::Return(true));
    EXPECT(window.should_close());
  }

  SHOULD("Return the driver value when checking if window should be closed") {
    EXPECT_CALL(driver, should_close()).WillOnce(testing::Return(false));
    EXPECT(not window.should_close());
  }

  SHOULD("Poll the driver") {
    EXPECT_CALL(driver, poll()).Times(1);
    window.poll();
  }

  SHOULD("Call render on the driver") {
    EXPECT_CALL(driver, render()).Times(1);
    window.render();
  }
}
