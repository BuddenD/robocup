/*! @file PanHeadJob.cpp
    @brief Implementation of PanHeadJob class

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

#include "PanHeadJob.h"
#include "debug.h"

/*! @brief Constructs a PanHeadJob
 
    @param period the new pan period in milliseconds
    @param centre the centre head position about which we will pan [yaw (rad), pitch (rad), roll (rad)]
    @param limits the lower and upper angle limits for the pan (rad)
 */
PanHeadJob::PanHeadJob(double period, const vector<float>& centre, const vector<float>& limits) : MotionJob(Job::MOTION_PAN)
{
    m_job_time = period;     
    m_centre_position = centre;
    m_limit_positions = limits;
}

/*! @brief Constructs a PanHeadJob from stream data
    @param time the time in ms to perform the kick
    @param input the stream from which to read the job specific data
 */
PanHeadJob::PanHeadJob(double time, istream& input) : MotionJob(Job::MOTION_PAN)
{
    m_job_time = time;

    // Temporary read buffers
    unsigned int uintBuffer;
    float floatBuffer;

    // read in the centre position size
    input.read(reinterpret_cast<char*>(&uintBuffer), sizeof(unsigned int));
    unsigned int m_centre_position_size = uintBuffer;
    
    // read in the centre position vector
    m_centre_position = vector<float>(m_centre_position_size, 0);
    for (unsigned int i=0; i<m_centre_position_size; i++)
    {
        input.read(reinterpret_cast<char*>(&floatBuffer), sizeof(float));
        m_centre_position[i] = floatBuffer;
    }
    
    // read in the limit position size
    input.read(reinterpret_cast<char*>(&uintBuffer), sizeof(unsigned int));
    unsigned int m_limit_positions_size = uintBuffer;
    
    // read in the limit position vector
    m_limit_positions = vector<float>(m_limit_positions_size, 0);
    for (unsigned int i=0; i<m_limit_positions_size; i++)
    {
        input.read(reinterpret_cast<char*>(&floatBuffer), sizeof(float));
        m_limit_positions[i] = floatBuffer;
    }
}

/*! @brief WalkJob destructor
 */
PanHeadJob::~PanHeadJob()
{
    m_centre_position.clear();
    m_limit_positions.clear();
}

/*! @brief Sets the pan for the head
 
    You should only need to use this function if you are recycling the one job 
    (ie. I provide this function if you are worried about creating a new job every time)
    
    @param period the new pan period in milliseconds
    @param centre the centre head position about which we will pan [yaw (rad), pitch (rad), roll (rad)]
    @param limits the lower and upper angle limits for the pan (rad)
 */
void PanHeadJob::setPan(double period, const vector<float>& centre, const vector<float>& limits)
{
    m_job_time = period;     
    m_centre_position = centre;
    m_limit_positions = limits;
}

/*! @brief Gets the pan for the head
 
    @param period the pan period in milliseconds
    @param centre the centre head position about which we will pan [yaw (rad), pitch (rad), roll (rad)]
    @param limits the lower and upper angle limits for the pan (rad)
 */
void PanHeadJob::getPan(double& period, vector<float>& centre, vector<float>& limits)
{
    period = m_job_time;
    centre = m_centre_position;
    limits = m_limit_positions;
}

/*! @brief Prints a human-readable summary to the stream
 @param output the stream to be written to
 */
void PanHeadJob::summaryTo(ostream& output)
{
    output << "PanHeadJob: " << m_job_time << " ";
    for (unsigned int i=0; i<m_centre_position.size(); i++)
        output << m_centre_position[i] << ",";
    output << " ";
    for (unsigned int i=0; i<m_limit_positions.size(); i++)
        output << m_limit_positions[i] << ",";
    output << endl;
}

/*! @brief Prints a csv version to the stream
 @param output the stream to be written to
 */
void PanHeadJob::csvTo(ostream& output)
{
    output << "PanHeadJob: " << m_job_time << " ";
    for (unsigned int i=0; i<m_centre_position.size(); i++)
        output << m_centre_position[i] << ", ";
    for (unsigned int i=0; i<m_limit_positions.size(); i++)
        output << m_limit_positions[i] << ", ";
    output << endl;
}

/*! @brief A helper function to ease writing Job objects to classes
 
 This function calls its parents versions of the toStream, each parent
 writes the members introduced at that level
 
 @param output the stream to write the job to
 */
void PanHeadJob::toStream(ostream& output) const
{
    debug << "PanHeadJob::toStream" << endl;
    Job::toStream(output);                  // This writes data introduced at the base level
    MotionJob::toStream(output);            // This writes data introduced at the motion level
                                            // Then we write PanHeadJob specific data
    unsigned int m_centre_position_size = m_centre_position.size();
    output.write((char*) &m_centre_position_size, sizeof(m_centre_position_size));
    for (unsigned int i=0; i<m_centre_position_size; i++)
        output.write((char*) &m_centre_position[i], sizeof(m_centre_position[i]));
    unsigned int m_limit_positions_size = m_limit_positions.size();
    output.write((char*) &m_limit_positions_size, sizeof(m_limit_positions_size));
    for (unsigned int i=0; i<m_limit_positions_size; i++)
        output.write((char*) &m_limit_positions[i], sizeof(m_limit_positions[i]));
}

/*! @relates PanHeadJob
 @brief Stream insertion operator for a PanHeadJob
 
 @param output the stream to write to
 @param job the job to be written to the stream
 */
ostream& operator<<(ostream& output, const PanHeadJob& job)
{
    debug << "<<PanHeadJob" << endl;
    job.toStream(output);
    return output;
}

/*! @relates PanHeadJob
 @brief Stream insertion operator for a pointer to PanHeadJob
 
 @param output the stream to write to
 @param job the job to be written to the stream
 */
ostream& operator<<(ostream& output, const PanHeadJob* job)
{
    debug << "<<PanHeadJob" << endl;
    if (job != NULL)
        job->toStream(output);
    else
        output << "NULL";
    return output;
}

