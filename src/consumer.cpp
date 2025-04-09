#include "config.h"
#include "redis_queue.h"
#include <atomic>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <zlib.h>

namespace fs = std::filesystem;

void compress_file(const fs::path &file_path, const fs::path &out_dir) {
  std::cout << "DEBUG: output dir: " << out_dir
            << " exists: " << fs::exists(out_dir) << std::endl;
  try {
    if (!fs::exists(out_dir)) {
      fs::create_directories(out_dir);
    }

    std::ifstream input(file_path, std::ios::binary);
    if (!input) {
      std::cerr << "Failed to open input file: " << file_path << std::endl;
      return;
    }

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

    fs::path out_path = out_dir / file_path.filename().replace_extension(".z");

    std::ofstream output(out_path, std::ios::binary);
    if (!output) {
      std::cerr << "Failed to create output file: " << out_path << std::endl;
      return;
    }

    output.write(reinterpret_cast<char *>(compressed_data.data()),
                 compressed_size);
    std::cout << "Successfully compressed: " << file_path << " -> " << out_path
              << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Error processing " << file_path << ": " << e.what()
              << std::endl;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <output_directory>" << std::endl;
    return 1;
  }

  fs::path out_dir(argv[1]);

  try {
    RedisQueue queue = RedisQueue::createFromConfig();
    std::atomic<bool> stop_flag{false};

    auto worker = [&queue, &stop_flag, &out_dir]() {
      fs::path file_path;
      while (!stop_flag) {
        if (queue.try_pop(file_path, 1)) {
          if (!file_path.empty()) {
            compress_file(file_path, out_dir);
          }
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
    std::cout << "Output directory: " << fs::absolute(out_dir) << std::endl;
    std::cout << "Press Enter to stop..." << std::endl;
    std::cin.get();

    stop_flag = true;
    for (auto &worker : workers) {
      if (worker.joinable())
        worker.join();
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}