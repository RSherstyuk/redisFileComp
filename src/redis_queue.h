#pragma once
#include <filesystem>
#include <hiredis/hiredis.h>
#include <string>

class RedisQueue {
public:
  RedisQueue(const std::string &host, int port);
  ~RedisQueue();

  static RedisQueue createFromConfig();

  void push(const std::filesystem::path &file_path);

  bool try_pop(std::filesystem::path &file_path, int timeout_sec);
  void push(const std::string &value);

private:
  redisContext *context_;
};