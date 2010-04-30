/*! @file NUbot.cpp
    @brief Implementation of top-level NUbot class

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

#include "debugverbositynubot.h"
#include "debug.h"
#include "NUPlatform/NUActionators/NUSounds.h"

#include "NUbot.h"
#include "Behaviour/Jobs.h"
#include "GameController/GameInformation.h"
#include "Behaviour/TeamInformation.h"

#if defined(USE_VISION) or defined(USE_LOCALISATION) or defined(USE_BEHAVIOUR) or defined(USE_MOTION)
    #include "NUbot/SeeThinkThread.h"
#endif
#include "NUbot/SenseMoveThread.h"
#include "NUbot/WatchDogThread.h"

#if defined(TARGET_IS_NAOWEBOTS)
    #include "NUPlatform/Platforms/NAOWebots/NAOWebotsPlatform.h"
    #include "NUPlatform/Platforms/NAOWebots/NAOWebotsIO.h"
#elif defined(TARGET_IS_NAO)
    #include "NUPlatform/Platforms/NAO/NAOPlatform.h"
    #include "NUPlatform/Platforms/NAO/NAOIO.h"
#elif defined(TARGET_IS_CYCLOID)
    #include "NUPlatform/Platforms/Cycloid/CycloidPlatform.h"
    #include "NUPlatform/Platforms/Cycloid/CycloidIO.h"
#elif defined(TARGET_IS_NUVIEW)
    #error You should not be compiling NUbot.cpp when targeting NUview, you should use the virtualNUbot.
#else
    #error There is no platform (TARGET_IS_${}) defined
#endif

#include <time.h>
#include <signal.h>
#include <string>
#include <sstream>
#include <unistd.h>

#ifndef TARGET_OS_IS_WINDOWS
    #include <errno.h>
#endif
#ifndef TARGET_OS_IS_WINDOWS
    #include <execinfo.h>
#endif

NUbot* NUbot::m_this = NULL;

/*! @brief Constructor for the nubot
    
    The parameters are for command line arguements. Webots gives the binary arguements which tell us the 
    robot's number. Consequently, these args are passed down to the webots platform.
 
    @param argc the number of command line arguements
    @param *argv[] the array of command line arguements
 */
NUbot::NUbot(int argc, const char *argv[])
{
    NUbot::m_this = this;
    connectErrorHandling();
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::NUbot(). Constructing NUPlatform." << endl;
    #endif
    
    // --------------------------------- construct the platform
    #if defined(TARGET_IS_NAOWEBOTS)
        m_platform = new NAOWebotsPlatform(argc, argv);
    #elif defined(TARGET_IS_NAO)
        m_platform = new NAOPlatform();
    #elif defined(TARGET_IS_CYCLOID)
        m_platform = new CycloidPlatform();
    #endif
    
    // --------------------------------- construct the public storage
    #ifdef USE_VISION
        Image = NULL;
    #endif
    SensorData = m_platform->sensors->getData();
    Actions = m_platform->actionators->getActions();
    Jobs = new JobList();
    GameInfo = new GameInformation(m_platform->getPlayerNumber(), m_platform->getTeamNumber());
    TeamInfo = new TeamInformation();

    // --------------------------------- construct the io
    #if defined(TARGET_IS_NAOWEBOTS)
        m_io = new NAOWebotsIO(m_platform->getPlayerNumber(), m_platform->getTeamNumber(), this);
    #elif defined(TARGET_IS_NAO)
        m_io = new NAOIO(this);
    #elif defined(TARGET_IS_CYCLOID)
        m_io = new CycloidIO(this);
    #endif
    
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::NUbot(). Constructing modules." << endl;
    #endif
    
    // --------------------------------- construct each enabled module 
    #ifdef USE_VISION
        m_vision = new Vision();
    #endif
    
    #ifdef USE_LOCALISATION
        //m_localisation = new Localisation();
    #endif
    
    #ifdef USE_BEHAVIOUR
        m_behaviour = new Behaviour();
    #endif
    
    #ifdef USE_MOTION
        m_motion = new NUMotion();
    #endif
    
    createThreads();
    
#if DEBUG_NUBOT_VERBOSITY > 0
    debug << "NUbot::NUbot(). Finished." << endl;
#endif
}

/*! @brief Connects error and signal handlers with the appropriate functions
 */
void NUbot::connectErrorHandling()
{
    #ifndef TARGET_OS_IS_WINDOWS
        struct sigaction newaction, oldaction;
        newaction.sa_handler = segFaultHandler;
        
        sigaction(SIGSEGV, &newaction, &oldaction);     //!< @todo TODO. On my computer the segfault is not escalated. It should be....
    #endif
}

/*! @brief Create nubot's threads
 */
void NUbot::createThreads()
{
#if DEBUG_NUBOT_VERBOSITY > 1
    debug << "NUbot::createThreads(). Constructing threads." << endl;
#endif
    
    #if defined(USE_VISION) or defined(USE_LOCALISATION) or defined(USE_BEHAVIOUR) or defined(USE_MOTION)
        m_seethink_thread = new SeeThinkThread(this);
    #endif
        
    m_sensemove_thread = new SenseMoveThread(this);
    m_sensemove_thread->start();
    
    m_watchdog_thread = new WatchDogThread(this);
    m_watchdog_thread->start();
    
    #if defined(USE_VISION) or defined(USE_LOCALISATION) or defined(USE_BEHAVIOUR) or defined(USE_MOTION)
        m_seethink_thread->start();
    #endif

#if DEBUG_NUBOT_VERBOSITY > 1
    debug << "NUbot::createThreads(). Finished." << endl;
#endif
}

/*! @brief Destructor for the nubot
 */
NUbot::~NUbot()
{
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::~NUbot()." << endl;
    #endif
    
    vector<float> rgb(3,0);
    vector<vector<float> > allleds;
    allleds.push_back(rgb);
    allleds[0][0] = 0.0; allleds[0][1] = 0.25; allleds[0][2] = 0.0;
    NUbot::m_this->Actions->addLeds(NUActionatorsData::AllLeds, 0, allleds);
    
    #ifdef USE_MOTION
        float l_positions[] = {0, 0, 0, 1.41, -1.1, -0.65, 0, 1.41, 1.1, 0.65, 0, -1.0, 0, 2.16, 0, -1.22, 0.0, -1, 0, 2.16, 0, -1.22};
        vector<float> gains(NUbot::m_this->Actions->getNumberOfJoints(NUActionatorsData::AllJoints), 50);
        vector<float> velocities(NUbot::m_this->Actions->getNumberOfJoints(NUActionatorsData::AllJoints), 50);
        vector<float> positions(l_positions, l_positions + sizeof(l_positions)/sizeof(*l_positions));
        NUbot::m_this->Actions->addJointPositions(NUActionatorsData::AllJoints, nusystem->getTime() + 2000, positions, velocities, gains);
        NUbot::m_this->m_platform->actionators->process(NUbot::m_this->Actions);
        NUSystem::msleep(2000);
    #endif

    // --------------------------------- delete threads
    #if defined(USE_VISION) or defined(USE_LOCALISATION) or defined(USE_BEHAVIOUR) or defined(USE_MOTION)
        if (m_seethink_thread != NULL)
            delete m_seethink_thread;
    #endif
    if (m_sensemove_thread != NULL)
        delete m_sensemove_thread;
    if (m_watchdog_thread != NULL)
        delete m_watchdog_thread;
    
    
    // --------------------------------- delete modules
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::~NUbot(). Deleting Modules" << endl;
    #endif
    
    if (m_platform != NULL)
        delete m_platform;
    #ifdef USE_VISION
        if (m_vision != NULL)
            delete m_vision;
    #endif
    #ifdef USE_LOCALISATION
        //if (m_localisation != NULL)
            //delete m_localisation;
    #endif
    #ifdef USE_BEHAVIOUR
        if (m_behaviour != NULL)
            delete m_behaviour;
    #endif
    #ifdef USE_MOTION
        if (m_motion != NULL)
            delete m_motion;
    #endif
    if (m_io != NULL)
        delete m_io;
    
    // --------------------------------- delete public storage variables
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::~NUbot(). Deleting Public Storage" << endl;
    #endif
    
    #ifdef USE_VISION
        if (Image != NULL)
            delete Image;
    #endif
    if (SensorData != NULL)
        delete SensorData;
    if (Actions != NULL)
        delete Actions;
    if (Jobs != NULL)
        delete Jobs;
    if (GameInfo != NULL)
        delete GameInfo;
    if (TeamInfo != NULL)
        delete TeamInfo;
    
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::~NUbot(). Finished!" << endl;
    #endif
}

/*! @brief The nubot's main loop
    
    The nubot's main loop. This function will probably never return.
 
    The idea is to simply have 
    @verbatim
    NUbot* nubot = new NUbot(arc, argv);
    nubot->run();
    delete nubot;
    @endverbatim

 */
void NUbot::run()
{
#if defined(TARGET_IS_NAOWEBOTS)
    int count = 0;
    double previoussimtime;
    NAOWebotsPlatform* webots = (NAOWebotsPlatform*) m_platform;
    int timestep = webots->getBasicTimeStep();
    while (true)
    {
        previoussimtime = nusystem->getTime();
        webots->step(timestep);           // stepping the simulator generates new data to run motion, and vision data
        #if defined(USE_MOTION)
            m_sensemove_thread->startLoop();
        #endif
        
        #if defined(USE_VISION) or defined(USE_LOCALISATION) or defined(USE_BEHAVIOUR) or defined(USE_MOTION)
            if (count%2 == 0)           // depending on the selected frame rate vision might not need to be updated every simulation step
            {
                m_seethink_thread->startLoop();         
                m_seethink_thread->waitForLoopCompletion();
            }
        #endif
        
        #if defined(USE_MOTION)
            m_sensemove_thread->waitForLoopCompletion();
        #endif
        
        count++;
    };
#else
    #if !defined(USE_VISION)
        while (true)
        {
            periodicSleep(33);
            m_seethink_thread->startLoop();
            m_seethink_thread->waitForLoopCompletion();
        }
    #endif
#endif
}

void NUbot::periodicSleep(int period)
{
    static double starttime = nusystem->getTime();
    double timenow = nusystem->getTime();
    double requiredsleeptime = period - (timenow - starttime);
    if (requiredsleeptime > 0)
        NUSystem::msleep(requiredsleeptime);
    starttime = nusystem->getTime();
}

/*! @brief 'Handles' a segmentation fault; logs the backtrace to errorlog
 */
void NUbot::segFaultHandler(int value)
{
    #ifndef TARGET_OS_IS_WINDOWS
        errorlog << "SEGMENTATION FAULT. " << endl;
        debug << "SEGMENTATION FAULT. " << endl;
        void *array[10];
        size_t size;
        char **strings;
        size = backtrace(array, 10);
        strings = backtrace_symbols(array, size);
        for (size_t i=0; i<size; i++)
            errorlog << strings[i] << endl;
    #endif
    
    vector<float> rgb(3,0);
    vector<vector<float> > allleds;
    allleds.push_back(rgb);
    allleds[0][0] = 1.0; allleds[0][1] = 0.0; allleds[0][2] = 0.0;
    NUbot::m_this->Actions->addLeds(NUActionatorsData::AllLeds, 0, allleds);
    NUbot::m_this->Actions->addSound(0, NUSounds::SEG_FAULT);
    
    float l_positions[] = {0, 0, 0, 1.41, -1.1, -0.65, 0, 1.41, 1.1, 0.65, 0, -1.0, 0, 2.16, 0, -1.22, 0.0, -1, 0, 2.16, 0, -1.22};
    vector<float> gains(NUbot::m_this->Actions->getNumberOfJoints(NUActionatorsData::AllJoints), 50);
    vector<float> velocities(NUbot::m_this->Actions->getNumberOfJoints(NUActionatorsData::AllJoints), 50);
    vector<float> positions(l_positions, l_positions + sizeof(l_positions)/sizeof(*l_positions));
    NUbot::m_this->Actions->addJointPositions(NUActionatorsData::AllJoints, nusystem->getTime() + 2000, positions, velocities, gains);
    NUbot::m_this->m_platform->actionators->process(NUbot::m_this->Actions);
	NUSystem::msleep(3000);
}

/*! @brief 'Handles an unhandled exception; logs the backtrace to errorlog
    @param e the exception
 */
void NUbot::unhandledExceptionHandler(exception& e)
{
	#ifndef TARGET_OS_IS_WINDOWS
        //!< @todo TODO: check whether the exception is serious, if it is fail safely
        NUbot::m_this->Actions->addSound(0, NUSounds::UNHANDLED_EXCEPTION);
        errorlog << "UNHANDLED EXCEPTION. " << endl;
        debug << "UNHANDLED EXCEPTION. " << endl; 
        void *array[10];
        size_t size;
        char **strings;
        size = backtrace(array, 10);
        strings = backtrace_symbols(array, size);
        for (size_t i=0; i<size; i++)
            errorlog << strings[i] << endl;
        errorlog << e.what() << endl;
	#endif
}


