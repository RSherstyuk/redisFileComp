#pragma once
#include <filesystem>
#include <hiredis/hiredis.h>
#include <stdexcept>
#include <string>

class RedisQueue {
public:
  RedisQueue(const std::string &host = "localhost", int port = 6379);
  ~RedisQueue();

  void push(const std::filesystem::path &file_path);
  bool try_pop(std::filesystem::path &file_path, int timeout_sec = 5);

private:
  redisContext *context_;
};