/*! @file BlockJob.cpp
    @brief Implementation of BlockJob class

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

#include "BlockJob.h"

BlockJob::BlockJob(double time, const vector<float>& position) : MotionJob(Job::MOTION_WALK)
{
    m_job_time = time;     
    m_block_position = position;
}

/*! @brief WalkJob destructor
 */
BlockJob::~BlockJob()
{
    m_block_position.clear();
}

/*! @brief Sets the position for the block to point
 
    You should only need to use this function if you are recycling the one job 
    (ie. I provide this function if you are worried about creating a new job every time)
    
    @param newposition the new position for the block job [x(cm), y(cm), theta(rad)]
 */
void BlockJob::setPosition(double time, const vector<float>& newposition)
{
    m_job_time = time;
    m_block_position = newposition;
}

/*! @brief Gets the position of the block job
    @param position parameter that will be updated with the point we want to block
 */
void BlockJob::getPosition(double& time, vector<float>& position)
{
    time = m_job_time;
    position = m_block_position;
}
