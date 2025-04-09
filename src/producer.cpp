#include "redis_queue.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <directory>" << std::endl;
    return 1;
  }

  fs::path dir_path(argv[1]);
  if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
    std::cerr << "Invalid directory path: " << dir_path << std::endl;
    return 1;
  }

  try {
    RedisQueue queue = RedisQueue::createFromConfig(); // Укажите ваш Redis host

    for (const auto &entry : fs::recursive_directory_iterator(dir_path)) {
      if (entry.is_regular_file()) {
        queue.push(entry.path());
        std::cout << "Added to queue: " << entry.path() << std::endl;
      }
    }

    std::cout << "All files added to queue" << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}