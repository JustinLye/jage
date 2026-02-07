#include <jage/window.hpp>

#include <jage/test/fixtures/window_test.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using jage::test::fixtures::window_test;

TEST_F(window_test, Check_driver_if_window_should_be_closed) {
  EXPECT_CALL(driver, should_close()).WillOnce(testing::Return(true));
  EXPECT_TRUE(window.should_close());
}
TEST_F(window_test,
       Return_the_driver_value_when_checking_if_window_should_be_closed) {
  EXPECT_CALL(driver, should_close()).WillOnce(testing::Return(false));
  EXPECT_TRUE(not window.should_close());
}

TEST_F(window_test, Poll_the_driver) {
  EXPECT_CALL(driver, poll()).Times(1);
  window.poll();
}

TEST_F(window_test, Call_render_on_the_driver) {
  EXPECT_CALL(driver, render()).Times(1);
  window.render();
}
