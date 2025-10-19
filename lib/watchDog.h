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

#include <iostream>
#include <string> 
#include <filesystem> //allow access to files platform independent
#include <chrono>  //makes timestamps easier
#include <ctime> //Convert time_t to std::tm for local time
#include <iomanip> //Formats std::tim into "YYYY-MM-DD HH-MM-SS"


/**
 * @brief watchdog class.
 *
 * Checks if a file is modified and displays date modified
 *
 * @author Preston Shultz (shultzp1@my.erau.edu)
 */
class Watchdog{
public:
    Watchdog(const std::string& path) : 
    path(path), watching(false), has_initial_time(false) {}

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
     * Stops the watchdog
     *
     * @author Preston Shultz (shultzp1@my.erau.edu)
     */
    void Stop() {watching = false;} //Disable

    /**
     * @brief watchdog class.
     *
     * Checks if a file has been modified
     *
     * @author Preston Shultz (shultzp1@my.erau.edu)
     */
    bool CheckFile(); 

    /**
     * @brief watchdog class.
     *
     * Returns files last modified date
     *
     * @author Preston Shultz (shultzp1@my.erau.edu)
     */
    std::filesystem::file_time_type GetLastWriteTime() 
    const {return last_write_time;}

    /**
     * @brief watchdog class.
     *
     * Converts time point into a string
     *
     * @author Preston Shultz (shultzp1@my.erau.edu)
     */
    static std::string TimePointToString(const std::filesystem::file_time_type& timePoint);

    /**
     * @brief watchdog class.
     *
     * Returns if the watchDog is on
     *
     * @author Preston Shultz (shultzp1@my.erau.edu)
     */
    bool IsWatching() const{return watching;}

private:
    std::string path; //file path
    bool watching; //Is watchdog on?
    std::filesystem::file_time_type last_write_time; 
    bool has_initial_time; //checks if initial time is given
};

#endif