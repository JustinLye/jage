#include <jage/scheduled_action.hpp>
#include <jage/scheduled_action_status.hpp>

#include <gtest/gtest.h>

using namespace std::chrono_literals;

class scheduled_action_status : public ::testing::Test {
protected:
  jage::scheduled_action<> scheduled_action;
};

class scheduled_action_with_time : public scheduled_action_status {
protected:
  void SetUp() override { scheduled_action = jage::scheduled_action{10ns}; }
};

TEST_F(scheduled_action_status, Be_active_on_construction) {
  EXPECT_EQ(jage::scheduled_action_status::active, scheduled_action.status());
}

TEST_F(scheduled_action_status, Be_paused_after_pause) {
  scheduled_action.pause();
  EXPECT_EQ(jage::scheduled_action_status::paused, scheduled_action.status());
}

TEST_F(scheduled_action_status, Be_active_after_resume) {
  scheduled_action.pause();
  scheduled_action.resume();
  EXPECT_EQ(jage::scheduled_action_status::active, scheduled_action.status());
}

TEST_F(scheduled_action_status, Be_canceled_after_cancel) {
  scheduled_action.cancel();
  EXPECT_EQ(jage::scheduled_action_status::canceled, scheduled_action.status());
}

TEST_F(scheduled_action_status, Not_resume_after_cancel) {
  scheduled_action.cancel();
  scheduled_action.resume();
  EXPECT_EQ(jage::scheduled_action_status::canceled, scheduled_action.status());
}

TEST_F(scheduled_action_status, Not_pause_after_cancel) {
  scheduled_action.cancel();
  scheduled_action.pause();
  EXPECT_EQ(jage::scheduled_action_status::canceled, scheduled_action.status());
}

TEST_F(scheduled_action_status, Complete_after_time_expires) {
  scheduled_action.update(1ns);
  EXPECT_EQ(jage::scheduled_action_status::complete, scheduled_action.status());
}

TEST_F(scheduled_action_status,
       Complete_after_0_ns_because_default_duration_is_0_ns) {
  scheduled_action.update(0ns);
  EXPECT_EQ(jage::scheduled_action_status::complete, scheduled_action.status());
}

TEST_F(scheduled_action_with_time, Stay_active_if_not_enough_time_has_elapsed) {
  scheduled_action.update(1ns);
  EXPECT_EQ(jage::scheduled_action_status::active, scheduled_action.status());
}

TEST_F(scheduled_action_with_time, Complete_after_enough_time_has_elapsed) {
  scheduled_action.update(1ns);
  scheduled_action.update(9ns);
  EXPECT_EQ(jage::scheduled_action_status::complete,
            scheduled_action.status());
}

TEST_F(scheduled_action_with_time,
       Remain_complete_after_cancel_post_time_elapsed) {
  scheduled_action.update(10ns);
  scheduled_action.cancel();
  EXPECT_EQ(jage::scheduled_action_status::complete,
            scheduled_action.status());
}

TEST_F(scheduled_action_with_time, Be_complete_after_time_expires) {
  scheduled_action.update(10ns);
  EXPECT_TRUE(scheduled_action.is_complete());
}

TEST_F(scheduled_action_with_time, Not_be_complete_before_time_expires) {
  scheduled_action.update(9ns);
  EXPECT_FALSE(scheduled_action.is_complete());
}

TEST_F(scheduled_action_with_time, Be_complete_after_cancel) {
  scheduled_action.cancel();
  EXPECT_TRUE(scheduled_action.is_complete());
}

TEST_F(scheduled_action_with_time, Not_be_complete_after_pause) {
  scheduled_action.pause();
  EXPECT_FALSE(scheduled_action.is_complete());
}

TEST_F(scheduled_action_with_time, Become_active_after_reset_while_paused) {
  scheduled_action.pause();
  scheduled_action.reset(10ns);
  EXPECT_EQ(jage::scheduled_action_status::active, scheduled_action.status());
}

TEST_F(scheduled_action_with_time, Become_active_after_reset_while_canceled) {
  scheduled_action.cancel();
  scheduled_action.reset(10ns);
  EXPECT_EQ(jage::scheduled_action_status::active, scheduled_action.status());
}

TEST_F(scheduled_action_with_time, Become_active_after_reset_while_complete) {
  scheduled_action.update(10ns);
  scheduled_action.reset(10ns);
  EXPECT_EQ(jage::scheduled_action_status::active, scheduled_action.status());
}

TEST_F(scheduled_action_with_time, Reset_elapsed_time_on_reset) {
  scheduled_action.update(10ns);
  scheduled_action.reset(10ns);
  scheduled_action.update(9ns);
  EXPECT_EQ(jage::scheduled_action_status::active, scheduled_action.status());
}

TEST_F(scheduled_action_with_time, Stay_active_after_extending_time) {
  scheduled_action.update(9ns);
  scheduled_action.extend(2ns);
  scheduled_action.update(1ns);
  EXPECT_EQ(jage::scheduled_action_status::active, scheduled_action.status());
}

TEST_F(scheduled_action_with_time, Complete_after_additional_time_elapses) {
  scheduled_action.update(9ns);
  scheduled_action.extend(2ns);
  scheduled_action.update(3ns);
  EXPECT_EQ(jage::scheduled_action_status::complete,
            scheduled_action.status());
}

TEST(scheduled_action_with_action, Not_execute_action_before_time_expires) {
  std::uint8_t value{0};
  auto scheduled_action = jage::scheduled_action{
      10ns,
      [&] { value = 42; },
  };
  scheduled_action.update(9ns);
  EXPECT_EQ(0, value);
}

TEST(scheduled_action_with_action, Execute_action_after_time_expires) {
  std::uint8_t value{0};
  auto scheduled_action = jage::scheduled_action{
      10ns,
      [&] { value = 42; },
  };

  scheduled_action.update(10ns);
  EXPECT_EQ(42, value);
}

TEST(scheduled_action_with_action, Not_invoke_the_action_again) {
  std::uint8_t value{0};
  auto scheduled_action = jage::scheduled_action{
      10ns,
      [&] { value = 42; },
  };

  scheduled_action.update(10ns);
  value = 0;
  scheduled_action.update(1ns);
  EXPECT_EQ(0, value);
}

TEST(scheduled_action_with_action, Invoke_the_action_again_after_reset) {
  std::uint8_t value{0};
  auto scheduled_action = jage::scheduled_action{
      10ns,
      [&] { value = 42; },
  };

  scheduled_action.update(10ns);
  value = 0;
  scheduled_action.reset(10ns);
  scheduled_action.update(9ns);
  EXPECT_EQ(0, value);
  scheduled_action.update(1ns);
  EXPECT_EQ(42, value);
}

TEST(scheduled_action_with_action, Not_update_time_remaining_when_paused) {
  std::uint8_t value{0};
  auto scheduled_action = jage::scheduled_action{
      10ns,
      [&] { value = 42; },
  };

  scheduled_action.pause();
  scheduled_action.update(20ns);
  EXPECT_EQ(0, value);
  scheduled_action.resume();
  scheduled_action.update(10ns);
  EXPECT_EQ(42, value);
}
