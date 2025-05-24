
#include <jage/test/fixtures/window_test.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using jage::test::fixtures::window_test;

TEST_F(window_test, should_check_driver_if_window_should_be_closed) {
  EXPECT_CALL(driver, should_close()).WillOnce(testing::Return(true));
  EXPECT_TRUE(window.should_close());
}

TEST_F(window_test,
       should_return_value_driver_returns_for_window_should_be_closed) {
  EXPECT_CALL(driver, should_close()).WillOnce(testing::Return(false));
  EXPECT_FALSE(window.should_close());
}

TEST_F(window_test, should_poll_driver) {
  EXPECT_CALL(driver, poll()).Times(1);
  window.poll();
}