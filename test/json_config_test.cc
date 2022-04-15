#include <gtest/gtest.h>

#include "config/config.h"
// #include ""

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override{
        _c = loadConfig("config/test_config.json");
    }

    void TearDown() override {
        delete _c;
    }

    Config *_c;
};

TEST_F(ConfigTest, configTest) {
    ASSERT_EQ(_c->acceptors.size(), 2);
    ASSERT_EQ(_c->dialers.size(), 2);
    auto &socks_in = _c->acceptors[0];
    auto &ss_in = _c->acceptors[1];
    auto &socks_out = _c->dialers[0];
    auto &ss_out = _c->dialers[1];
    EXPECT_EQ(socks_in.tag, "my_socks");
    EXPECT_EQ(socks_in.protocol, "socks");
    EXPECT_EQ(socks_in.detail, nullptr);
    EXPECT_EQ(ss_in.tag, "fake_shadowsocks_in");
    EXPECT_EQ(ss_in.protocol, "aes_128_cfb_test");
    EXPECT_EQ(ss_in.detail->address, "0.0.0.0");
    EXPECT_EQ(ss_in.detail->type, "domain");
    EXPECT_EQ(ss_in.detail->port, 23333);
    EXPECT_EQ(ss_in.detail->password, "hello this is a cipher");

    EXPECT_EQ(socks_out.tag, "direct");
    EXPECT_EQ(socks_out.protocol, "direct");
    EXPECT_EQ(socks_out.detail, nullptr);
    EXPECT_EQ(ss_out.tag, "fake_shadowsocks_out");
    EXPECT_EQ(ss_out.protocol, "aes_128_cfb_test");
    EXPECT_EQ(ss_out.detail->address, "remote.com");
    EXPECT_EQ(ss_out.detail->type, "domain");
    EXPECT_EQ(ss_out.detail->port, 42);
    EXPECT_EQ(ss_out.detail->password, "hello this is a cipher");
}
