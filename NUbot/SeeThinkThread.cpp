/*! @file SeeThinkThread.cpp
    @brief Implementation of the see->think thread class.

    @author Jason Kulk
 
 Copyright (c) 2010 Jason Kulk
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SeeThinkThread.h"
#include "NUbot.h"

#include "debug.h"
#include "debugverbositynubot.h"
#include "debugverbositythreading.h"

#include <errno.h>

#if DEBUG_NUBOT_VERBOSITY > DEBUG_THREADING_VERBOSITY
    #define DEBUG_VERBOSITY DEBUG_NUBOT_VERBOSITY
#else
    #define DEBUG_VERBOSITY DEBUG_THREADING_VERBOSITY
#endif

/*! @brief Constructs the sense->move thread
 */

SeeThinkThread::SeeThinkThread(NUbot* nubot) : ConditionalThread(string("SeeThinkThread"), THREAD_SEETHINK_PRIORITY)
{
    #if DEBUG_VERBOSITY > 0
        debug << "SeeThinkThread::SeeThinkThread(" << nubot << ") with priority " << static_cast<int>(m_priority) << endl;
    #endif
    m_nubot = nubot;
}

SeeThinkThread::~SeeThinkThread()
{
    #if DEBUG_VERBOSITY > 0
        debug << "SeeThinkThread::~SeeThinkThread()" << endl;
    #endif
}

/*! @brief The sense->move main loop
 
    When signalled the thread will quickly grab the new sensor data, compute a response, 
    and then send the commands to the actionators.
 
    Note that you can not safely use the job interface in this thread, if you need to add
    jobs provide a process function for this thread, and *another* process for the behaviour 
    thread which creates the jobs.
 
 */
void SeeThinkThread::run()
{
    #if DEBUG_VERBOSITY > 0
        debug << "SeeThinkThread::run()" << endl;
    #endif
    
    #ifdef THREAD_SEETHINK_MONITOR_TIME
        double entrytime;
        double realstarttime, processstarttime, threadstarttime; 
        double realendtime, processendtime, threadendtime;
    #endif
    
    int err = 0;                FieldObjects* AllObjects= m_nubot->m_vision->ProcessFrame(m_nubot->Image, m_nubot->SensorData);
    while (err == 0 && errno != EINTR)
    {
        try
        {
            #ifdef THREAD_SEETHINK_MONITOR_TIME
                entrytime = NUSystem::getRealTime();
            #endif
            
            #if defined(TARGET_IS_NAOWEBOTS) or (not defined(USE_VISION))
                waitForCondition();
            #endif
            
            #ifdef USE_VISION
                m_nubot->Image = m_nubot->m_platform->camera->grabNewImage();
            #endif

            #ifdef THREAD_SEETHINK_MONITOR_TIME
                realstarttime = NUSystem::getRealTime();
                #ifndef TARGET_IS_NAOWEBOTS         // there is no point monitoring wait times in webots
                if (realstarttime - entrytime > 40)
                    debug << "SeeThinkThread. Warning. Waittime " << realstarttime - entrytime << "ms."<< endl;
                #endif
                processstarttime = NUSystem::getProcessTime();
                threadstarttime = NUSystem::getThreadTime();
            #endif
                
            // -----------------------------------------------------------------------------------------------------------------------------------------------------------------
            #ifdef USE_VISION
                FieldObjects* AllObjects= m_nubot->m_vision->ProcessFrame(m_nubot->Image, m_nubot->SensorData);
            #endif
            
            #ifdef USE_LOCALISATION
                //wm = nubot->localisation->process(fieldobj, teaminfo, odometry, gamectrl, actions)
            #endif
            
            #ifdef USE_BEHAVIOUR
                //m_nubot->m_behaviour->process();
            #endif
            
            #ifdef USE_MOTION
                m_nubot->m_motion->process(*m_nubot->Jobs);
            #endif
            // -----------------------------------------------------------------------------------------------------------------------------------------------------------------

            #if DEBUG_VERBOSITY > 0
                m_nubot->Jobs->summaryTo(debug);
            #endif
            
            #ifdef THREAD_SEETHINK_MONITOR_TIME
                realendtime = NUSystem::getRealTime();
                processendtime = NUSystem::getProcessTime();
                threadendtime = NUSystem::getThreadTime();
                if (threadendtime - threadstarttime > 7)
                    debug << "SeeThinkThread. Warning. Thread took a long time to complete. Time spent in this thread: " << (threadendtime - threadstarttime) << "ms, in this process: " << (processendtime - processstarttime) << "ms, in realtime: " << realendtime - realstarttime << "ms." << endl;
            #endif
            
            #if defined(TARGET_IS_NAOWEBOTS) or (not defined(USE_VISION))
                onLoopCompleted();
            #endif
        }
        catch (std::exception& e)
        {
            m_nubot->unhandledExceptionHandler(e);
        }
    } 
    errorlog << "SeeThinkThread is exiting. err: " << err << " errno: " << errno << endl;
}
