/*! @file NUMotion.cpp
    @brief Implementation of motion class

    @author Jason Kulk
 
 Copyright (c) 2009, 2010 Jason Kulk
 
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
#include "NUMotion.h"

#include "NUPlatform/NUPlatform.h"
#include "NUPlatform/NUSensors/NUSensorsData.h"
#include "NUPlatform/NUActionators/NUActionatorsData.h"
#include "debug.h"
#include "debugverbositynumotion.h"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

#include "Tools/MotionScript.h"

/*! @brief Constructor for motion module
 */
NUMotion::NUMotion()
{
    #if DEBUG_NUMOTION_VERBOSITY > 4
        debug << "NUMotion::NUMotion" << endl;
    #endif
    m_current_time = 0;
    m_previous_time = 0;
    #ifdef USE_HEAD
        m_head = new NUHead();
    #endif
    #ifdef USE_WALK
        m_walk = NUWalk::getWalkEngine();
    #endif
    #ifdef USE_KICK
        m_kick = new NUKick();
    #endif
    
    m_block_left = MotionScript("BlockLeft");
}

/*! @brief Destructor for motion module
 */
NUMotion::~NUMotion()
{
    if (m_fall_protection != NULL)
        delete m_fall_protection;
    
    if (m_getup != NULL)
        delete m_getup;                   
    #ifdef USE_HEAD
        if (m_head != NULL)
            delete m_head;
    #endif
    #ifdef USE_WALK
        if (m_walk != NULL)
            delete m_walk;                
    #endif
    #ifdef USE_KICK
        if (m_kick != NULL)
            delete m_kick;                   
    #endif
}

/*! @brief Adds actions to bring the robot to rest quickly, and go into a safe-for-robot pose
 */
void NUMotion::safeKill(NUSensorsData* data, NUActionatorsData* actions)
{
    float safelegpositions[] = {0, -1.0, 0, 2.16, 0, -1.22};
    float safelarmpositions[] = {0, 1.41, -1.1, -0.65};
    float saferarmpositions[] = {0, 1.41, 1.1, 0.65};
    vector<float> legpositions(safelegpositions, safelegpositions + sizeof(safelegpositions)/sizeof(*safelegpositions));
    vector<float> larmpositions(safelarmpositions, safelarmpositions + sizeof(safelarmpositions)/sizeof(*safelarmpositions));
    vector<float> rarmpositions(saferarmpositions, saferarmpositions + sizeof(saferarmpositions)/sizeof(*saferarmpositions));
    vector<float> legvelocities(legpositions.size(), 1.0);
    vector<float> armvelocities(larmpositions.size(), 1.0);
    
    // check if there is a reason it is not safe or possible to go into the crouch position
    if (actions == NULL)
        return;
    else if (data != NULL)
    {
        vector<float> orientation;
        if (m_data->getOrientation(orientation))
            if (fabs(orientation[0]) > 0.5 or fabs(orientation[1]) > 0.5)
                return;
        /*bool leftsupport, rightsupport;
        if (m_data->leftSupport(leftsupport) and m_data->rightSupport(rightsupport))
            if (not (leftsupport and rightsupport)
                return;*/
    }
    
    actions->addJointPositions(NUActionatorsData::LeftLegJoints, nusystem->getTime() + 1250, legpositions, legvelocities, 50);
    actions->addJointPositions(NUActionatorsData::RightLegJoints, nusystem->getTime() + 1250, legpositions, legvelocities, 50);
    actions->addJointPositions(NUActionatorsData::LeftArmJoints, nusystem->getTime() + 750, larmpositions, armvelocities, 30);
    actions->addJointPositions(NUActionatorsData::RightArmJoints, nusystem->getTime() + 750, rarmpositions, armvelocities, 30);
}

/*! @brief Process new sensor data, and produce actionator commands.
 
    This function basically calls all of the process functions of the submodules of motion. I have it setup
    so that the process functions are only called when they are allowed to be executed.
 
    @param data a pointer to the most recent sensor data storage class
    @param actions a pointer to the actionators data storage class. This variable will be filled
                   with the desired actions by NUMotion provided the NUActionatorsData instance
                   has been initialised by NUActionators.
 */
void NUMotion::process(NUSensorsData* data, NUActionatorsData* actions)
{
#if DEBUG_NUMOTION_VERBOSITY > 2
    debug << "NUMotion::process(" << data << ", " << actions << ")" << endl;
#endif
    if (data == NULL || actions == NULL)
        return;
    m_data = data;
    m_actions = actions;
    m_current_time = m_data->CurrentTime;
    calculateCycleTime();
    
    static vector<float> fallingvalues;
    static vector<float> fallenvalues;
    data->getFalling(fallingvalues);
    data->getFallen(fallenvalues);              //! @todo Put in a compile flag here or something because I need to walk while fallen atm
    if (false && fallingvalues[0] > 0)                           // If falling you can't do ANY motion except the fall protection.
        m_fall_protection->process(data, actions);
    else if (false && fallenvalues[0] > 0)                       // If fallen you can only getup
    {
        m_getup->process(data, actions);
        if (m_getup->headReady())                       // And you can only use the head if the getup lets you
        {
            #ifdef USE_HEAD
                m_head->process(data, actions);
            #endif
        }
    }
    else                                                // If not falling and not fallen I can do kicks, walks, saves and blocks
    {
        #ifdef USE_HEAD
            m_head->process(data, actions);
        #endif
        #ifdef USE_WALK
            m_walk->process(data, actions);
        #endif
        #ifdef USE_KICK
            m_kick->process(data, actions);
        #endif
    }
    
    m_previous_time = m_current_time;
    
    static bool alreadyran = false;
    if (m_current_time > 15500 and not alreadyran)
    {
        m_block_left.play(data, actions);
        alreadyran = true;
    }
}

/*! @brief Process the jobs. Jobs are deleted when they are completed, and more jobs can be added inside this function.
    
    @param jobs the current list of jobs
 */
void NUMotion::process(JobList& jobs)
{
#if DEBUG_NUMOTION_VERBOSITY > 4
    debug << "NUMotion::process(): Start" << endl;
#endif
    
    list<Job*>::iterator it = jobs.motion_begin();     // the iterator over the motion jobs
    while (it != jobs.motion_end())
    {
        Job::job_id_t id = (*it)->getID();
        switch (id) 
        {
        #ifdef USE_WALK
            case Job::MOTION_WALK:
                    static vector<float> speed;
                    static WalkJob* walkjob;
                    
                    walkjob = (WalkJob*) (*it);
                    walkjob->getSpeed(speed);
                    m_walk->walkSpeed(speed);
                break;
            case Job::MOTION_WALK_TO_POINT:
                    static double time_a;
                    static vector<float> position;
                    static WalkToPointJob* walktopointjob;
                    
                    walktopointjob = (WalkToPointJob*) (*it);
                    walktopointjob->getPosition(time_a, position);
                    
                    m_walk->walkToPoint(time_a, position);
                break;
            case Job::MOTION_WALK_PARAMETERS:
                static WalkParameters parameters;
                static WalkParametersJob* parametersjob;
                
                parametersjob = (WalkParametersJob*) (*it);
                parametersjob->getWalkParameters(parameters);
                
                m_walk->setWalkParameters(parameters);
                break;
        #endif // USE_WALK
        #ifdef USE_KICK
            case Job::MOTION_KICK:
                static double time_b;
                static vector<float> kickposition;
                static vector<float> kicktarget;
                static KickJob* kickjob;
                
                kickjob = (KickJob*) (*it);
                kickjob->getKick(time_b, kickposition, kicktarget);
                
                m_kick->kickToPoint(kickposition, kicktarget);
                break;
        #endif //USE_KICK
        #ifdef USE_HEAD
            case Job::MOTION_HEAD:
                m_head->process(reinterpret_cast<HeadJob*> (*it));
                break;
            case Job:: MOTION_PAN:
                m_head->process(reinterpret_cast<HeadPanJob*> (*it));
                break;
            case Job::MOTION_NOD:
                m_head->process(reinterpret_cast<HeadNodJob*> (*it));
                break;
        #endif // USE_HEAD
            default:
                break;
        }
        it = jobs.removeMotionJob(it);
    }
    
    #if DEBUG_NUMOTION_VERBOSITY > 4
        debug << "NUMotion::process(): Finished" << endl;
    #endif
}

/*! @brief Calculates the cycle time. 
 
 To be platform independent I calculate the motion cycle time online by averaging the cycle times
*/
void NUMotion::calculateCycleTime()
{
    using namespace boost::accumulators;
    static accumulator_set<float, stats<tag::mean> > cycle_time_accumulator;
    
    cycle_time_accumulator(m_current_time - m_previous_time);
    m_cycle_time = static_cast<int> (mean(cycle_time_accumulator));
}


