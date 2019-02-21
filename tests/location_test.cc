#include "gtest/gtest.h"
#include "config_parser.h"
#include "location.h"

namespace {
class LocationUtilsTest : public ::testing::Test {
 protected:
  NginxConfigParser parser;
  NginxConfig config;
  LocationMap locMap;

  LocationUtilsTest() {}

  virtual ~LocationUtilsTest() {}

  virtual void TearDown() {
    for (auto& locInfo : locMap)
      delete locInfo.second;
  }
};

TEST_F(LocationUtilsTest, SimpleLocationInfos) {
  bool success =
    parser.Parse("config_parser_test_input/simple_location_infos_config", &config);
  EXPECT_TRUE(success);

  locMap = LocationUtils::getLocationMapFrom(config);
  EXPECT_EQ(locMap.size(), 3);

  auto staticEntry = locMap.find("/static1");

  ASSERT_NE(staticEntry, locMap.end());
  ASSERT_NE(staticEntry->second, nullptr);
  EXPECT_EQ(staticEntry->second->handlerType, "static");
  ASSERT_NE(staticEntry->second->blockConfig, nullptr);
  EXPECT_EQ(staticEntry->second->blockConfig->statements_.size(), 2);
}

TEST_F(LocationUtilsTest, DuplicateLocationsTest) {
  bool success =
    parser.Parse("config_parser_test_input/duplicate_locations_config", &config);
  EXPECT_TRUE(success);

  locMap = LocationUtils::getLocationMapFrom(config);
  EXPECT_EQ(locMap.size(), 1);

  auto echoEntry = locMap.find("/echo");
  ASSERT_NE(echoEntry, locMap.end());
  ASSERT_NE(echoEntry->second, nullptr);
  EXPECT_EQ(echoEntry->second->handlerType, "echo1");
  ASSERT_NE(echoEntry->second->blockConfig, nullptr);
  EXPECT_EQ(echoEntry->second->blockConfig->statements_.size(), 3);
}

TEST_F(LocationUtilsTest, LongestMatchingLocationTest) {
  bool success =
    parser.Parse("config_parser_test_input/simple_location_infos_config", &config);
  EXPECT_TRUE(success);

  locMap = LocationUtils::getLocationMapFrom(config);
  EXPECT_EQ(locMap.size(), 3);

  LocationInfo *matchingLocInfo = LocationUtils::getLongestMatchingLocation("/very/long", locMap);
  ASSERT_NE(matchingLocInfo, nullptr);
  EXPECT_EQ(matchingLocInfo->handlerType, "static_b");
  ASSERT_NE(matchingLocInfo->blockConfig, nullptr);
  EXPECT_EQ(matchingLocInfo->blockConfig->statements_.size(), 4);
}
}  // namespace
