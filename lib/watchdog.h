/**
 * @file watchdog.h
 * @brief file watcher
 * @author Preston Shultz
 * Sources:
 *  https://en.cppreference.com/w/cpp/filesystem.html
 *  https://en.cppreference.com/w/cpp/chrono.html
 *  Format the time into a string using strftime
 *  https://en.cppreference.com/w/cpp/chrono/c/strftime.html
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <chrono>     //makes timestamps easier
#include <ctime>      //Convert time_t to std::tm for local time
#include <filesystem> //allow access to files platform independent
#include <iomanip>    //Formats std::tim into "YYYY-MM-DD HH-MM-SS"
#include <iostream>
#include <string>

/**
 * @brief watchdog class.
 *
 * Checks if a file is modified and displays date modified
 *
 * @author Preston Shultz (shultzp1@my.erau.edu)
 */
class Watchdog {
protected:
  /**
   * @brief watchdog class.
   *
   * Checks if a file has been modified
   *
   * @author Preston Shultz (shultzp1@my.erau.edu)
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  void CheckFile();

public:
  Watchdog(const std::string &path) : path(path) {}

  /**
   * @brief watchdog class.
   *
   * Starts the watchdog to check of a file is modified
   *
   * @author Preston Shultz (shultzp1@my.erau.edu)
   */
  void Start();

  /**
   * @brief watchdog class.
   *
   * Converts time point into a string
   *
   * Details below:
   *
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
   *
   * @author Preston Shultz (shultzp1@my.erau.edu)
   */
  static std::string
  TimePointToString(const std::filesystem::file_time_type &timePoint);

private:
  std::string path;
  std::filesystem::file_time_type last_write_time;

  // Timing values for the watchdog
  const std::chrono::milliseconds POLLING_INTERVAL =
      std::chrono::milliseconds(100);
  const int MAX_RETRIES = 20;
  const std::chrono::milliseconds RETRY_DELAY = std::chrono::milliseconds(20);
};

#endif
