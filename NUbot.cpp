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

#include <time.h>
#include <errno.h>
#include <iostream>
using namespace std;

#include "NUbot.h"
#ifdef TARGET_IS_NAOWEBOTS
    #include "NUPlatform/Platforms/NAOWebots/NAOWebotsPlatform.h"
#endif
#ifdef TARGET_IS_NAO
    #include "NUPlatform/Platforms/NAO/NAOPlatform.h"
#endif
#ifdef TARGET_IS_CYCLOID
    #include "NUPlatform/Platforms/Cycloid/CycloidPlatform.h"
#endif

static pthread_mutex_t mutexMotionData;    //!< lock for new motion data signal @relates NUbot
static pthread_cond_t condMotionData;      //!< signal for new motion data      @relates NUbot
static pthread_mutex_t mutexVisionData;    //!< lock for new vision data signal @relates NUbot
static pthread_cond_t condVisionData;      //!< signal for new vision data      @relates NUbot

static void* runThreadMotion(void* arg);
static void* runThreadVision(void* arg);

/*! @brief Constructor for the nubot
    
    The parameters are for command line arguements. Webots gives the binary arguements which tell us the 
    robot's number. Consequently, these args are passed down to the webots platform.
 
    @param argc the number of command line arguements
    @param *argv[] the array of command line arguements
 */
NUbot::NUbot(int argc, const char *argv[])
{
#if DEBUG_NUBOT_VERBOSITY > 4
    debug << "NUbot::NUbot(). Constructing NUPlatform." << endl;
#endif
    // Construct the right Platform
    #ifdef TARGET_IS_NAOWEBOTS
        platform = new NAOWebotsPlatform(argc, argv);
    #endif
    #ifdef TARGET_IS_NAO
        platform = new NAOPlatform();
    #endif
    #ifdef TARGET_IS_CYCLOID
        platform = new CycloidPlatform();
    #endif

#if DEBUG_NUBOT_VERBOSITY > 4
    debug << "NUbot::NUbot(). Constructing modules." << endl;
#endif
    
    // Construct each enabled module 
    #ifdef USE_VISION
        vision = new Vision();
    #endif
    #ifdef USE_LOCALISATION
        localisation = new Localisation();
    #endif
    #ifdef USE_BEHAVIOUR
        behaviour = new Behaviour();
    #endif
    #ifdef USE_MOTION
        motion = new NUMotion();
    #endif
    #ifdef USE_NETWORK
        network = new Network();
    #endif
    
    createThreads();
    
#if DEBUG_NUBOT_VERBOSITY > 4
    debug << "NUbot::NUbot(). Finished." << endl;
#endif
}

/*! @brief Create nubot's threads
 
    There is a single real-time thread in which motion runs. A semaphore is set by the body driver which triggers
    the execution of this thread. The fresh sensors data is processed by motion, and commands are sent to the actionators.
 
    The scheduling policy is SCHED_FIFO if the priority (THREAD_MOTION_PRIORITY) is non-zero, otherwise the default policy
    is selected. I recommend that this thread have a non-zero priority.
 
    There is a single non-real-time thread in which vision, localisation and behaviour run. A semaphore is set by the camera
    driver which triggers the execution of this thread. The new image is processed, and computed actions are sent to motion
    and lcs.
 
    The scheduling policy is SCHED_FIFO if the priority (THREAD_VISION_PRIORITY) is non-zero, otherwise the default policy
    is selected. I recommend that this thread have a zero priority.
 */

void NUbot::createThreads()
{
#if DEBUG_NUBOT_VERBOSITY > 4
    debug << "NUbot::createThreads(). Constructing threads." << endl;
#endif
    // create threadMotion and its trigger condMotionData
    int err;
    err = pthread_mutex_init(&mutexMotionData, NULL);
    if (err != 0)
        debug << "NUbot::createThreads(). Failed to initialise mutexMotionData errno: " << errno << endl;
    
    err = pthread_cond_init(&condMotionData, NULL);
    if (err != 0)
        debug << "NUbot::createThreads(). Failed to initialise condMotionData errno: " << errno << endl;

#if THREAD_MOTION_PRIORITY > 0
    err = pthread_create(&threadMotion, NULL, runThreadMotion, (void*) this);         // The last parameter is the arguement to the thread
    if (err != 0)
        debug << "NUbot::createThreads(). Failed to create threadMotion! The error code was: " << err << endl;
    
    sched_param param;
    param.sched_priority = THREAD_MOTION_PRIORITY;
    pthread_setschedparam(threadMotion, SCHED_FIFO, &param);            // Note. This will fail (quietly) if the underlying OS doesn't allow real-time
    
    // double check
    int actualpolicy;
    sched_param actualparam;
    pthread_getschedparam(threadMotion, &actualpolicy, &actualparam);
    #if DEBUG_NUBOT_VERBOSITY > 4
        debug << "NUbot::createThreads(). threadMotion Policy: " << actualpolicy << " Priority: " << actualparam.sched_priority << endl;
    #endif
#else   
    debug << "NUbot::createThreads(). Warning. Creating threadMotion as non-realtime" << endl;
    int err = pthread_create(&threadMotion, NULL, runThreadMotion, (void*) this);
    if (err != 0)
        debug << "NUbot::createThreads(). Failed to create threadMotion! The error code was: " << err << endl;
#endif

    // create threadVision and its trigger condVisionData
    err = pthread_mutex_init(&mutexVisionData, NULL);
    if (err != 0)
        debug << "NUbot::createThreads(). Failed to initialise mutexVisionData errno: " << errno << endl;
    
    err = pthread_cond_init(&condVisionData, NULL);
    if (err != 0)
        debug << "NUbot::createThreads(). Failed to initialise condVisionData errno: " << errno << endl;
    
#if THREAD_VISION_PRIORITY > 0
    debug << "NUbot::createThreads(). Warning. Creating threadVision as realtime" << endl;
    err = pthread_create(&threadVision, NULL, runThreadVision, (void*) this);         // The last parameter is the arguement to the thread
    
    sched_param param;
    param.sched_priority = THREAD_VISION_PRIORITY;
    pthread_setschedparam(threadVision, SCHED_FIFO, &param);            // Note. This will fail (quietly) if the underlying OS doesn't allow real-time
    
    // double check
    int actualpolicy;
    sched_param actualparam;
    pthread_getschedparam(threadVision, &actualpolicy, &actualparam);
    #if DEBUG_NUBOT_VERBOSITY > 4
        debug << "NUbot::createThreads(). threadMotion Policy: " << actualpolicy << " Priority: " << actualparam.sched_priority << endl;
    #endif
#else   
    err = pthread_create(&threadVision, NULL, runThreadVision, (void*) this);
#endif
    if (err != 0)
        debug << "NUbot::createThreads(). Failed to create threadVision! The error code was: " << err << endl;
    
#if DEBUG_NUBOT_VERBOSITY > 4
    debug << "NUbot::createThreads(). Finished." << endl;
#endif
}

/*! @brief Destructor for the nubot
 */
NUbot::~NUbot()
{
    #if DEBUG_NUBOT_VERBOSITY > 4
        debug << "NUbot::~NUbot()." << endl;
    #endif
    
    // destroy threading
    pthread_cancel(threadVision);
    pthread_cancel(threadMotion);
    
    pthread_mutex_destroy(&mutexVisionData);
    pthread_cond_destroy(&condVisionData);
    pthread_mutex_destroy(&mutexMotionData);
    pthread_cond_destroy(&condMotionData);
    
    // delete modules
    delete platform;
    #ifdef USE_VISION
        delete vision;
    #endif
    #ifdef USE_LOCALISATION
        delete localisation;
    #endif
    #ifdef USE_BEHAVIOUR
        delete behaviour;
    #endif
    #ifdef USE_MOTION
        delete motion;
    #endif
    #ifdef USE_NETWORK
        delete network;
    #endif
    #if DEBUG_NUBOT_VERBOSITY > 4
        debug << "NUbot::~NUbot(). Finished." << endl;
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
    // do something smart here!??!
    int count = 0;
    while (true)
    {
        signalMotion();
        if (count%2 == 0)
            signalVision();
        count++;
        usleep(1.0/50*1e6);
    };
    return;
}

/*! @brief Signal the motion thread to run
 
    This should be called by the underlying platform when new motion data is avaliable.
    It is implemented using a pthread_mutex_t and a pthread_cond_t.
 
    This function is static so that the underlying platform does not need a pointer to the nubot.
    However, this means the pthread_mutex_t and pthread_cond_t are external to the NUbot class.
 
    @return returns the error return by the underlying pthread_cond_signal
 */
int NUbot::signalMotion()
{
    int err = 0;
    pthread_mutex_lock(&mutexMotionData);
    err = pthread_cond_signal(&condMotionData);
    pthread_mutex_unlock(&mutexMotionData);
    return err;
}

/*! @brief Blocks the calling thread until signalMotion() is called
 
    @return returns the error return by the underlying pthread_cond_wait
 */
int NUbot::waitForNewMotionData()
{
    int err = 0;
    pthread_mutex_lock(&mutexMotionData);
    err = pthread_cond_wait(&condMotionData, &mutexMotionData);
    pthread_mutex_unlock(&mutexMotionData);
    return err;
}

/*! @brief Signal the vision thread to run
 
 This should be called by the underlying platform when new vision data is avaliable.
 It is implemented using a pthread_mutex_t and a pthread_cond_t.
 
 This function is static so that the underlying platform does not need a pointer to the nubot.
 However, this means the pthread_mutex_t and pthread_cond_t are external to the NUbot class.
 
 @return returns the error return by the underlying pthread_cond_signal
 */
int NUbot::signalVision()
{
    int err = 0;
    pthread_mutex_lock(&mutexVisionData);
    pthread_cond_signal(&condVisionData);
    pthread_mutex_unlock(&mutexVisionData);
    return err;
}

/*! @brief Blocks the calling thread until signalVision() is called
 
 @return returns the error return by the underlying pthread_cond_wait
 */
int NUbot::waitForNewVisionData()
{
    int err = 0;
    pthread_mutex_lock(&mutexVisionData);
    err = pthread_cond_wait(&condVisionData, &mutexVisionData);
    pthread_mutex_unlock(&mutexVisionData);
    return err;
}

/*! @brief The motion control loop
    @relates NUbot
 
    The thread is set up to run when signalled by signalMotion(). It will quickly grab the
    new sensor data, compute a response, and then send the commands to the actionators.
 
    @param arg a pointer to the nubot
 */
void* runThreadMotion(void* arg)
{
    debug << "NUbot::runThreadMotion: Starting." << endl;
    
    NUbot* nubot = (NUbot*) arg;                // the nubot
    
    NUSensorsData* data;
    
#ifdef THREAD_MOTION_MONITOR_TIME
    double entrytime;
    double realstarttime, processstarttime, threadstarttime; 
    double realendtime, processendtime, threadendtime;
#endif
    
    int err;
    do 
    {
#ifdef THREAD_MOTION_MONITOR_TIME
        entrytime = NUSystem::getTime();
#endif
        err = nubot->waitForNewMotionData();

#ifdef THREAD_MOTION_MONITOR_TIME
        realstarttime = NUSystem::getTime();
        if (realstarttime - entrytime > 25)
            debug << "NUbot::runThreadMotion. Waittime " << realstarttime - entrytime << " ms."<< endl;
        processstarttime = NUSystem::getProcessTime();
        threadstarttime = NUSystem::getThreadTime();
#endif
        // -----------------------------------------------------------------------------------------------------------------------------------------------------------------
        data = nubot->platform->sensors->update();
        //                cmds = nubot->motion->process(data)                       // it is up to motion to decide whether it should deep copy
        //        nubot->platform->actionators->process(cmds)
        // -----------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef THREAD_MOTION_MONITOR_TIME
        realendtime = NUSystem::getTime();
        processendtime = NUSystem::getProcessTime();
        threadendtime = NUSystem::getThreadTime();
        if (threadendtime - threadstarttime > 3)
            debug << "NUbot::runThreadMotion. Thread took a long time to complete. Time spent in this thread: " << (threadendtime - threadstarttime) << "ms, in this process: " << (processendtime - processstarttime) << "ms, in realtime: " << realendtime - realstarttime << "ms." << endl;
#endif
    } 
    while (err == 0 | errno != EINTR);
    pthread_exit(NULL);
}

/*! @brief The vision control loop
    @relates NUbot
 
     The thread is set up to run when signalled by signalVision(). It will grab the
     new image, compute a response, and then send the commands to motion and lcs.
 
    @param arg a pointer to the nubot
 */
void* runThreadVision(void* arg)
{
    debug << "NUbot::runThreadVision: Starting." << endl;
    
    NUbot* nubot = (NUbot*) arg;                // the nubot
    JobList joblist = JobList();
    
#ifdef THREAD_VISION_MONITOR_TIME
    double entrytime;
    double realstarttime, processstarttime, threadstarttime; 
    double realendtime, processendtime, threadendtime;
#endif
    
    double timenow;
    
    int err;
    do 
    {
#ifdef THREAD_VISION_MONITOR_TIME
        entrytime = NUSystem::getTime();
#endif
        err = nubot->waitForNewVisionData();
        
#ifdef THREAD_VISION_MONITOR_TIME
        realstarttime = NUSystem::getTime();
        if (realstarttime - entrytime > 1000/15.0 + 5)
            debug << "NUbot::runThreadVision. Waittime " << realstarttime - entrytime << " ms."<< endl;
        
        processstarttime = NUSystem::getProcessTime();
        threadstarttime = NUSystem::getThreadTime();
#endif
        // -----------------------------------------------------------------------------------------------------------------------------------------------------------------
        //          image = nubot->platform->camera->getData()
        //          data = nubot->platform->sensors->getData()                // I should not deep copy the data here
        //      NUCamera::imageWidth
        //      #idef NAOWebots:
        //          NUCamera::imageWidth = WebotsCamera::imageWidth
        //
        //                 odometry = nubot->motion->getData()                       // There is no deep copy here either
        //      gamectrl, teaminfo = nubot->network->getData()
        //          fieldobj = nubot->vision->process(image, data, gamectrl)
        //          wm = nubot->localisation->process(fieldobj, teaminfo, odometry, gamectrl, actions)
        nubot->behaviour->process(joblist);      //TODO: nubot->behaviour->process(wm, gamectrl, p_jobs)
        nubot->motion->process(joblist);
        //          cmds = nubot->lcs->process(lcsactions)
        //          nubot->platform->actionators->process(cmds)
        joblist.clear();                           // assume that all of the jobs have been completed
        // -----------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef THREAD_VISION_MONITOR_TIME
        realendtime = NUSystem::getTime();
        processendtime = NUSystem::getProcessTime();
        threadendtime = NUSystem::getThreadTime();
        if (threadendtime - threadstarttime > 10)
            debug << "NUbot::runThreadVision. Thread took a long time to complete. Time spent in this thread: " << (threadendtime - threadstarttime) << "ms, in this process: " << (processendtime - processstarttime) << "ms, in realtime: " << realendtime - realstarttime << "ms." << endl;
#endif
    } 
    while (err == 0 | errno != EINTR);
    pthread_exit(NULL);
}

