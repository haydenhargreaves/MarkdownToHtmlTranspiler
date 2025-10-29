#include "watchdog.h"

namespace fs = std::filesystem; // makes it easier to read

void Watchdog::Start() {
  // checks if file exist
  if (!fs::exists(path)) {
    // returns and sets parameters to false if file doesnt exist
    this->watching = false;
    this->has_initial_time = false;
    std::cout << "Watchdog: File does not exists: " << path << std::endl;
    return;
  }
  // grabs intial write time
  this->last_write_time = fs::last_write_time(path);
  this->watching = true;
  this->has_initial_time = true;
  std::cout << "Watchdog: Started" << std::endl;
}

bool Watchdog::CheckFile() {
  // If not watching returns false
  if (!watching)
    return false;

  try {
    // Checking if file was deleted
    if (!fs::exists(path)) {
      if (this->has_initial_time) {
        std::cout << "Watchdog: File was delete: " << path << std::endl;
        this->has_initial_time = false;
        return true;
      }
      return false;
    }

    // Built in function with file system to check last write tim
    fs::file_time_type currentWriteTime = fs::last_write_time(path);

    // File was just created
    if (!this->has_initial_time) {
      this->last_write_time = currentWriteTime;
      this->has_initial_time = true;
      std::cout << "Watchdog: File created: " << path << std::endl;
      return true;
    }

    // File modified
    if (currentWriteTime != this->last_write_time) {
      this->last_write_time = currentWriteTime;
      std::cout << "Watchdog: File modifed at "
                << TimePointToString(this->last_write_time) << std::endl;
      return true;
    }
  } catch (const fs::filesystem_error &e) {
    // File deleted, inaccessible, or path invalid
    if (this->has_initial_time) {
      std::cout << "Watchdog: File deleted or inaccessible: " << path
                << std::endl;
      this->has_initial_time = false;
      return true;
    }
    return false;
  } catch (const std::exception &e) {
    std::cerr << "Watchdog: Unexpected error checking file: " << e.what()
              << std::endl;
    return false;
  }

  // No change
  return false;
}

std::string Watchdog::TimePointToString(const fs::file_time_type &timePoint) {
  /**
   * Step 1: timePoint
   *   - This is the last write time of the file, returned by std::filesystem.
   *   - Its clock is platform-dependent (filesystem clock).
   *
   * Step 2: fs::file_time_type::clock::now()
   *   - Current time according to the filesystem clock.
   *
   * Step 3: std::chrono::system_clock::now()
   *   - Current time according to the system clock (standard C++ clock).
   *
   * Conversion formula:
   *   timePoint - fs::file_time_type::clock::now() +
   * std::chrono::system_clock::now()
   *
   * Explanation:
   *   a) timePoint - fs::file_time_type::clock::now()
   *      - Calculates the duration between the file's last write time and "now"
   *        according to the filesystem clock.
   *   b) + std::chrono::system_clock::now()
   *      - Shifts that duration to align with the system clock timeline.
   *   c) std::chrono::time_point_cast<std::chrono::system_clock::duration>(...)
   *      - Ensures the resulting time_point uses the correct duration type
   *        for std::chrono::system_clock.
   *
   * Result:
   *   - systemTime is a std::chrono::system_clock::time_point representing
   *     the same instant as timePoint, but compatible with system_clock.
   */
  std::chrono::system_clock::time_point systemTimePoint =
      std::chrono::time_point_cast<std::chrono::system_clock::duration>(
          timePoint - fs::file_time_type::clock::now() +
          std::chrono::system_clock::now());

  // Converts to seconds
  std::time_t timeInSeconds =
      std::chrono::system_clock::to_time_t(systemTimePoint);

  // Converts to local time, built in function
  std::tm localTime = *std::localtime(&timeInSeconds);

  // Format the time into a string using strftime
  char buffer[20];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);

  return std::string(buffer);
}
