#include "redis_queue.h"
#include <iostream>

RedisQueue::RedisQueue(const std::string &host, int port) {
  context_ = redisConnect(host.c_str(), port);
  if (context_ == nullptr || context_->err) {
    if (context_) {
      throw std::runtime_error("Redis connection error: " +
                               std::string(context_->errstr));
    } else {
      throw std::runtime_error("Can't allocate redis context");
    }
  }
}

RedisQueue::~RedisQueue() {
  if (context_) {
    redisFree(context_);
  }
}

void RedisQueue::push(const std::filesystem::path &file_path) {
  auto reply = (redisReply *)redisCommand(context_, "LPUSH file_queue %s",
                                          file_path.string().c_str());
  if (!reply) {
    throw std::runtime_error("Redis command failed");
  }
  freeReplyObject(reply);
}

bool RedisQueue::try_pop(std::filesystem::path &file_path, int timeout_sec) {
  auto reply =
      (redisReply *)redisCommand(context_, "BRPOP file_queue %d", timeout_sec);
  if (!reply || reply->type == REDIS_REPLY_NIL) {
    if (reply)
      freeReplyObject(reply);
    return false;
  }

  if (reply->type == REDIS_REPLY_ARRAY && reply->elements == 2) {
    file_path = std::filesystem::path(reply->element[1]->str);
    freeReplyObject(reply);
    return true;
  }

  freeReplyObject(reply);
  return false;
}