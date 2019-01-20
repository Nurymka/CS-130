#include "gtest/gtest.h"
#include "config_parser.h"

// referred from https://stackoverflow.com/questions/26030700/unit-testing-c-setup-and-teardown
namespace {
  class ConfigParserTest : public ::testing::Test {

  protected:
    NginxConfigParser* parser;
    NginxConfig* out_config;

    ConfigParserTest() {

    }

    virtual ~ConfigParserTest() {

    }

    virtual void SetUp() {
        parser = new NginxConfigParser();
        out_config = new NginxConfig();
    }

    //clean up after each test case
    virtual void TearDown() {
        delete parser;
        delete out_config;
    }
  };

  TEST_F(ConfigParserTest, EmptyContent) {
    bool success = parser->Parse("example_config_EmptyContent", out_config);
    EXPECT_FALSE(success);
  }
  
  TEST_F(ConfigParserTest, Allowed_NoSemicolonInBlock) {
    bool success = parser->Parse("example_config_Allowed_NoSemicolonInBlock", out_config);
    EXPECT_TRUE(success);
  }

  TEST_F(ConfigParserTest, Allowed_NoSemicolonInMain) {
    bool success = parser->Parse("example_config_Allowed_NoSemicolonInMain", out_config);
    EXPECT_TRUE(success);
  }

  TEST_F(ConfigParserTest, Allowed_SingleQuoteSlashProblem) {
    bool success = parser->Parse("example_config_Allowed_SingleQuoteSlashProblem", out_config);
    EXPECT_TRUE(success);
  }

  TEST_F(ConfigParserTest, Allowed_DoubleQuoteSlashProblem) {
    bool success = parser->Parse("example_config_Allowed_DoubleQuoteSlashProblem", out_config);
    EXPECT_TRUE(success);
  }
  
  TEST_F(ConfigParserTest, Example_Config) {
    bool success = parser->Parse("example_config", out_config);
    EXPECT_TRUE(success);
  }

  TEST_F(ConfigParserTest, NotAllowed_ExtraLeftBrace) {
    bool success = parser->Parse("example_config_NotAllowed_ExtraLeftBrace", out_config);
    EXPECT_FALSE(success);
  }

  TEST_F(ConfigParserTest, NotAllowed_NoSemicolonInMain) {
    bool success = parser->Parse("example_config_NotAllowed_NoSemicolonInMain", out_config);
    EXPECT_FALSE(success);
  }

  TEST_F(ConfigParserTest, NotAllowed_SemicolonAfterBraces) {
    bool success = parser->Parse("example_config_NotAllowed_SemicolonAfterBraces", out_config);
    EXPECT_FALSE(success);
  }

  TEST_F(ConfigParserTest, NotAllowed_LackDoubleQuote) {
    bool success = parser->Parse("example_config_NotAllowed_LackDoubleQuote", out_config);
    EXPECT_FALSE(success);
  }

  TEST_F(ConfigParserTest, NotAllowed_LackDoubleQuote2) {
    bool success = parser->Parse("example_config_NotAllowed_LackDoubleQuote2", out_config);
    EXPECT_FALSE(success);
  }

  TEST_F(ConfigParserTest, NotAllowed_LackSingleQuote1) {
    bool success = parser->Parse("example_config_NotAllowed_LackSingleQuote1", out_config);
    EXPECT_FALSE(success);
  }

  TEST_F(ConfigParserTest, NotAllowed_LackSingleQuote2) {
    bool success = parser->Parse("example_config_NotAllowed_LackSingleQuote2", out_config);
    EXPECT_FALSE(success);
  }

  TEST_F(ConfigParserTest, NotAllowed_SingleQuoteSlashProblem) {
    bool success = parser->Parse("example_config_NotAllowed_SingleQuoteSlashProblem", out_config);
    EXPECT_FALSE(success);
  }

  TEST_F(ConfigParserTest, NotAllowed_DoubleQuoteSlashProblem) {
    bool success = parser->Parse("example_config_NotAllowed_DoubleQuoteSlashProblem", out_config);
    EXPECT_FALSE(success);
  }

}


