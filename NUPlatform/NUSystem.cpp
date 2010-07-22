/*! @file Actuators.cpp
    @brief Partial implementation of base actuator class

    @author Jason Kulk
 
  Copyright (c) 2009 Jason Kulk
 
 This file is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This file is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with NUbot.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "NUSystem.h"
#include "Infrastructure/NUSensorsData/NUSensorsData.h"
#include "Infrastructure/NUActionatorsData/NUActionatorsData.h"

#include "nubotdataconfig.h"
#include "Motion/Tools/MotionFileTools.h"

#include "debug.h"
#include "debugverbositynusystem.h"
#include "targetconfig.h"

#include <unistd.h>
#ifdef TARGET_OS_IS_WINDOWS
	#include <windows.h>
#endif

#include <sstream>
#include <cstring>
#include <locale>
#include <memory>
#ifndef WIN32
    #include <sys/ioctl.h>
    #include <sys/socket.h>
    #include <net/if.h>
    #include <netinet/in.h>
#endif

using namespace std;

NUSystem* System = NULL;

#ifdef __NU_SYSTEM_CLOCK_GETTIME
    struct timespec NUSystem::m_gettime_starttime;            //!< the program's start time according to gettime()
    struct timespec NUSystem::m_gettimefast_starttime;        //!< the program's start time according to the fast verion of gettime()

    #ifndef CLOCK_REALTIME_FAST                             // not all distros will have the CLOCK_REALTIME_FAST, fudge it if they don't
        #define CLOCK_REALTIME_FAST CLOCK_REALTIME
    #endif
#else
    using namespace boost::posix_time;
    ptime NUSystem::m_microsec_starttime = microsec_clock::local_time();
#endif
long double NUSystem::m_time_offset = 0;

NUSystem::NUSystem()
{
    #if DEBUG_NUSYSTEM_VERBOSITY > 4
        debug << "NUSystem::NUSystem()" << endl;
    #endif 
    System = this;
        
    #ifdef __NU_SYSTEM_CLOCK_GETTIME
        clock_gettime(CLOCK_REALTIME, &m_gettime_starttime);
        clock_gettime(CLOCK_REALTIME_FAST, &m_gettimefast_starttime); 
    #endif
    // I need to get the offset between the unix timestamp and the time since start
    m_time_offset = getPosixTimeStamp();
}

NUSystem::~NUSystem()
{
#if DEBUG_NUSYSTEM_VERBOSITY > 4
    debug << "NUSystem::~NUSystem()" << endl;
#endif 
}

/*! @brief Returns a timestamp in milliseconds since the epoch (ie. The UNIX timestamp)
 
    This function can be slow on some platforms. Only use this if you need the best precision time
    the platform supports. Typically, the precision will be in the nanosecond range.
 
    In simulators, the timestamp will be fudged so that the timestamp returned will be the unix
    timestamp at the start of the simulation + the *simulated* time
 */
long double NUSystem::getPosixTimeStamp()
{
    static long double timeinmilliseconds;
#ifdef __NU_SYSTEM_CLOCK_GETTIME
    static struct timespec timenow;
    clock_gettime(CLOCK_REALTIME_FAST, &timenow);
    timeinmilliseconds = timenow.tv_nsec/1e6 + timenow.tv_sec*1e3;
#else
    timeinmilliseconds = (microsec_clock::universal_time() - from_time_t(0)).total_nanoseconds()/1e6;
#endif
    return timeinmilliseconds;
}

/*! @brief Returns the time in milliseconds since the start of the program
 
    This function is simulator safe, in that if you pause the simulator the time returned by this
    function will not increase.
 */
double NUSystem::getTime()
{
    return getRealTime();       // the default implementation is to just return the actual time
}

/*! @brief Returns the time in milliseconds since the start of the program
 
 This function is simulator safe, in that if you pause the simulator the time returned by this
 function will not increase.
 
 This function will be faster than getTime() on some platforms. Use this function if you don't need
 accuracy better than +/- 10ms.
 */
double NUSystem::getTimeFast()
{
    return getRealTimeFast();       // the default implementation is to just return the actual time
}

/*! @brief Returns the time offset such that timesincestart = timestamp - offset and timestamp = timesincestart + offset
 */
long double NUSystem::getTimeOffset()
{
    return m_time_offset;
}


/*! @brief Returns the real time in milliseconds since the start of the program.
 
    This always returns the real time. If you pause time (in a simulator) this clock keeps running
 */
double NUSystem::getRealTime()
{
    static double timeinmilliseconds;
#ifdef __NU_SYSTEM_CLOCK_GETTIME
    static struct timespec timenow;
    clock_gettime(CLOCK_REALTIME, &timenow);
    timeinmilliseconds = (timenow.tv_nsec - m_gettime_starttime.tv_nsec)/1e6 + (timenow.tv_sec - m_gettime_starttime.tv_sec)*1e3;
#else
    static ptime timenow;
    timenow = microsec_clock::local_time();
    timeinmilliseconds = (timenow - m_microsec_starttime).total_nanoseconds()/1e6;
#endif
    return timeinmilliseconds;
}

/*! @brief Returns the real time in milliseconds since the start of the program.
 
    This function will be faster than getTime() on some platforms. Use this function if you don't need
    accuracy better than +/- 10ms.
 
    This always returns the real time. If you pause time (in a simulator) this clock keeps running
 */
double NUSystem::getRealTimeFast()
{
    static double timeinmilliseconds;
#ifdef __NU_SYSTEM_CLOCK_GETTIME
    static struct timespec timenow;
    clock_gettime(CLOCK_REALTIME_FAST, &timenow);
    timeinmilliseconds = (timenow.tv_nsec - m_gettimefast_starttime.tv_nsec)/1e6 + (timenow.tv_sec - m_gettimefast_starttime.tv_sec)*1e3;
#else
    timeinmilliseconds = getRealTime();
#endif
    return timeinmilliseconds;
}

/*! @brief Returns the the time in milliseconds spent in this process since some arbitary point in the past. 
 
    Each process will need to be responsible for keeping tracking of their start time; I can't do it here
    because I can only save the starting process time where this class was created. However, usually
    the underlying implementation starts the clock from zero when the thread is created.
 
    This function might return the thread time if the platform doesn't support the process times. 
 */
double NUSystem::getProcessTime()
{
    static double timeinmilliseconds;
#ifdef __NU_SYSTEM_CLOCK_GETTIME
    static struct timespec timenow;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timenow);
    timeinmilliseconds = timenow.tv_nsec/1e6 + timenow.tv_sec*1e3;
#else
    timeinmilliseconds = 1e3*clock()/float(CLOCKS_PER_SEC);
#endif
    return timeinmilliseconds;
}

/*! @brief Returns the the time in milliseconds spent in this thread since the program started.  
 
    Each process will need to be responsible for keeping tracking of their start time; I can't do it here
    because I can only save the starting process time where this class was created. However, usually
    the underlying implementation starts the clock from zero when the thread is created.
 
    This function might return the process time if the platform doesn't support the thread times. 
 */
double NUSystem::getThreadTime()
{
    static double timeinmilliseconds;
#ifdef __NU_SYSTEM_CLOCK_GETTIME
    static struct timespec timenow;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timenow);
    timeinmilliseconds = timenow.tv_nsec/1e6 + timenow.tv_sec*1e3;
#else
    timeinmilliseconds = 1e3*clock()/float(CLOCKS_PER_SEC);
#endif
    return timeinmilliseconds;
}

void NUSystem::msleep(double milliseconds)
{
    #ifdef __NU_PERIODIC_CLOCK_NANOSLEEP
        struct timespec sleeptime;
        sleeptime.tv_sec = static_cast<int> (milliseconds/1e3);
        sleeptime.tv_nsec = 1e6*milliseconds - sleeptime.tv_sec*1e9;
        clock_nanosleep(CLOCK_REALTIME, 0, &sleeptime, NULL);  
    #else
        #ifdef TARGET_OS_IS_WINDOWS
            Sleep(DWORD(milliseconds));
        #else
            if (milliseconds <= 1000)
                usleep(static_cast<int> (milliseconds*1e3));
            else
            {
                sleep(milliseconds/1e3);
            }
        #endif
    #endif
}

/*! @brief Display the current state of the battery
    @param data a pointer to the shared sensor data object (it contains the battery values)
    @param actions a pointer to the shared actionator object
 */
void NUSystem::displayBatteryState(NUSensorsData* data, NUActionatorsData* actions)
{
    // by default there is no way to display such information!
}

/*! @brief Displays that a team packet has been successfully received
    @param actions a pointer to the shared actionator object
 */
void NUSystem::displayTeamPacketReceived(NUActionatorsData* actions)
{
    // by default there is no way to display such information!    
}

/*! @brief Displays that a team packet has been successfully sent
    @param actions a pointer to the shared actionator object
 */
void NUSystem::displayTeamPacketSent(NUActionatorsData* actions)
{
    // by default there is no way to display such information!
}

/*! @brief Displays that a game packet has been successfully received
    @param actions a pointer to the shared actionator object
 */
void NUSystem::displayGamePacketReceived(NUActionatorsData* actions)
{
    // by default there is no way to display such information!
}

/*! @brief Displays that a packet has been successfully received
    @param actions a pointer to the shared actionator object
 */
void NUSystem::displayOtherPacketReceived(NUActionatorsData* actions)
{
    // by default there is no way to display such information!
}

/*! @brief Display some sign that a vision frame has been dropped
    @param actions a pointer to the shared actionator object
 */
void NUSystem::displayVisionFrameDrop(NUActionatorsData* actions)
{
    // by default there is no way to display such information!
}

/*! @brief Loads the robot's idenity from system commands and files
 
    The m_name, m_robot_number, m_team_number, wired MAC address. By default the hostname is used to set m_name, m_robot_number; m_team_number is loaded from a file,
    and the MAC address is obtained through ioctrl.
 
    The function is virtual so special cases can implemented by children of the NUSystem.
 */
void NUSystem::loadRobotIdentity()
{
    char hostname[255];                                         // we store the name in /etc/hostname so the network name and robot name are the same
    gethostname(hostname, 255);
    m_name = string(hostname);  
    
    int robot_number_start = 0;
    for (size_t i=m_name.size()-1; i>0; i--)
    {
        if (isalpha(m_name[i]))
        {   // find the last alphabet, everything after that is the robot number
            robot_number_start = i;
            break;
        }
    }
    m_robot_number = atoi(m_name.substr(robot_number_start).c_str());           // the name will be nubotXX where XX is the player number
    
    ifstream teamfile((string(CONFIG_DIR) + string("Team.cfg")).c_str());      // the team number is stored in a file
    if (teamfile.is_open())
        m_team_number = MotionFileTools::toFloat(teamfile);
    else
    {
        errorlog << "NUSystem::loadRobotIdentity(). Unable to load Team.cfg" << endl;
        m_team_number = 0;
    }
    teamfile.close();
    
    #ifndef TARGET_OS_IS_WINDOWS
        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd == -1) 
            errorlog << "NUSystem::loadRobotIdentity(). Unable to open socket, so unable to get mac address." << endl;
        else
        {
            struct ifreq ifr;
            memset(&ifr, 0, sizeof(ifr));
            #ifdef TARGET_OS_IS_DARWIN
                strcpy(ifr.ifr_name, "en0");
                if (ioctl(sockfd, SIOCGIFMAC, &ifr) != -1)
                {
                    // doesnt work on OS-X :(
                }
            #else
                strcpy(ifr.ifr_name, "eth0");
                if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) != -1)
                {
                    stringstream ss;
                    ss << hex << setw(2) << setfill('0');
                    for (int i=0; i<5; i++)
                        ss << setw(2) << setfill('0') << (int) (unsigned char) ifr.ifr_hwaddr.sa_data[i] << "-";
                    ss << setw(2) << setfill('0') << (int) (unsigned char) ifr.ifr_hwaddr.sa_data[5];
                    m_mac_address = ss.str();
                }
            #endif
            close(sockfd);
        }
    #endif
}

/*! @brief Returns the name of the robot */
string& NUSystem::getName()
{
    return m_name;
}

/*! @brief Returns the robot's number */
int NUSystem::getRobotNumber()
{
    return m_robot_number;
}

/*! @brief Returns the robot's team number */
int NUSystem::getTeamNumber()
{
    return m_team_number;
}

/*! @brief Returns the robot's mac address. The address is the wired, and the wireless only if the system has no wired. */
string& NUSystem::getMacAddress()
{
    return m_mac_address;
}

