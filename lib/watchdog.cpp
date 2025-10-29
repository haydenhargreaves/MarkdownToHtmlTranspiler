#include "watchdog.h"
#include <chrono>
#include <stdexcept>
#include <thread>

namespace fs = std::filesystem;
using namespace std::chrono;

void Watchdog::Start(std::function<void()> callback) {
  // checks if file exist
  if (!fs::exists(this->path))
    throw std::runtime_error("File does not exist.");

  // Loop forever and check the file.
  while (true) {
    CheckFile(callback);
    std::this_thread::sleep_for(this->POLLING_INTERVAL);
  }
}

void Watchdog::CheckFile(std::function<void()> callback) {
  // LONG STORY SHORT:
  // When a file is written to, it aquires an OS lock, which means our program
  // cannot access it. So when we request, it fails. So we should basically just
  // keep trying until we get access.
  //
  // After implementing some retry logic, I was unable to replicate the error.

  for (int attempt = 0; attempt < this->MAX_RETRIES; ++attempt) {
    try {
      // Checking if file was deleted
      if (!fs::exists(this->path))
        throw std::runtime_error(
            "File could not be found. Maybe it was deleted?");

      // Built in function with file system to check last write time
      fs::file_time_type currentWriteTime = fs::last_write_time(path);

      // File modified
      if (currentWriteTime != this->last_write_time) {
        this->last_write_time = currentWriteTime;

        time_point before = high_resolution_clock::now();
        callback();
        time_point after = high_resolution_clock::now();

        duration dur = after - before;
        long ms = std::chrono::duration_cast<milliseconds>(dur).count();
        long us = std::chrono::duration_cast<microseconds>(dur).count();

        if (ms > 0) {
          std::cout << std::endl
            << "Recompiled in \033[36m" << ms << "ms\033[0m" << std::endl;
        } else {
          std::cout << std::endl
            << "Recompiled in \033[36m" << us << "μs\033[0m" << std::endl;
        }
      }
    } catch (const std::exception &ex) {
      // On last attempt, bubble the error outward
      if (attempt == this->MAX_RETRIES - 1)
        throw std::runtime_error("Watchdog failed after multiple retries: " +
                                 std::string(ex.what()));

      // Wait and then try again
      std::this_thread::sleep_for(this->RETRY_DELAY);
    }
  }
}

std::string Watchdog::TimePointToString(const fs::file_time_type &timePoint) {
  system_clock::time_point systemTimePoint =
      time_point_cast<system_clock::duration>(
          timePoint - fs::file_time_type::clock::now() + system_clock::now());

  // Converts to seconds
  std::time_t timeInSeconds = system_clock::to_time_t(systemTimePoint);

  // Converts to local time, built in function
  std::tm localTime = *std::localtime(&timeInSeconds);

  // Format the time into a string using strftime
  char buffer[20];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);

  return std::string(buffer);
}
