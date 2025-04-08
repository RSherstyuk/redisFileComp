#include "config.h"
#include <cstdlib>
#include <stdexcept>

std::string RedisConfig::getEnvOrDefault(const char* env_var, const std::string& default_value) {
    const char* val = std::getenv(env_var);
    return val ? val : default_value;
}

int RedisConfig::getEnvOrDefault(const char* env_var, int default_value) {
    const char* val = std::getenv(env_var);
    return val ? std::stoi(val) : default_value;
}

std::string RedisConfig::getHost() {
    return getEnvOrDefault("REDIS_HOST", "localhost");
}

int RedisConfig::getPort() {
    return getEnvOrDefault("REDIS_PORT", 6379);
}