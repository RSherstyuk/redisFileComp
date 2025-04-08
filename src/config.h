#pragma once
#include <string>

class RedisConfig {
public:
    static std::string getHost();
    static int getPort();
    
private:
    static std::string getEnvOrDefault(const char* env_var, const std::string& default_value);
    static int getEnvOrDefault(const char* env_var, int default_value);
};