/*
 * COPYRIGHT 2016 SEAGATE LLC
 *
 * THIS DRAWING/DOCUMENT, ITS SPECIFICATIONS, AND THE DATA CONTAINED
 * HEREIN, ARE THE EXCLUSIVE PROPERTY OF SEAGATE TECHNOLOGY
 * LIMITED, ISSUED IN STRICT CONFIDENCE AND SHALL NOT, WITHOUT
 * THE PRIOR WRITTEN PERMISSION OF SEAGATE TECHNOLOGY LIMITED,
 * BE REPRODUCED, COPIED, OR DISCLOSED TO A THIRD PARTY, OR
 * USED FOR ANY PURPOSE WHATSOEVER, OR STORED IN A RETRIEVAL SYSTEM
 * EXCEPT AS ALLOWED BY THE TERMS OF SEAGATE LICENSES AND AGREEMENTS.
 *
 * YOU SHOULD HAVE RECEIVED A COPY OF SEAGATE'S LICENSE ALONG WITH
 * THIS RELEASE. IF NOT PLEASE CONTACT A SEAGATE REPRESENTATIVE
 * http://www.seagate.com/contact
 *
 * Original author:  Rajesh Nambiar <rajesh.nambiar@seagate.com>
 * Original creation date: 30-March-2016
 */

#include "gtest/gtest.h"
#include <functional>
#include "s3_option.h"
#include <iostream>
#include <fstream>
#include <memory>

class S3OptionsTest : public testing::Test {
  protected:
    S3OptionsTest() {
      instance = S3Option::get_instance();
      instance->set_option_file("s3config-test.yaml");
    }

  ~S3OptionsTest() {
    S3Option::destroy_instance();
  }

  S3Option *instance;
};

TEST_F(S3OptionsTest, Constructor) {
  EXPECT_STREQ("/var/log/seagate/s3/s3server.log", instance->get_log_filename().c_str());
  EXPECT_STREQ("INFO", instance->get_log_level().c_str());
  EXPECT_STREQ("0.0.0.0", instance->get_bind_addr().c_str());
  EXPECT_STREQ("localhost@tcp:12345:33:100", instance->get_clovis_local_addr().c_str());
  EXPECT_STREQ("localhost@tcp:12345:33:100", instance->get_clovis_confd_addr().c_str());
  EXPECT_STREQ("<0x7000000000000001:0>", instance->get_clovis_prof().c_str());
  EXPECT_STREQ("127.0.0.1", instance->get_auth_ip_addr().c_str());
  EXPECT_EQ(8081, instance->get_s3_bind_port());
  EXPECT_EQ(8095, instance->get_auth_port());
  EXPECT_EQ(9, instance->get_clovis_layout_id());
}

TEST_F(S3OptionsTest, SingletonCheck) {
  S3Option *inst1 = S3Option::get_instance();
  S3Option *inst2 = S3Option::get_instance();
  EXPECT_EQ(inst1, inst2);
  ASSERT_TRUE(inst1);
}

TEST_F(S3OptionsTest, GetOptionsfromFile) {
  instance->load_all_sections(false);
  EXPECT_EQ(std::string("s3config-test.yaml"), instance->get_option_file());
  EXPECT_EQ(std::string("/var/log/seagate/s3/s3server.log"), instance->get_log_filename());
  EXPECT_EQ(std::string("INFO"), instance->get_log_level());
  EXPECT_EQ(std::string("10.10.1.1"), instance->get_bind_addr());
  EXPECT_EQ(std::string("<ipaddress>@tcp:12345:33:100"), instance->get_clovis_local_addr());
  EXPECT_EQ(std::string("<ipaddress>@tcp:12345:33:100"), instance->get_clovis_confd_addr());
  EXPECT_EQ(std::string("<0x7000000000000001:0>"), instance->get_clovis_prof());
  EXPECT_EQ(std::string("10.10.1.2"), instance->get_auth_ip_addr());
  EXPECT_EQ(9081, instance->get_s3_bind_port());
  EXPECT_EQ(8095, instance->get_auth_port());
  EXPECT_EQ(1, instance->get_clovis_layout_id());
  EXPECT_EQ(0, instance->s3_performance_enabled());
}

TEST_F(S3OptionsTest, TestOverrideOptions) {
  instance->set_cmdline_option(S3_OPTION_BIND_ADDR, "198.1.1.1");
  instance->set_cmdline_option(S3_OPTION_BIND_PORT, "1");
  instance->set_cmdline_option(S3_OPTION_CLOVIS_LOCAL_ADDR, "localhost@test");
  instance->set_cmdline_option(S3_OPTION_CLOVIS_CONFD_ADDR, "localhost@test");
  instance->set_cmdline_option(S3_OPTION_AUTH_IP_ADDR, "192.192.191");
  instance->set_cmdline_option(S3_OPTION_AUTH_PORT, "2");
  instance->set_cmdline_option(S3_CLOVIS_LAYOUT_ID, "123");
  instance->set_cmdline_option(S3_OPTION_LOG_FILE, "/tmp/log.txt");
  instance->set_cmdline_option(S3_OPTION_LOG_MODE, "debug");
  // load from Config file, overriding the command options
  instance->load_all_sections(true);
  EXPECT_EQ(std::string("/var/log/seagate/s3/s3server.log"), instance->get_log_filename());
  EXPECT_EQ(std::string("INFO"), instance->get_log_level());
  EXPECT_EQ(std::string("10.10.1.1"), instance->get_bind_addr());
  EXPECT_EQ(std::string("<ipaddress>@tcp:12345:33:100"), instance->get_clovis_local_addr());
  EXPECT_EQ(std::string("<ipaddress>@tcp:12345:33:100"), instance->get_clovis_confd_addr());
  EXPECT_EQ(std::string("<0x7000000000000001:0>"), instance->get_clovis_prof());
  EXPECT_EQ(std::string("10.10.1.2"), instance->get_auth_ip_addr());
  EXPECT_EQ(9081, instance->get_s3_bind_port());
  EXPECT_EQ(8095, instance->get_auth_port());
  EXPECT_EQ(1, instance->get_clovis_layout_id());
}

TEST_F(S3OptionsTest, TestDontOverrideCmdOptions) {
  instance->set_cmdline_option(S3_OPTION_BIND_ADDR, "198.1.1.1");
  instance->set_cmdline_option(S3_OPTION_BIND_PORT, "1");
  instance->set_cmdline_option(S3_OPTION_CLOVIS_LOCAL_ADDR, "localhost@test");
  instance->set_cmdline_option(S3_OPTION_CLOVIS_CONFD_ADDR, "localhost@test");
  instance->set_cmdline_option(S3_OPTION_AUTH_IP_ADDR, "192.168.15.131");
  instance->set_cmdline_option(S3_OPTION_AUTH_PORT, "2");
  instance->set_cmdline_option(S3_CLOVIS_LAYOUT_ID, "123");
  instance->set_cmdline_option(S3_OPTION_LOG_FILE, "/tmp/log.txt");
  instance->set_cmdline_option(S3_OPTION_LOG_MODE, "debug");
  instance->load_all_sections(false);
  EXPECT_EQ(std::string("s3config-test.yaml"), instance->get_option_file());
  EXPECT_EQ(std::string("/tmp/log.txt"), instance->get_log_filename());
  EXPECT_EQ(std::string("debug"), instance->get_log_level());
  EXPECT_EQ(std::string("198.1.1.1"), instance->get_bind_addr());
  EXPECT_EQ(std::string("localhost@test"), instance->get_clovis_local_addr());
  EXPECT_EQ(std::string("localhost@test"), instance->get_clovis_confd_addr());
  EXPECT_EQ(std::string("192.168.15.131"), instance->get_auth_ip_addr());
  EXPECT_EQ(1, instance->get_s3_bind_port());
  EXPECT_EQ(2, instance->get_auth_port());
  EXPECT_EQ(123, instance->get_clovis_layout_id());
}

TEST_F(S3OptionsTest, LoadS3SectionFromFile) {
  instance->load_section("S3_SERVER_CONFIG", false);

  EXPECT_EQ(std::string("/var/log/seagate/s3/s3server.log"), instance->get_log_filename());
  EXPECT_EQ(std::string("INFO"), instance->get_log_level());
  EXPECT_EQ(std::string("10.10.1.1"), instance->get_bind_addr());
  EXPECT_EQ(9081, instance->get_s3_bind_port());

  // These will come with default values.
  EXPECT_EQ(std::string("localhost@tcp:12345:33:100"), instance->get_clovis_local_addr());
  EXPECT_EQ(std::string("localhost@tcp:12345:33:100"), instance->get_clovis_confd_addr());
  EXPECT_EQ(std::string("<0x7000000000000001:0>"), instance->get_clovis_prof());
  EXPECT_EQ(9, instance->get_clovis_layout_id());
  EXPECT_EQ(std::string("127.0.0.1"), instance->get_auth_ip_addr());
  EXPECT_EQ(8095, instance->get_auth_port());

}

TEST_F(S3OptionsTest, LoadSelectiveS3SectionFromFile) {

  instance->set_cmdline_option(S3_OPTION_BIND_ADDR, "198.1.1.1");
  instance->set_cmdline_option(S3_OPTION_BIND_PORT, "1");
  instance->set_cmdline_option(S3_OPTION_LOG_FILE, "/tmp/log.txt");
  instance->set_cmdline_option(S3_OPTION_LOG_MODE, "debug");
  instance->load_section("S3_SERVER_CONFIG", true);

  EXPECT_EQ(std::string("/var/log/seagate/s3/s3server.log"), instance->get_log_filename());
  EXPECT_EQ(std::string("INFO"), instance->get_log_level());
  EXPECT_EQ(std::string("10.10.1.1"), instance->get_bind_addr());
  EXPECT_EQ(9081, instance->get_s3_bind_port());

  // These should be default values
  EXPECT_EQ(std::string("localhost@tcp:12345:33:100"), instance->get_clovis_local_addr());
  EXPECT_EQ(std::string("localhost@tcp:12345:33:100"), instance->get_clovis_confd_addr());
  EXPECT_EQ(std::string("<0x7000000000000001:0>"), instance->get_clovis_prof());
  EXPECT_EQ(9, instance->get_clovis_layout_id());
  EXPECT_EQ(std::string("127.0.0.1"), instance->get_auth_ip_addr());
  EXPECT_EQ(8095, instance->get_auth_port());
}

TEST_F(S3OptionsTest, LoadAuthSectionFromFile) {
  instance->load_section("S3_AUTH_CONFIG", false);

  EXPECT_EQ(std::string("10.10.1.2"), instance->get_auth_ip_addr());
  EXPECT_EQ(8095, instance->get_auth_port());

  // Others should not be loaded
  EXPECT_EQ(std::string("/var/log/seagate/s3/s3server.log"), instance->get_log_filename());
  EXPECT_EQ(std::string("INFO"), instance->get_log_level());
  EXPECT_EQ(std::string("0.0.0.0"), instance->get_bind_addr());
  EXPECT_EQ(8081, instance->get_s3_bind_port());
  EXPECT_EQ(std::string("localhost@tcp:12345:33:100"), instance->get_clovis_local_addr());
  EXPECT_EQ(std::string("localhost@tcp:12345:33:100"), instance->get_clovis_confd_addr());
  EXPECT_EQ(std::string("<0x7000000000000001:0>"), instance->get_clovis_prof());
  EXPECT_EQ(9, instance->get_clovis_layout_id());
}

TEST_F(S3OptionsTest, LoadSelectiveAuthSectionFromFile) {

  instance->set_cmdline_option(S3_OPTION_AUTH_IP_ADDR, "192.192.191.1");
  instance->set_cmdline_option(S3_OPTION_AUTH_PORT, "2");
  instance->load_section("S3_AUTH_CONFIG", true);

  EXPECT_EQ(std::string("10.10.1.2"), instance->get_auth_ip_addr());
  EXPECT_EQ(8095, instance->get_auth_port());

  // Others should not be loaded
  EXPECT_EQ(std::string("/var/log/seagate/s3/s3server.log"), instance->get_log_filename());
  EXPECT_EQ(std::string("INFO"), instance->get_log_level());
  EXPECT_EQ(std::string("0.0.0.0"), instance->get_bind_addr());
  EXPECT_EQ(8081, instance->get_s3_bind_port());
  EXPECT_EQ(std::string("localhost@tcp:12345:33:100"), instance->get_clovis_local_addr());
  EXPECT_EQ(std::string("localhost@tcp:12345:33:100"), instance->get_clovis_confd_addr());
  EXPECT_EQ(std::string("<0x7000000000000001:0>"), instance->get_clovis_prof());
  EXPECT_EQ(9, instance->get_clovis_layout_id());
}

TEST_F(S3OptionsTest, LoadClovisSectionFromFile) {
  instance->load_section("S3_CLOVIS_CONFIG", false);

  EXPECT_EQ(std::string("<ipaddress>@tcp:12345:33:100"), instance->get_clovis_local_addr());
  EXPECT_EQ(std::string("<ipaddress>@tcp:12345:33:100"), instance->get_clovis_confd_addr());
  EXPECT_EQ(std::string("<0x7000000000000001:0>"), instance->get_clovis_prof());

  // Others should not be loaded
  EXPECT_EQ(std::string("/var/log/seagate/s3/s3server.log"), instance->get_log_filename());
  EXPECT_EQ(std::string("INFO"), instance->get_log_level());
  EXPECT_EQ(std::string("0.0.0.0"), instance->get_bind_addr());
  EXPECT_EQ(8081, instance->get_s3_bind_port());
}

TEST_F(S3OptionsTest, LoadSelectiveClovisSectionFromFile) {
  instance->set_cmdline_option(S3_OPTION_CLOVIS_LOCAL_ADDR, "localhost@test");
  instance->set_cmdline_option(S3_OPTION_CLOVIS_CONFD_ADDR, "localhost@test");
  instance->load_section("S3_CLOVIS_CONFIG", true);

  EXPECT_EQ(std::string("<ipaddress>@tcp:12345:33:100"), instance->get_clovis_local_addr());
  EXPECT_EQ(std::string("<ipaddress>@tcp:12345:33:100"), instance->get_clovis_confd_addr());
  EXPECT_EQ(std::string("<0x7000000000000001:0>"), instance->get_clovis_prof());

  // Others should not be loaded
  EXPECT_EQ(std::string("/var/log/seagate/s3/s3server.log"), instance->get_log_filename());
  EXPECT_EQ(std::string("INFO"), instance->get_log_level());
  EXPECT_EQ(std::string("0.0.0.0"), instance->get_bind_addr());
  EXPECT_EQ(8081, instance->get_s3_bind_port());
}

TEST_F(S3OptionsTest, SetCmdOptionFlag) {
  int flag;
  instance->set_cmdline_option(S3_OPTION_BIND_ADDR, "198.1.1.1");
  instance->set_cmdline_option(S3_OPTION_BIND_PORT, "1");
  instance->set_cmdline_option(S3_OPTION_CLOVIS_LOCAL_ADDR, "localhost@test");
  instance->set_cmdline_option(S3_OPTION_CLOVIS_CONFD_ADDR, "localhost@test");
  instance->set_cmdline_option(S3_OPTION_AUTH_IP_ADDR, "192.192.191");
  instance->set_cmdline_option(S3_OPTION_AUTH_PORT, "2");
  instance->set_cmdline_option(S3_CLOVIS_LAYOUT_ID, "123");
  instance->set_cmdline_option(S3_OPTION_LOG_FILE, "/tmp/log.txt");
  instance->set_cmdline_option(S3_OPTION_LOG_MODE, "debug");

  flag = S3_OPTION_BIND_ADDR | S3_OPTION_BIND_PORT | S3_OPTION_CLOVIS_LOCAL_ADDR |
         S3_OPTION_CLOVIS_CONFD_ADDR | S3_OPTION_AUTH_IP_ADDR | S3_OPTION_AUTH_PORT |
         S3_CLOVIS_LAYOUT_ID | S3_OPTION_LOG_FILE | S3_OPTION_LOG_MODE;

  EXPECT_EQ(flag, instance->get_cmd_opt_flag());
}

TEST_F(S3OptionsTest, GetDefaultEndPoint) {
  instance->load_all_sections(false);
  EXPECT_EQ(std::string("s3.seagate-test.com"), instance->get_default_endpoint());
}

TEST_F(S3OptionsTest, GetRegionEndPoints) {
  instance->load_all_sections(false);
  std::set<std::string> region_eps = instance->get_region_endpoints();
  EXPECT_TRUE(region_eps.find("s3-asia.seagate-test.com") != region_eps.end());
  EXPECT_TRUE(region_eps.find("s3-us.seagate-test.com") != region_eps.end());
  EXPECT_TRUE(region_eps.find("s3-europe.seagate-test.com") != region_eps.end());
  EXPECT_FALSE(region_eps.find("invalid-region.seagate.com") != region_eps.end());
}
