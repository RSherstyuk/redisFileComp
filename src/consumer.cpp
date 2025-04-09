#include "redis_queue.h"
#include <atomic>
#include <config.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <zlib.h>

namespace fs = std::filesystem;

void compress_file(const fs::path &file_path) {
  try {
    std::ifstream input(file_path, std::ios::binary);
    if (!input)
      return;

    std::vector<char> input_data((std::istreambuf_iterator<char>(input)),
                                 std::istreambuf_iterator<char>());

    uLongf compressed_size = compressBound(input_data.size());
    std::vector<Bytef> compressed_data(compressed_size);

    if (compress(compressed_data.data(), &compressed_size,
                 reinterpret_cast<Bytef *>(input_data.data()),
                 input_data.size()) != Z_OK) {
      std::cerr << "Compression failed for: " << file_path << std::endl;
      return;
    }

    fs::path output_path =
        file_path.parent_path() / (file_path.filename().string() + ".z");
    std::ofstream output(output_path, std::ios::binary);
    if (!output)
      return;

    output.write(reinterpret_cast<char *>(compressed_data.data()),
                 compressed_size);
    std::cout << "Compressed: " << file_path << " -> " << output_path
              << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error processing " << file_path << ": " << e.what()
              << std::endl;
  }
}

int main() {
  try {
    RedisQueue queue = RedisQueue::createFromConfig();
    std::atomic<bool> stop_flag{false};

    auto worker = [&queue, &stop_flag]() {
      fs::path file_path;
      while (!stop_flag) {
        if (queue.try_pop(file_path, 10)) {
          compress_file(file_path);
        }
      }
    };

    size_t thread_count = std::thread::hardware_concurrency();
    if (thread_count == 0)
      thread_count = 2;

    std::vector<std::thread> workers;
    for (size_t i = 0; i < thread_count; ++i) {
      workers.emplace_back(worker);
    }

    std::cout << "Consumer started with " << thread_count << " workers"
              << std::endl;
    std::cout << "Press Enter to stop..." << std::endl;
    std::cin.get();

    stop_flag = true;
    for (auto &worker : workers) {
      worker.join();
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}