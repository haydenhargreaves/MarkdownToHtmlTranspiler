#include "watchdog.h"

namespace fs = std::filesystem; //makes it easier to read

void Watchdog::Start()
{
    //Checks if file exist
    if(!fs::exists(path))
    {
        //Returns and sets parameters to false if file doesnt exist
        watching = false;
        has_initial_time = false;
        std::cout << "WatchDog: File does not exists: " << path << std::endl;
        return;
    }
    //Grabs intial write time
    last_write_time = fs::last_write_time(path);
    //Sets watchdog
    watching = true;
    has_initial_time = true;
    std::cout << "WatchDog: Started" << std::endl;
}

bool Watchdog::CheckFile(){
    //If not watching returns false
    if (!watching) return false;

    //Checking if file was deleted
    if(!fs::exists(path))
    {
        if (has_initial_time) {
            std::cout << "WatchDog: File was delete: " << path << std::endl;
            has_initial_time = false;
            return true;
        }
        return false;
    }
    
    //Built in function with file system to check last write time
    fs::file_time_type currentWriteTime = fs::last_write_time(path);

    //File was just created
    if(!has_initial_time)
    {
        last_write_time = currentWriteTime;
        has_initial_time = true;
        std::cout << "WatchDog: File created: " << path << std::endl;
        return true;
    }

    //File modified
    if (currentWriteTime != last_write_time)
    {
        last_write_time = currentWriteTime;
        std::cout << "WatchDog: File modifed at " 
                  << TimePointToString(last_write_time) << std::endl;
        return true;      
    }

    //No change
    return false;
}

std::string Watchdog::TimePointToString(const fs::file_time_type& timePoint){
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
    
    //Converts to seconds
    std::time_t timeInSeconds = std::chrono::system_clock::to_time_t(systemTimePoint);

    //Converts to local time, built in function
    std::tm localTime = *std::localtime(&timeInSeconds);

    // Format the time into a string using strftime
    char buffer[20]; 
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);

    return std::string(buffer);
}