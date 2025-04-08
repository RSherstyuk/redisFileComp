#include "config.h"
#include <cstdlib>

std::string RedisConfig::getHost() {
  if (const char *host = std::getenv("REDIS_HOST")) {
    return host;
  }
  return "localhost";
}

int RedisConfig::getPort() {
  if (const char *port = std::getenv("REDIS_PORT")) {
    return std::stoi(port);
  }
  return 6379;
}