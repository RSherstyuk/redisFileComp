#pragma once
#include <string>

struct RedisConfig {
  static std::string getHost();
  static int getPort();
};