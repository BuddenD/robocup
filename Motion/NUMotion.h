/*! @file NUMotion.h
    @brief Declaration of motion class
 
    @class NUMotion
    @brief A module to provide motion
 
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

#ifndef NUMOTION_H
#define NUMOTION_H

class NUSensorsData;
class NUActionatorsData;
class JobList;

#include "motionconfig.h"
#ifdef USE_HEAD
    class NUHead;
#endif
#ifdef USE_WALK
    class NUWalk;
#endif
#ifdef USE_KICK
    class NUKick;
#endif
#if defined(USE_BLOCK) or defined(USE_SAVE)
    class NUSave;
#endif
#ifdef USE_SCRIPT
    class Script;
#endif
class FallProtection;
class Getup;
class MotionScript;

class NUMotion
{
public:
    NUMotion();
    ~NUMotion();
    
    void process(NUSensorsData* data, NUActionatorsData* actions);
    void process(JobList* jobs);
    
    void kill();
private:
public:
protected:
private:
    NUSensorsData* m_data;              //!< pointer to shared sensors data object
    NUActionatorsData* m_actions;       //!< pointer to shared actionators data object
    
    // essential motion components
    FallProtection* m_fall_protection;  //!< the fall protection module
    Getup* m_getup;                     //!< the getup module
    // optional motion components
    #ifdef USE_HEAD
        NUHead* m_head;                     //!< the head module
    #endif
    #ifdef USE_WALK
        NUWalk* m_walk;                     //!< the walk module
    #endif
    #ifdef USE_KICK
        NUKick* m_kick;                     //!< the kick module
    #endif
    #if defined(USE_BLOCK) or defined(USE_SAVE)
        NUSave* m_save;                     //!< the save module
    #endif
    #ifdef USE_SCRIPT
        Script* m_script;                   //!< the script module
    #endif
    
    double m_current_time;              //!< the current time (ms)
    double m_previous_time;             //!< the previous time (ms)
    int m_cycle_time;                   //!< the cycle time in ms
    
    MotionScript* m_block_left;
};

#endif

