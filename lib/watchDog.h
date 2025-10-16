#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <iostream>
#include <string>
/*
 *https://en.cppreference.com/w/cpp/filesystem.html
 *allow access to files platform independent
 */
#include <filesystem> 
/*
 *https://en.cppreference.com/w/cpp/chrono.html
 *makes timestamps easier
 *how to do lots of research to get this to work
 */
#include <chrono> 
/**
 * @brief watchdog class.
 *
 * Checks if a file is modified and displays date modified
 *
 * @author Preston Shultz (shultzp1@my.erau.edu)
 */
#include <ctime> //Convert time_t to std::tm for local time
#include <iomanip> //Formats std::tim into "YYYY-MM-DD HH-MM-SS"


class WatchDog{
    public:
        WatchDog(const std::string& path) : 
        path(path), watching(false), hasInitialTime(false) {}

        /**
         * @brief watchdog class.
         *
         * Starts the watchdog to check of a file is modified
         *
         * @author Preston Shultz (shultzp1@my.erau.edu)
         */
        void start();
        /**
         * @brief watchdog class.
         *
         * Stops the watchdog
         *
         * @author Preston Shultz (shultzp1@my.erau.edu)
         */
        void stop(); //Disable

        /**
         * @brief watchdog class.
         *
         * Checks if a file has been modified
         *
         * @author Preston Shultz (shultzp1@my.erau.edu)
         */
        bool checkFile(); 

        /**
         * @brief watchdog class.
         *
         * Returns files last modified date
         *
         * @author Preston Shultz (shultzp1@my.erau.edu)
         */
        std::filesystem::file_time_type getLastWriteTime() 
        const {return lastWriteTime;}

        /**
         * @brief watchdog class.
         *
         * Converts time point into a string
         *
         * @author Preston Shultz (shultzp1@my.erau.edu)
         */
        static std::string timePointToString(const std::filesystem::file_time_type& timePoint);

        /**
         * @brief watchdog class.
         *
         * Returns if the watchDog is on
         *
         * @author Preston Shultz (shultzp1@my.erau.edu)
         */
        bool isWatching() const{return watching;}

    private:
        std::string path; //file path
        bool watching; //Is watchdog on?
        std::filesystem::file_time_type lastWriteTime; 
        bool hasInitialTime; //checks if initial time is given
};

#endif