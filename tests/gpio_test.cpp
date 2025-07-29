#include "sdk/gpio.hpp"
#include <gtest/gtest.h>


using namespace ti_sdk;

class GPIOTest : public ::testing::Test {
protected:
  void SetUp() override { GPIO::initialize(); }

  void TearDown() override {
    // Reset GPIO state after each test
    GPIO::initialize();
  }
};

TEST_F(GPIOTest, ConfigurePin) {
  EXPECT_TRUE(GPIO::configurePin(1, 0, PinMode::OUTPUT));
  EXPECT_TRUE(GPIO::configurePin(1, 0, PinMode::INPUT));
}

TEST_F(GPIOTest, WriteAndReadPin) {
  EXPECT_TRUE(GPIO::configurePin(1, 0, PinMode::OUTPUT));

  EXPECT_TRUE(GPIO::writePin(1, 0, PinState::HIGH));
  EXPECT_EQ(GPIO::readPin(1, 0), PinState::HIGH);

  EXPECT_TRUE(GPIO::writePin(1, 0, PinState::LOW));
  EXPECT_EQ(GPIO::readPin(1, 0), PinState::LOW);
}

TEST_F(GPIOTest, TogglePin) {
  EXPECT_TRUE(GPIO::configurePin(1, 0, PinMode::OUTPUT));

  EXPECT_TRUE(GPIO::writePin(1, 0, PinState::LOW));
  EXPECT_TRUE(GPIO::togglePin(1, 0));
  EXPECT_EQ(GPIO::readPin(1, 0), PinState::HIGH);

  EXPECT_TRUE(GPIO::togglePin(1, 0));
  EXPECT_EQ(GPIO::readPin(1, 0), PinState::LOW);
}

TEST_F(GPIOTest, WriteToInputPin) {
  EXPECT_TRUE(GPIO::configurePin(1, 0, PinMode::INPUT));
  EXPECT_FALSE(GPIO::writePin(1, 0, PinState::HIGH));
}

TEST_F(GPIOTest, SaveAndRestoreState) {
  EXPECT_TRUE(GPIO::configurePin(1, 0, PinMode::OUTPUT));
  EXPECT_TRUE(GPIO::writePin(1, 0, PinState::HIGH));

  std::string state = GPIO::saveState();
  EXPECT_FALSE(state.empty());

  EXPECT_TRUE(GPIO::initialize());               // Reset state
  EXPECT_EQ(GPIO::readPin(1, 0), PinState::LOW); // Default state

  EXPECT_TRUE(GPIO::restoreState(state));
  EXPECT_EQ(GPIO::readPin(1, 0), PinState::HIGH); // Restored state
}