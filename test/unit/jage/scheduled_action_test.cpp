#include <jage/scheduled_action.hpp>
#include <jage/scheduled_action_status.hpp>

#include <GUnit.h>

using namespace std::chrono_literals;

class scheduled_action_status : public ::testing::Test {
protected:
  jage::scheduled_action<> scheduled_action;
};

class scheduled_action_with_time : public scheduled_action_status {
protected:
  void SetUp() override { scheduled_action = jage::scheduled_action{10ns}; }
};

GTEST(scheduled_action_status) {
  using jage::scheduled_action_status;

  SHOULD("Be active on construction") {
    EXPECT(scheduled_action_status::active == scheduled_action.status());
  }

  SHOULD("Be paused after pause") {
    scheduled_action.pause();
    EXPECT(scheduled_action_status::paused == scheduled_action.status());
  }

  SHOULD("Be active after resume") {
    scheduled_action.pause();
    scheduled_action.resume();
    EXPECT(scheduled_action_status::active == scheduled_action.status());
  }

  SHOULD("Be canceled after cancel") {
    scheduled_action.cancel();
    EXPECT(scheduled_action_status::canceled == scheduled_action.status());
  }

  SHOULD("Not resume after cancel") {
    scheduled_action.cancel();
    scheduled_action.resume();
    EXPECT(scheduled_action_status::canceled == scheduled_action.status());
  }

  SHOULD("Not pause after cancel") {
    scheduled_action.cancel();
    scheduled_action.pause();
    EXPECT(scheduled_action_status::canceled == scheduled_action.status());
  }

  SHOULD("Complete after time expires") {
    scheduled_action.update(1ns);
    EXPECT(scheduled_action_status::complete == scheduled_action.status());
  }

  SHOULD("Complete after 0 ns because default duration is 0 ns") {
    scheduled_action.update(0ns);
    EXPECT(scheduled_action_status::complete == scheduled_action.status());
  }
}

GTEST(scheduled_action_with_time) {
  using jage::scheduled_action_status;

  SHOULD("Stay active if not enough time has elapsed") {
    scheduled_action.update(1ns);
    EXPECT(scheduled_action_status::active == scheduled_action.status());
  }

  SHOULD("Complete after enough time has elapsed") {
    scheduled_action.update(1ns);
    scheduled_action.update(9ns);
    EXPECT(scheduled_action_status::complete == scheduled_action.status());
  }

  SHOULD("Remain complete after cancel post time elapsed") {
    scheduled_action.update(10ns);
    scheduled_action.cancel();
    EXPECT(scheduled_action_status::complete == scheduled_action.status());
  }

  SHOULD("Be complete after time expires") {
    scheduled_action.update(10ns);
    EXPECT(scheduled_action.is_complete());
  }

  SHOULD("Not be complete before time expires") {
    scheduled_action.update(9ns);
    EXPECT(not scheduled_action.is_complete());
  }

  SHOULD("Be complete after cancel") {
    scheduled_action.cancel();
    EXPECT(scheduled_action.is_complete());
  }

  SHOULD("Not be complete after pause") {
    scheduled_action.pause();
    EXPECT(not scheduled_action.is_complete());
  }

  SHOULD("Become active after reset while paused") {
    scheduled_action.pause();
    scheduled_action.reset(10ns);
    EXPECT(scheduled_action_status::active == scheduled_action.status());
  }

  SHOULD("Become active after reset while canceled") {
    scheduled_action.cancel();
    scheduled_action.reset(10ns);
    EXPECT(scheduled_action_status::active == scheduled_action.status());
  }

  SHOULD("Become active after reset while complete") {
    scheduled_action.update(10ns);
    scheduled_action.reset(10ns);
    EXPECT(scheduled_action_status::active == scheduled_action.status());
  }

  SHOULD("Reset elapsed time on reset") {
    scheduled_action.update(10ns);
    scheduled_action.reset(10ns);
    scheduled_action.update(9ns);
    EXPECT(scheduled_action_status::active == scheduled_action.status());
  }

  SHOULD("Stay active after extending time") {
    scheduled_action.update(9ns);
    scheduled_action.extend(2ns);
    scheduled_action.update(1ns);
    EXPECT(scheduled_action_status::active == scheduled_action.status());
  }

  SHOULD("Complete after additional time elapses") {
    scheduled_action.update(9ns);
    scheduled_action.extend(2ns);
    scheduled_action.update(3ns);
    EXPECT(scheduled_action_status::complete == scheduled_action.status());
  }
}

GTEST("scheduled_action_with_action") {
  SHOULD("Not execute action before time expires") {
    std::uint8_t value{0};
    auto scheduled_action = jage::scheduled_action{
        10ns,
        [&] { value = 42; },
    };
    scheduled_action.update(9ns);
    EXPECT(0 == value);
  }

  SHOULD("Execute action after time expires") {
    std::uint8_t value{0};
    auto scheduled_action = jage::scheduled_action{
        10ns,
        [&] { value = 42; },
    };

    scheduled_action.update(10ns);
    EXPECT(42 == value);
  }

  SHOULD("Not invoke the action again") {
    std::uint8_t value{0};
    auto scheduled_action = jage::scheduled_action{
        10ns,
        [&] { value = 42; },
    };

    scheduled_action.update(10ns);
    value = 0;
    scheduled_action.update(1ns);
    EXPECT(0 == value);
  }

  SHOULD("Invoke the action again after reset") {
    std::uint8_t value{0};
    auto scheduled_action = jage::scheduled_action{
        10ns,
        [&] { value = 42; },
    };

    scheduled_action.update(10ns);
    value = 0;
    scheduled_action.reset(10ns);
    scheduled_action.update(9ns);
    EXPECT(0 == value);
    scheduled_action.update(1ns);
    EXPECT(42 == value);
  }
}

GTEST("schedule_action_with_action") {
  SHOULD("Not update time remaining when paused") {
    std::uint8_t value{0};
    auto scheduled_action = jage::scheduled_action{
        10ns,
        [&] { value = 42; },
    };

    scheduled_action.pause();
    scheduled_action.update(20ns);
    EXPECT(0 == value);
    scheduled_action.resume();
    scheduled_action.update(10ns);
    EXPECT(42 == value);
  }
}
