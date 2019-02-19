#include "gtest/gtest.h"
#include "config_parser.h"

// referred from https://stackoverflow.com/questions/26030700/unit-testing-c-setup-and-teardown
namespace {
class ConfigParserTest : public ::testing::Test {
 protected:
  NginxConfigParser* parser;
  NginxConfig* out_config;
  LocationMap locInfos;

  ConfigParserTest() {}

  virtual ~ConfigParserTest() {}

  virtual void SetUp() {
      parser = new NginxConfigParser();
      out_config = new NginxConfig();
  }

  // clean up after each test case
  virtual void TearDown() {
      delete parser;
      delete out_config;
      for (auto& locInfo : locInfos)
        delete locInfo.second;
  }
};

TEST_F(ConfigParserTest, EmptyContent) {
  bool success =
    parser->Parse("config_parser_test_input/example_config_EmptyContent", out_config);
  EXPECT_FALSE(success);
}

TEST_F(ConfigParserTest, Allowed_NoSemicolonInBlock) {
  bool success =
    parser->Parse(
      "config_parser_test_input/example_config_Allowed_NoSemicolonInBlock", out_config);
  EXPECT_TRUE(success);
}

TEST_F(ConfigParserTest, Allowed_NoSemicolonInMain) {
  bool success =
    parser->Parse(
      "config_parser_test_input/example_config_Allowed_NoSemicolonInMain", out_config);
  EXPECT_TRUE(success);
}

TEST_F(ConfigParserTest, Allowed_SingleQuoteSlashProblem) {
  bool success =
    parser->Parse(
      "config_parser_test_input/example_config_Allowed_SingleQuoteSlashProblem", out_config);
  EXPECT_TRUE(success);
}

TEST_F(ConfigParserTest, Allowed_DoubleQuoteSlashProblem) {
  bool success =
    parser->Parse(
      "config_parser_test_input/example_config_Allowed_DoubleQuoteSlashProblem", out_config);
  EXPECT_TRUE(success);
}

TEST_F(ConfigParserTest, Example_Config) {
  bool success = parser->Parse("config_parser_test_input/example_config", out_config);
  EXPECT_TRUE(success);
}

TEST_F(ConfigParserTest, NotAllowed_SemicolonAfterBraces) {
  bool success =
    parser->Parse(
      "config_parser_test_input/example_config_NotAllowed_SemicolonAfterBraces", out_config);
  EXPECT_FALSE(success);
}

TEST_F(ConfigParserTest, NotAllowed_LackDoubleQuote1) {
  bool success =
    parser->Parse(
      "config_parser_test_input/example_config_NotAllowed_LackDoubleQuote1", out_config);
  EXPECT_FALSE(success);
}

TEST_F(ConfigParserTest, NotAllowed_LackDoubleQuote2) {
  bool success =
    parser->Parse(
      "config_parser_test_input/example_config_NotAllowed_LackDoubleQuote2", out_config);
  EXPECT_FALSE(success);
}

TEST_F(ConfigParserTest, NotAllowed_LackSingleQuote1) {
  bool success =
    parser->Parse(
      "config_parser_test_input/example_config_NotAllowed_LackSingleQuote1", out_config);
  EXPECT_FALSE(success);
}

TEST_F(ConfigParserTest, NotAllowed_LackSingleQuote2) {
  bool success =
    parser->Parse(
      "config_parser_test_input/example_config_NotAllowed_LackSingleQuote2", out_config);
  EXPECT_FALSE(success);
}

TEST_F(ConfigParserTest, NotAllowed_SingleQuoteSlashProblem) {
  bool success =
    parser->Parse(
      "config_parser_test_input/example_config_NotAllowed_SingleQuoteSlashProblem", out_config);
  EXPECT_FALSE(success);
}

TEST_F(ConfigParserTest, NotAllowed_DoubleQuoteSlashProblem) {
  bool success =
    parser->Parse(
      "config_parser_test_input/example_config_NotAllowed_DoubleQuoteSlashProblem", out_config);
  EXPECT_FALSE(success);
}

// copying over tests from Nate's Assignment 1
TEST_F(ConfigParserTest, SimpleContextConfig) {
  bool success =
    parser->Parse(
      "config_parser_test_input/simple_context_config", out_config);
  EXPECT_TRUE(success);
  EXPECT_EQ(1, out_config->statements_.size());
}


TEST_F(ConfigParserTest, MultipleContextConfig) {
  bool success =
    parser->Parse(
      "config_parser_test_input/multiple_context_config", out_config);
  EXPECT_TRUE(success);
  EXPECT_EQ(2, out_config->statements_.size());
}

TEST_F(ConfigParserTest, ExtraEndBlockConfig) {
  bool success =
    parser->Parse(
      "config_parser_test_input/extra_end_block_config", out_config);
  EXPECT_FALSE(success);
}

TEST_F(ConfigParserTest, EndBlockWithoutStartConfig) {
  bool success =
    parser->Parse(
      "config_parser_test_input/end_block_without_start_config", out_config);
  EXPECT_FALSE(success);
}

TEST_F(ConfigParserTest, MissingEndBlockConfig) {
  bool success =
    parser->Parse(
      "config_parser_test_input/missing_end_block_config", out_config);
  EXPECT_FALSE(success);
}

// Example at http://nginx.org/en/docs/beginners_guide.html#conf_structure shows
// an empty Location block
TEST_F(ConfigParserTest, EmptyBlockConfig) {
  bool success =
    parser->Parse(
      "config_parser_test_input/empty_block_config", out_config);
  EXPECT_TRUE(success);
}

TEST_F(ConfigParserTest, ExtraSemicolonConfig) {
  bool success =
    parser->Parse(
      "config_parser_test_input/extra_semicolon_config", out_config);
  EXPECT_FALSE(success);
}

TEST_F(ConfigParserTest, PortConfigInFirstStatement) {
  bool success =
    parser->Parse(
      "config_parser_test_input/port_config_listen_statement", out_config);
  EXPECT_TRUE(success);
  int16_t portNo = out_config->getPort();
  EXPECT_EQ(portNo, 8080);
}

TEST_F(ConfigParserTest, PortConfigNoFile) {
  bool success = parser->Parse("config_parser_test_input/FILE_DOES_NOT_EXIST", out_config);
  EXPECT_FALSE(success);
  int16_t portNo = out_config->getPort();
  EXPECT_EQ(portNo, -1);
}

TEST_F(ConfigParserTest, TopLevelRootStatement) {
  bool success =
    parser->Parse("config_parser_test_input/top_level_root_statement_config", out_config);
  EXPECT_TRUE(success);
  string rootPath = out_config->getRootPath();
  EXPECT_EQ(rootPath, "/usr/src/projects/iceberg-webserver");
}

TEST_F(ConfigParserTest, SimpleLocationInfos) {
  bool success =
    parser->Parse("config_parser_test_input/simple_location_infos_config", out_config);
  EXPECT_TRUE(success);

  locInfos = out_config->getLocationInfos();
  EXPECT_EQ(locInfos.size(), 2);

  auto staticEntry = locInfos.find("/static1");


  // In simple_locations_infos_config,
  // even though handler for /very/long/path is registered second,
  // it has to be internally sorted in descending length of locations,
  // meaning hanlder for /static1 must come second.
  EXPECT_NE(staticEntry, locInfos.begin());

  ASSERT_NE(staticEntry, locInfos.end());
  ASSERT_NE(staticEntry->second, nullptr);
  EXPECT_EQ(staticEntry->second->handlerType, "static");
  ASSERT_NE(staticEntry->second->blockConfig, nullptr);
  EXPECT_EQ(staticEntry->second->blockConfig->statements_.size(), 2);
}

TEST_F(ConfigParserTest, DuplicateLocationsTest) {
  bool success =
    parser->Parse("config_parser_test_input/duplicate_locations_config", out_config);
  EXPECT_TRUE(success);

  locInfos = out_config->getLocationInfos();
  EXPECT_EQ(locInfos.size(), 1);

  auto echoEntry = locInfos.find("/echo");
  ASSERT_NE(echoEntry, locInfos.end());
  ASSERT_NE(echoEntry->second, nullptr);
  EXPECT_EQ(echoEntry->second->handlerType, "echo1");
  ASSERT_NE(echoEntry->second->blockConfig, nullptr);
  EXPECT_EQ(echoEntry->second->blockConfig->statements_.size(), 3);
}
}  // namespace
