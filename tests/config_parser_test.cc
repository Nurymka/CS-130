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

  TEST_F(ConfigParserTest, NotAllowed_SemicolonAfterBraces) {
    bool success = parser->Parse("example_config_NotAllowed_SemicolonAfterBraces", out_config);
    EXPECT_FALSE(success);
  }

  TEST_F(ConfigParserTest, NotAllowed_LackDoubleQuote1) {
    bool success = parser->Parse("example_config_NotAllowed_LackDoubleQuote1", out_config);
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

  // copying over tests from Nate's Assignment 1
  TEST_F(ConfigParserTest, SimpleContextConfig) {
    bool success = parser->Parse("simple_context_config", out_config);
    EXPECT_TRUE(success);
    EXPECT_EQ(1, out_config->statements_.size());
  }


  TEST_F(ConfigParserTest, MultipleContextConfig) {
    bool success = parser->Parse("multiple_context_config", out_config);
    EXPECT_TRUE(success);
    EXPECT_EQ(2, out_config->statements_.size());
  }

  TEST_F(ConfigParserTest, ExtraEndBlockConfig) {
    bool success = parser->Parse("extra_end_block_config", out_config);
    EXPECT_FALSE(success);
  }

  TEST_F(ConfigParserTest, EndBlockWithoutStartConfig) {
    bool success = parser->Parse("end_block_without_start_config", out_config);
    EXPECT_FALSE(success);
  }

  TEST_F(ConfigParserTest, MissingEndBlockConfig) {
    bool success = parser->Parse("missing_end_block_config", out_config);
    EXPECT_FALSE(success);
  }

  // Example at http://nginx.org/en/docs/beginners_guide.html#conf_structure shows
  // an empty Location block
  TEST_F(ConfigParserTest, EmptyBlockConfig) {
    bool success = parser->Parse("empty_block_config", out_config);
    EXPECT_TRUE(success);
  }

  TEST_F(ConfigParserTest, ExtraSemicolonConfig) {
    bool success = parser->Parse("extra_semicolon_config", out_config);
    EXPECT_FALSE(success);
  }

  TEST_F(ConfigParserTest, PortConfigInHttpBlock) {
    bool success = parser->Parse("port_config_http_block", out_config);
    EXPECT_TRUE(success);
    int portNo = out_config->getPort();
    EXPECT_EQ(portNo, 8080);
  }

  TEST_F(ConfigParserTest, PortConfigInServerBlock) {
    bool success = parser->Parse("port_config_server_block", out_config);
    EXPECT_TRUE(success);
    int portNo = out_config->getPort();
    EXPECT_EQ(portNo, 8080);
  }

  TEST_F(ConfigParserTest, PortConfigInFirstStatement) {
    bool success = parser->Parse("port_config_listen_statement", out_config);
    EXPECT_TRUE(success);
    int portNo = out_config->getPort();
    EXPECT_EQ(portNo, 8080);
  }

  TEST_F(ConfigParserTest, PortConfigNoFile) {
    bool success = parser->Parse("FILE_DOES_NOT_EXIST", out_config);
    EXPECT_FALSE(success);
    int portNo = out_config->getPort();
    EXPECT_EQ(portNo, -1);
  }


}


