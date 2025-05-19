#include <jage/input/button/states.hpp>
#include <jage/input/button/status.hpp>
#include <jage/input/keyboard/keys.hpp>
#include <jage/input/keyboard/monitor.hpp>
#include <jage/input/mouse/buttons.hpp>
#include <jage/input/mouse/monitor.hpp>

#include <jage/test/fixtures/input/button/monitoring.hpp>
#include <jage/test/mocks/input/button/driver.hpp>

#include <gtest/gtest.h>

#include <stdexcept>

using namespace testing;
using keyboard_monitor_test_params = std::pair<
    jage::input::keyboard::monitor<
        jage::test::mocks::input::button::driver<jage::input::keyboard::keys>>,
    jage::input::keyboard::keys>;

using mouse_monitor_test_params = std::pair<
    jage::input::mouse::monitor<
        jage::test::mocks::input::button::driver<jage::input::mouse::buttons>>,
    jage::input::mouse::buttons>;
using test_types =
    Types<keyboard_monitor_test_params, mouse_monitor_test_params>;
template <class TPair>
struct monitoring
    : public jage::test::fixtures::input::button::monitoring<
          typename TPair::first_type, typename TPair::second_type> {};

TYPED_TEST_SUITE(monitoring, test_types);

TYPED_TEST(monitoring, no_query_if_no_callbacks) {
  this->expect_call_to_is_down(this->button0, 0U);
  this->monitor.monitor_input(this->button0);
  this->monitor.poll();
}

TYPED_TEST(monitoring, should_query_all_monitored_keys) {
  this->expect_call_to_is_down(this->button0);
  this->expect_call_to_is_down(this->button1);

  this->monitor.monitor_input(this->button0);
  this->monitor.monitor_input(this->button1);
  std::ignore = this->monitor.register_callback(this->null_callback);
  this->monitor.poll();
}

TYPED_TEST(monitoring, should_allow_variadic_key_input) {
  this->expect_call_to_is_down(this->button0);
  this->expect_call_to_is_down(this->button1);
  this->expect_call_to_is_down(this->button2);

  this->monitor.monitor_input(this->button0, this->button2, this->button1);
  std::ignore = this->monitor.register_callback(this->null_callback);
  this->monitor.poll();
}

TYPED_TEST(monitoring, should_allow_range_input) {
  this->expect_call_to_is_down(this->button0);
  this->expect_call_to_is_down(this->button1);
  this->expect_call_to_is_down(this->button2);

  this->monitor.monitor_input(std::array{
      this->button0,
      this->button2,
      this->button1,
  });
  std::ignore = this->monitor.register_callback(this->null_callback);
  this->monitor.poll();
}

TYPED_TEST(monitoring, should_invoke_registered_callback) {
  this->expect_call_to_is_down(this->button0);
  this->expect_call_to_callback();

  this->monitor.monitor_input(this->button0);
  std::ignore = this->monitor.register_callback(
      [&](const auto &state) -> void { this->mock_callback.call(state); });
  this->monitor.poll();
}

TYPED_TEST(monitoring, should_report_correct_status_of_monitored_key) {

  this->expect_call_to_is_down(this->button0);

  this->monitor.monitor_input(this->button0);
  auto keyboard_state =
      jage::input::button::states<typename TestFixture::button_type>{};
  std::ignore = this->monitor.register_callback(
      [&](const auto &state) -> void { keyboard_state = state; });
  this->monitor.poll();

  EXPECT_TRUE(jage::input::button::status::down ==
              keyboard_state[this->button0].status);
}

TYPED_TEST(monitoring, should_invoke_all_registered_callbacks) {
  this->expect_call_to_is_down(this->button0);
  this->expect_call_to_callback(2);

  this->monitor.monitor_input(this->button0);
  std::ignore = this->monitor.register_callback(
      [&](const auto &state) -> void { this->mock_callback.call(state); });
  std::ignore = this->monitor.register_callback(
      [&](const auto &state) -> void { this->mock_callback.call(state); });
  this->monitor.poll();
}

TYPED_TEST(monitoring, should_throw_on_attempt_to_register_too_many_callbacks) {
  std::ignore = this->monitor.register_callback(
      [&](const auto &state) -> void { this->mock_callback.call(state); });
  std::ignore = this->monitor.register_callback(
      [&](const auto &state) -> void { this->mock_callback.call(state); });
  EXPECT_THROW(
      std::ignore = this->monitor.register_callback(
          [&](const auto &state) -> void { this->mock_callback.call(state); }),
      std::runtime_error);
}

TYPED_TEST(monitoring, should_be_able_to_deregister_callback) {
  this->expect_call_to_is_down(this->button0);
  this->expect_call_to_callback();

  this->monitor.monitor_input(this->button0);
  auto callback = this->monitor.register_callback(
      [&](const auto &state) -> void { this->mock_callback.call(state); });
  this->monitor.poll();
  callback.deregister();
  this->monitor.poll();
}

TYPED_TEST(monitoring, should_update_after_deregister) {
  this->expect_call_to_is_down(this->button0, 2);

  auto callback0_call_count = 0UZ;
  auto callback1_call_count = 0UZ;

  this->monitor.monitor_input(this->button0);
  auto callback = this->monitor.register_callback(
      [&](const auto &) -> void { ++callback0_call_count; });
  std::ignore = this->monitor.register_callback(
      [&](const auto &) -> void { ++callback1_call_count; });
  this->monitor.poll();
  callback.deregister();
  this->monitor.poll();
  EXPECT_EQ(1, callback0_call_count);
  EXPECT_EQ(2, callback1_call_count);
}

struct keyboard_monitoring_parameterized
    : public monitoring<keyboard_monitor_test_params>,
      public testing::WithParamInterface<std::underlying_type_t<
          typename keyboard_monitor_test_params::second_type>> {};

TEST_P(keyboard_monitoring_parameterized, should_query_monitor_any_key) {

  const auto key =
      static_cast<typename keyboard_monitor_test_params::second_type>(
          GetParam());
  this->expect_call_to_is_down(key);
  this->monitor.monitor_input(key);
  std::ignore = this->monitor.register_callback(this->null_callback);
  this->monitor.poll();
}

INSTANTIATE_TEST_SUITE_P(
    all_keyboard_keys, keyboard_monitoring_parameterized,
    Range(std::to_underlying(jage::input::keyboard::keys::BEGIN),
          static_cast<std::underlying_type_t<jage::input::keyboard::keys>>(
              std::to_underlying(jage::input::keyboard::keys::END) + 1)));

struct mouse_monitoring_parameterized
    : public monitoring<mouse_monitor_test_params>,
      public testing::WithParamInterface<std::underlying_type_t<
          typename mouse_monitor_test_params::second_type>> {};

TEST_P(mouse_monitoring_parameterized, should_query_monitor_any_key) {

  const auto key =
      static_cast<typename mouse_monitor_test_params::second_type>(GetParam());
  this->expect_call_to_is_down(key);
  this->monitor.monitor_input(key);
  std::ignore = this->monitor.register_callback(this->null_callback);
  this->monitor.poll();
}

INSTANTIATE_TEST_SUITE_P(
    all_mouse_buttons, mouse_monitoring_parameterized,
    Range(std::to_underlying(jage::input::mouse::buttons::BEGIN),
          static_cast<std::underlying_type_t<jage::input::mouse::buttons>>(
              std::to_underlying(jage::input::mouse::buttons::END) + 1)));