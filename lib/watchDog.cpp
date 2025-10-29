/*
 *I don't know how threads work, so I might come back to see how they work.
 *Right now you have to manually call the checkfile function to see if any changes have occured
 */

#include "watchDog.h"

namespace fs = std::filesystem; //makes it easier to read
void WatchDog::start() {
    try {
        // Try to get the last write time directly
        lastWriteTime = fs::last_write_time(path);

        // If successful, start watching
        watching = true;
        hasInitialTime = true;
        std::cout << "WatchDog: Started monitoring " << path << std::endl;
    }
    catch (const fs::filesystem_error& e) {
        // If the file doesn't exist or another filesystem error occurs
        watching = false;
        hasInitialTime = false;
        std::cerr << "WatchDog: Cannot start. File error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        // Catch any other unexpected errors
        watching = false;
        hasInitialTime = false;
        std::cerr << "WatchDog: Unexpected error: " << e.what() << std::endl;
    }
}
void WatchDog::stop(){
    watching = false;
}
//this is currently just telling you when the file is edited, the exact time its edited
bool WatchDog::checkFile() {
    if (!watching) return false;

    try {
        // Try to get last write time
        fs::file_time_type currentWriteTime = fs::last_write_time(path);

        // File was just created
        if (!hasInitialTime) {
            lastWriteTime = currentWriteTime;
            hasInitialTime = true;
            std::cout << "WatchDog: File created: " << path << std::endl;
            return true;
        }

        // File modified
        if (currentWriteTime != lastWriteTime) {
            lastWriteTime = currentWriteTime;
            std::cout << "WatchDog: File modified at "
                      << timePointToString(lastWriteTime) << std::endl;
            return true;
        }

    } catch (const fs::filesystem_error& e) {
        // File deleted, inaccessible, or path invalid
        if (hasInitialTime) {
            std::cout << "WatchDog: File deleted or inaccessible: " << path << std::endl;
            hasInitialTime = false;
            return true;
        }
        return false;

    } catch (const std::exception& e) {
        std::cerr << "WatchDog: Unexpected error checking file: " << e.what() << std::endl;
        return false;
    }

    // No change
    return false;
}

std::string WatchDog::timePointToString(const fs::file_time_type& timePoint){
    /*
     * https://en.cppreference.com/w/cpp/chrono.html
     * std::chrono -> name space, system_clock -> c++ clock(computers clock),
     * ime_point -> exact instant on system clock, used to cast
     * Comments: I had to use a lot of sources and googling to get this work
     *           We can get rid of this or document my troubles, because tbh this time stuff confuses me
     *
     * std::chrono::system_clock::time_point systemTimePoint =
     * std::chrono::clock_cast<std::chrono::system_clock>(timePoint);
     * This solutions doesn't work as it only works with C++ 20 only :(
     */

    //Convert std::filesystem::file_time_type (timePoint) to std::chrono::system_clock::time_point 

    /* 
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
    *   timePoint - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
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
            timePoint - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
    
    //converts to seconds
    std::time_t timeInSeconds = std::chrono::system_clock::to_time_t(systemTimePoint);

    //Converts to local time, built in function
    std::tm localTime = *std::localtime(&timeInSeconds);

    // Format the time into a string using strftime
    //https://en.cppreference.com/w/cpp/chrono/c/strftime.html
    char buffer[20]; 
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);

    return std::string(buffer);
}