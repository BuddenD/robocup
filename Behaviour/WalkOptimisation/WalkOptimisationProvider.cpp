/*! @file WalkOptimisationProvider.cpp
    @brief Implementation of chase ball behaviour class

    @author Jason Kulk
 
 Copyright (c) 2010 Jason Kulk
 
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

#include "WalkOptimisationProvider.h"
#include "ChaseBlueGoal.h"
#include "ChaseYellowGoal.h"
#include "SearchForBlueGoal.h"
#include "SearchForYellowGoal.h"
#include "Paused.h"

#include "Behaviour/Jobs/JobList.h"
#include "NUPlatform/NUSensors/NUSensorsData.h"
#include "NUPlatform/NUActionators/NUActionatorsData.h"
#include "Vision/FieldObjects/FieldObjects.h"
#include "GameController/GameInformation.h"
#include "Behaviour/TeamInformation.h"

#include "Behaviour/Jobs/MotionJobs/HeadJob.h"
#include "Behaviour/Jobs/MotionJobs/HeadPanJob.h"
#include "Behaviour/Jobs/MotionJobs/HeadNodJob.h"
#include "Behaviour/Jobs/MotionJobs/WalkJob.h"

#include "debug.h"
#include "debugverbositybehaviour.h"

using namespace std;

WalkOptimisationProvider::WalkOptimisationProvider(Behaviour* manager) : BehaviourFSMProvider(manager)
{
    m_chase_blue_goal = new ChaseBlueGoal(this);
    addState(m_chase_blue_goal);
    m_chase_yellow_goal = new ChaseYellowGoal(this);
    addState(m_chase_yellow_goal);
    m_search_blue_goal = new SearchForBlueGoal(this);
    addState(m_search_blue_goal);
    m_search_yellow_goal = new SearchForYellowGoal(this);
    addState(m_search_yellow_goal);
    m_paused = new Paused(this);
    addState(m_paused);
    
    m_state = m_search_blue_goal;
}


WalkOptimisationProvider::~WalkOptimisationProvider()
{
}

BehaviourState* WalkOptimisationProvider::nextStateCommons()
{
    if (singleChestClick() or longChestClick())
    {
        if (m_state == m_paused)
            return m_previous_state;
        else
            return m_paused;
    }
    else
        return m_state;
}


