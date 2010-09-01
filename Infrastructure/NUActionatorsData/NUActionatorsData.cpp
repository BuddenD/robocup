/*! @file NUActionatorsData.cpp
    @brief Implementation of actionators data class

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

#include "NUActionatorsData.h"
#include "Actionator.h"

#include <sstream>

#include "debug.h"
#include "debugverbositynuactionators.h"

int a_curr_id = NUData::NumCommonIds.Id+1;
vector<NUActionatorsData::id_t*> NUActionatorsData::m_ids;
// Led actionators
const NUActionatorsData::id_t NUActionatorsData::ChestLed(a_curr_id++, "ChestLed", NUActionatorsData::m_ids);
const NUActionatorsData::id_t NUActionatorsData::LFootLed(a_curr_id++, "LFootLed", NUActionatorsData::m_ids);
const NUActionatorsData::id_t NUActionatorsData::RFootLed(a_curr_id++, "RFootLed", NUActionatorsData::m_ids);
const NUActionatorsData::id_t NUActionatorsData::LEyeLed(a_curr_id++, "LEyeLed", NUActionatorsData::m_ids);
const NUActionatorsData::id_t NUActionatorsData::REyeLed(a_curr_id++, "REyeLed", NUActionatorsData::m_ids);
const NUActionatorsData::id_t NUActionatorsData::LEarLed(a_curr_id++, "LEarLed", NUActionatorsData::m_ids);
const NUActionatorsData::id_t NUActionatorsData::REarLed(a_curr_id++, "REarLed", NUActionatorsData::m_ids);
// Led groups
const NUActionatorsData::id_t NUActionatorsData::FaceLeds(a_curr_id++, "FaceLeds", NUActionatorsData::m_ids);
const NUActionatorsData::id_t NUActionatorsData::FeetLeds(a_curr_id++, "FeetLeds", NUActionatorsData::m_ids);
const NUActionatorsData::id_t NUActionatorsData::AllLeds(a_curr_id++, "AllLeds", NUActionatorsData::m_ids);
// Other actionators
const NUActionatorsData::id_t NUActionatorsData::Sound(a_curr_id++, "Sound", NUActionatorsData::m_ids);
const NUActionatorsData::id_t NUActionatorsData::Teleporter(a_curr_id++, "Teleporter", NUActionatorsData::m_ids);

/*! @brief Default constructor for a NUActionatorsData storage class.
 */
NUActionatorsData::NUActionatorsData()
{
    #if DEBUG_NUACTIONATORS_VERBOSITY > 0
        debug << "NUActionatorsData::NUActionatorsData" << endl;
    #endif
    CurrentTime = 0;
    
    m_ids.insert(m_ids.begin(), NUData::Ids.begin(), NUData::Ids.end());
    
    for (size_t i=0; i<m_ids.size(); i++)
        m_actionators.push_back(Actionator(m_ids[i]->Name));
    m_id_to_indices = vector<vector<int> >(m_ids.size(), vector<int>());
}

/*! @brief Destroys the NUActionatorsData storage class
 */
NUActionatorsData::~NUActionatorsData()
{
}

/******************************************************************************************************************************************
 Initialisation and Availability Setting Methods
 ******************************************************************************************************************************************/
/*! @brief Adds the actionators to the NUActionatorsData listed in hardwarenames
    @param hardwarenames a vector containing every actionator available on the robotic platform
 */
void NUActionatorsData::addActionators(const vector<string>& hardwarenames)
{
    vector<string> names = standardiseNames(hardwarenames);
    for (size_t i=0; i<names.size(); i++)
    {	// for each name compare it to the name of that actionator
        for (size_t j=0; j<m_ids.size(); j++)
        {
            if (*(m_ids[j]) == names[i])
            {
                m_id_to_indices[j].push_back(j);
                m_available_actionators.push_back(j);
            }
        }
    }
    
    for (size_t i=0; i<m_ids.size(); i++)
    {	// fill in the groups
        for (size_t j=0; j<m_ids.size(); j++)
        {
            if (not m_id_to_indices[j].empty() and belongsToGroup(*m_ids[j], *m_ids[i]))
                m_id_to_indices[i].push_back(j);
        }
    }
    
    #if DEBUG_NUACTIONATORS_VERBOSITY > 0
        debug << "NUActionatorsData::addActionators:" << endl;
        printMap(debug);
    #endif
}

/*! @brief Returns true if member belongs to group (this function extends the NUData::belongsToGroup)
    @param member the single id
    @param group the group id
    @return true if member belongs to group 
 */
bool NUActionatorsData::belongsToGroup(const id_t& member, const id_t& group)
{
    bool belongstonudata = NUData::belongsToGroup(member, group);
    if (belongstonudata)
        return true;
    else
    {
        if (group == FaceLeds)
        {
            if (member.Name.find("Led") != string::npos and (member.Name.find("Eye") != string::npos  or member.Name.find("Mouth") != string::npos))
                return true;
            else
                return false;
        }
        else if (group == FeetLeds)
        {
            if (member.Name.find("Led") != string::npos and member.Name.find("Foot") != string::npos)
                return true;
            else
                return false;
        }
        else if (group == AllLeds)
        {
            if (member.Name.find("Led") != string::npos)
                return true;
            else
                return false;
        }
        else
            return false;
    }
}

/******************************************************************************************************************************************
 Get Methods
 ******************************************************************************************************************************************/
/*! @brief Pre processes the data to be ready for copying to hardware communication
 */
void NUActionatorsData::preProcess(double currenttime)
{
    CurrentTime = currenttime;
    for (size_t i=0; i<m_available_actionators.size(); i++)
        m_actionators[m_available_actionators[i]].preProcess();
}

/*! @brief Post processes the data after sending it to the hardware communications (Remove all of the completed actionator points)
    @param currenttime all actionator points that have times before this one are assumed to have been completed, and they will be removed
 */
void NUActionatorsData::postProcess()
{
    for (size_t i=0; i<m_available_actionators.size(); i++)
        m_actionators[m_available_actionators[i]].postProcess(CurrentTime);
}

vector<int>& NUActionatorsData::getIndices(const id_t& actionatorid)
{
    return m_id_to_indices[0];
}

size_t NUActionatorsData::getSize(const id_t& actionatorid)
{
    return m_id_to_indices[actionatorid.Id].size();;
}

/******************************************************************************************************************************************
 Set Methods
 ******************************************************************************************************************************************/
/*! @brief Adds a single [time, data] point to the actionatorid
    @param actionatorid the id of the actionator to add the data
    @param time the time in ms associated with the data
    @param data the data
 */
void NUActionatorsData::add(const id_t& actionatorid, double time, float data)
{
    #if DEBUG_NUACTIONATORS_VERBOSITY > 4
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << ")" << endl;
    #endif
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    for (size_t i=0; i<ids.size(); i++)
        m_actionators[ids[i]].add(time, data);
}

/*! @brief Adds a [time, [data,gain]] tuple to the actionatorid
    @param actionatorid the id of the actionator to add the data
    @param time the time in ms associated with the data,gain point
    @param data the data
    @param gain the gain
 */
void NUActionatorsData::add(const id_t& actionatorid, double time, float data, float gain)
{
    #if DEBUG_NUACTIONATORS_VERBOSITY > 4
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "," << gain << ")" << endl;
    #endif
    // combine the data and gain into a vector
    vector<float> d(2,0);
    d[0] = data;
    d[1] = gain;
    // then just add it
    add(actionatorid, time, d);
}

/*! @brief Adds the data to the actionatorid with a single time. 
 
           If the size of the data matches the number of actionators in actionatorid
           then each element is given to each actionator.
           If the size does not match, or there is only a single actionator under this actionatorid
           then the actionator(s) are given the entire data.
 
    @param actionatorid the id of the targeted actionator(s)
    @param time the time in ms associated with the data
    @param data the data
*/
void NUActionatorsData::add(const id_t& actionatorid, double time, const vector<float>& data)
{
    #if DEBUG_NUACTIONATORS_VERBOSITY > 4
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << ")" << endl;
    #endif
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    if (numids == 0)
        return;
    else if (numids == 1)
        m_actionators[ids[0]].add(time, data);
    else
    {   // if actionatorid is a group then either add elements or add vector depending on the data's size
        if (data.size() == numids)
        {
            for (size_t i=0; i<numids; i++)
                m_actionators[ids[i]].add(time, data[i]);
        }
        else
        {
            for (size_t i=0; i<numids; i++)
                m_actionators[ids[i]].add(time, data);
        }
    }
}

/*! @brief Adds the data and gain to the actionatorid with a single time. The size of the data must match the number
           of actionators under the id, and the same gain value is used for each.
 
    @param actionatorid the id of the targeted actionators
    @param time the time in ms associated with the data,gain
    @param data the data
    @param gain the gain
 */
void NUActionatorsData::add(const id_t& actionatorid, double time, const vector<float>& data, float gain)
{
    #if DEBUG_NUACTIONATORS_VERBOSITY > 4
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "," << gain << ")" << endl;
    #endif
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    if (numids == 0)
        return;
    else if (numids > 1 and numids == data.size())
    {
        vector<float> d(2,0);
        d[1] = gain;
        for (size_t i=0; i<numids; i++)
        {
            d[0] = data[i];
            m_actionators[ids[i]].add(time, d);
        }
    }
    else
    {
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "," << gain << "). The data is incorrectly formatted. ";
        debug << "data.size():" << data.size() << " must be ids.size()" << numids << endl;
    }
}

/*! @brief Adds the data and gain to the actionatorid with a single time. The size of the data and gains must match the number
           of actionators under the id
 
    @param actionatorid the id of the targeted actionators
    @param time the time in ms associated with the data,gain
    @param data the data
    @param gain the gain
 */
void NUActionatorsData::add(const id_t& actionatorid, double time, const vector<float>& data, const vector<float>& gain)
{
    #if DEBUG_NUACTIONATORS_VERBOSITY > 4
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "," << gain << ")" << endl;
    #endif
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    if (numids == 0)
        return;
    else if (numids > 1 and numids == data.size() and numids == gain.size())
    {
        vector<float> d(2,0);
        for (size_t i=0; i<numids; i++)
        {
            d[0] = data[i];
            d[1] = gain[i];
            m_actionators[ids[i]].add(time, d);
        }
    }
    else
    {
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "," << gain << "). The data is incorrectly formatted. ";
        debug << "data.size():" << data.size() << " and gain.size():" << gain.size() << " must be ids.size()" << numids << endl;
    }
}

/*! @brief Adds the data to the actionatorid with a single time. 
 
            If the size of the data matches the number of actionators in actionatorid
            then each element is given to each actionator.
            If the size does not match, or there is only a single actionator under this actionatorid
            then the actionator(s) are given the entire data.
 
    @param actionatorid the id of the targeted actionator(s)
    @param time the time in ms associated with the data
    @param data the data
 */
void NUActionatorsData::add(const id_t& actionatorid, double time, const vector<vector<float> >& data)
{
    #if DEBUG_NUACTIONATORS_VERBOSITY > 4
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << ",...)" << endl;
    #endif
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    if (numids == 0)
        return;
    else if (numids == 1)
        m_actionators[ids[0]].add(time, data);
    else
    {   // if actionatorid is a group then either add elements or add vector depending on the data's size
        if (data.size() == numids)
        {
            for (size_t i=0; i<numids; i++)
                m_actionators[ids[i]].add(time, data[i]);
        }
        else
        {
            for (size_t i=0; i<numids; i++)
                m_actionators[ids[i]].add(time, data);
        }
    }
}

/*! @brief Adds the data to the actionatorid with a single time. 
 
             If the size of the data matches the number of actionators in actionatorid
             then each element is given to each actionator.
             If the size does not match, or there is only a single actionator under this actionatorid
             then the actionator(s) are given the entire data.
 
    @param actionatorid the id of the targeted actionator(s)
    @param time the time in ms associated with the data
    @param data the data
 */
void NUActionatorsData::add(const id_t& actionatorid, double time, const vector<vector<vector<float> > >& data)
{
    #if DEBUG_NUACTIONATORS_VERBOSITY > 4
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << ",...)" << endl;
    #endif
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    if (numids == 0)
        return;
    else if (numids == 1)
        m_actionators[ids[0]].add(time, data);
    else
    {   // if actionatorid is a group then either add elements or add vector depending on the data's size
        if (data.size() == numids)
        {
            for (size_t i=0; i<numids; i++)
                m_actionators[ids[i]].add(time, data[i]);
        }
        else
        {
            for (size_t i=0; i<numids; i++)
                m_actionators[ids[i]].add(time, data);
        }
    }
}

/*! @brief Adds a single [time, data] point to the actionatorid
    @param actionatorid the id of the actionator to add the data
    @param time the time in ms associated with the data
    @param data the data
 */
void NUActionatorsData::add(const id_t& actionatorid, double time, const string& data)
{
    #if DEBUG_NUACTIONATORS_VERBOSITY > 4
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << ")" << endl;
    #endif
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    for (size_t i=0; i<ids.size(); i++)
        m_actionators[ids[i]].add(time, data);
}

/*! @brief Adds a sequence of points to an actionatorid
        
        For a single actionator the data needs to be formatted as [time0, time1, ... , timeN, [data0, data1, ... , dataN].
        For a group actionator the data needs to be formatted as either 
            [id0, id1, ... , idN] [time0, time1, ... , timeN] [data0, data1, ... , dataN]
            [id0, id1, ... , idN] [time0, time1, ... , timeN] data
 
    @param actionatorid the id of the targetted actionator(s)
    @param time the sequence of times in ms
    @param data the sequence of data points
 */
void NUActionatorsData::add(const id_t& actionatorid, const vector<double>& time, const vector<float>& data)
{
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();

    if (numids == 0)
        return;
    else if (numids == 1)
    {   // if actionatorid is a single actionator
        if (time.size() == data.size())
        {
            for (size_t i=0; i<time.size(); i++)
                add(actionatorid, time[i], data[i]);
        }
        else
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). time and data need to have the same length when targeting a single actionator." << endl;
    }
    else
    {   // if actionatorid is a group
        if (time.size() == numids)
        {   // if there is a time entry for each member of the group
            if (data.size() == numids)
            {   // if the data size matches the group size, then each member gets a single element
                for (size_t i=0; i<numids; i++)
                    add(*m_ids[ids[i]], time[i], data[i]);
            }
            else
            {   // if the data size doesn't match, then each member gets the entire vector
                for (size_t i=0; i<numids; i++)
                    add(*m_ids[ids[i]], time[i], data);
            }
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). ";
            debug << "time.size():" << time.size() << " must be ids.size():" << ids.size() << endl;
        }
    }
}

/*! @brief Adds a sequence of points to an actionatorid with a gain. 
           The size of the time and data must match, and in the case actionatorid is a group, they must also match the group size.
 
     For a single actionator the data needs to be formatted as [time0, time1, ... , timeN, [data0, data1, ... , dataN], gain
     For a group actionator the data needs to be formatted as [id0, id1, ... , idN] [time0, time1, ... , timeN] [data0, data1, ... , dataN], gain
     where in both cases gain is added to every point.
 
    @param actionatorid the id of the targetted actionator(s)
    @param time the sequence of times in ms
    @param data the sequence of data points
 */
void NUActionatorsData::add(const id_t& actionatorid, const vector<double>& time, const vector<float>& data, float gain)
{
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    
    if (numids == 0)
        return;
    else if (numids == 1)
    {   // if actionatorid is a single actionator
        if (time.size() == data.size())
        {
            for (size_t i=0; i<time.size(); i++)
                add(actionatorid, time[i], data[i], gain);
        }
        else
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). time and data need to have the same length when targeting a single actionator." << endl;
    }
    else
    {   // if actionatorid is a group
        if (time.size() == numids and data.size() == numids)
        {   // if there is an entry for each member of the group
            for (size_t i=0; i<numids; i++)
                add(*m_ids[ids[i]], time[i], data[i], gain);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "," << gain << "). ";
            debug << "time.size():" << time.size() << " and " << data.size() << " must be ids.size():" << ids.size() << endl;
        }
    }
}

/*! @brief Adds a sequence of [time,data,gain] points to an actionatorid
           The size of time, data and gain must be the same, in the case that actionatorid is a group, they must also match the group size.
    
     For a single actionator the data needs to be formatted as [time0, time1, ... , timeN, [data0, data1, ... , dataN], [gain0, gain1, ... , gainN]
     For a group actionator the data needs to be formatted as [id0, id1, ... , idN] [time0, time1, ... , timeN] [data0, data1, ... , dataN], [gain0, gain1, ... , gainN]
 
    @param actionatorid the id of the targetted actionator(s)
    @param time the sequence of times in ms
    @param data the sequence of data points
    @param gain the sequence of gain points (%)
 */
void NUActionatorsData::add(const id_t& actionatorid, const vector<double>& time, const vector<float>& data, const vector<float>& gain)
{
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    
    if (numids == 0)
        return;
    else if (numids == 1)
    {   // if actionatorid is a single actionator
        if (time.size() == data.size() and time.size() == gain.size())
        {
            for (size_t i=0; i<time.size(); i++)
                add(actionatorid, time[i], data[i], gain[i]);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "," << gain << "). ";
            debug << " time.size():" << time.size() << ", data.size():" << data.size() << "and gain.size():" << gain.size() << " must be the same." << endl;
        }
    }
    else
    {   // if actionatorid is a group
        if (time.size() == numids and data.size() == numids and gain.size() == numids)
        {   // if there is an entry for each member of the group
            for (size_t i=0; i<numids; i++)
                add(*m_ids[ids[i]], time[i], data[i], gain[i]);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "," << gain << "). ";
            debug << "time.size():" << time.size() << ", data.size():" << data.size() << " and gain.size():" << gain.size() << " must be ids.size():" << ids.size() << endl;
        }
    }
}

/*! @brief Adds a sequence of [time, vector] pairs to an actionatorid
 
    For a single actionator the data needs to be formatted as [time0, time1, ... , timeN, [data0, data1, ... , dataN], where each data is a vector.
    For a group actionator the data needs to
        id [time0, time1, ... , timeN], [data0, data1, ... , dataN], so each data element is given to the group with a single time
        [id0, id1, ... , idN] time [data0, data1, ... , dataN], so each actionator in the group receives a single data element at a sequence of times
        [id0, id1, ... , idN] [time0, time1, ... , timeN] data, so each actionator in the group receives the same data matrix at different times 
 
    @param actionatorid the id of the targetted actionator
    @param time the sequence of times in ms
    @param data the sequence of vectors to give to the actionator
 */
void NUActionatorsData::add(const id_t& actionatorid, const vector<double>& time, const vector<vector<float> >& data)
{
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    
    if (numids == 0)
        return;
    else if (numids == 1)
    {   // if actionatorid is a single
        if (time.size() == data.size())
        {
            for (size_t i=0; i<time.size(); i++)
                add(actionatorid, time[i], data[i]);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). ";
            debug << " time.size():" << time.size() << " and data.size():" << data.size() << " must be the same." << endl;
        }
    }
    else
    {   // if actionatorid is a group
        if (time.size() == data.size())
        {
            for (size_t i=0; i<time.size(); i++)
                add(actionatorid, time[i], data[i]);
        }
        else if (numids == data.size())
        {
            for (size_t i=0; i<numids; i++)
                add(*m_ids[ids[i]], time, data[i]);
        }
        else if (time.size() == numids)
        {
            for (size_t i=0; i<numids; i++)
                add(*m_ids[ids[i]], time[i], data);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). lolwut?";
        }
    }
}

/*! @brief Adds a sequence of [time, matrix] pairs to an actionatorid
 
         For a single actionator the data needs to be formatted as [time0, time1, ... , timeN, [data0, data1, ... , dataN], where each data is a vector.
         For a group actionator the data needs to
             id [time0, time1, ... , timeN], [data0, data1, ... , dataN], so each data element is given to the group with a single time
             [id0, id1, ... , idN] time [data0, data1, ... , dataN], so each actionator in the group receives a single data element at a sequence of times
             [id0, id1, ... , idN] [time0, time1, ... , timeN] data, so each actionator in the group receives the same data matrix at different times 
 
    @param actionatorid the id of the targetted actionator
    @param time the sequence of times in ms
    @param data the sequence of vectors to give to the actionator
 */
void NUActionatorsData::add(const id_t& actionatorid, const vector<double>& time, const vector<vector<vector<float> > >& data)
{
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    
    if (numids == 0)
        return;
    else if (numids == 1)
    {   // if actionatorid is a single
        if (time.size() == data.size())
        {
            for (size_t i=0; i<time.size(); i++)
                add(actionatorid, time[i], data[i]);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). ";
            debug << " time.size():" << time.size() << " and data.size():" << data.size() << " must be the same." << endl;
        }
    }
    else
    {   // if actionatorid is a group
        if (time.size() == data.size())
        {
            for (size_t i=0; i<time.size(); i++)
                add(actionatorid, time[i], data[i]);
        }
        else if (numids == data.size())
        {
            for (size_t i=0; i<numids; i++)
                add(*m_ids[ids[i]], time, data[i]);
        }
        else if (time.size() == numids)
        {
            for (size_t i=0; i<numids; i++)
                add(*m_ids[ids[i]], time[i], data);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). lolwut?" << endl;
        }
    }
}

/*! @brief Adds a sequence of [time, matrix] pairs to an actionatorid
 
     For a single actionator the data needs to be formatted as [time0, time1, ... , timeN, [data0, data1, ... , dataN], where each data is a vector.
     For a group actionator the data needs to
         id [time0, time1, ... , timeN], [data0, data1, ... , dataN], so each data element is given to the group with a single time
         [id0, id1, ... , idN] time [data0, data1, ... , dataN], so each actionator in the group receives a single data element at a sequence of times
         [id0, id1, ... , idN] [time0, time1, ... , timeN] data, so each actionator in the group receives the same data matrix at different times 
 
    @param actionatorid the id of the targetted actionator
    @param time the sequence of times in ms
    @param data the sequence of vectors to give to the actionator
 */
void NUActionatorsData::add(const id_t& actionatorid, const vector<double>& time, const vector<vector<vector<vector<float> > > >& data)
{
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    
    if (numids == 0)
        return;
    else if (numids == 1)
    {   // if actionatorid is a single
        if (time.size() == data.size())
        {
            for (size_t i=0; i<time.size(); i++)
                add(actionatorid, time[i], data[i]);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). ";
            debug << " time.size():" << time.size() << " and data.size():" << data.size() << " must be the same." << endl;
        }
    }
    else
    {   // if actionatorid is a group
        if (time.size() == data.size())
        {
            for (size_t i=0; i<time.size(); i++)
                add(actionatorid, time[i], data[i]);
        }
        else if (numids == data.size())
        {
            for (size_t i=0; i<numids; i++)
                add(*m_ids[ids[i]], time, data[i]);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). lolwut?" << endl;
        }
    }
}

/*! @brief Adds a sequence of strings to an actionatorid
 
         For a single actionator the data needs to be formatted as [time0, time1, ... , timeN, [string0, string1, ... , stringN].
         For a group actionator the data needs to be formatted as [id0, id1, ... , idN] [time0, time1, ... , timeN] [string0, string1, ... , stringN].
 
 @param actionatorid the id of the targetted actionator(s)
 @param time the sequence of times in ms
 @param data the sequence of strings
 */
void NUActionatorsData::add(const id_t& actionatorid, const vector<double>& time, const vector<string>& data)
{
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    
    if (numids == 0)
        return;
    else if (numids == 1)
    {   // if actionatorid is a single actionator
        if (time.size() == data.size())
        {
            for (size_t i=0; i<time.size(); i++)
                add(actionatorid, time[i], data[i]);
        }
        else
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). time and data need to have the same length when targeting a single actionator." << endl;
    }
    else
    {   // if actionatorid is a group
        if (time.size() == numids and data.size() == numids)
        {   // if there is an entry for each member of the group
            for (size_t i=0; i<numids; i++)
                add(*m_ids[ids[i]], time[i], data[i]);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). ";
            debug << "time.size():" << time.size() << " and data.size():" << data.size() << " must be ids.size():" << ids.size() << endl;
        }
    }
}

/*! @brief Adds a sequence of [vector<time>, matrix] pairs to an actionatorid
 
     For a group actionator the data needs to be formatted as [id0, id1, ... , idN] [time0, time1, ... , timeN], [data0, data1, ... , dataN], 
     so each data vector is given to the group with a single time vector
 
    @param actionatorid the id of the targetted actionator
    @param time the sequence of times in ms
    @param data the sequence of vectors to give to the actionator
 */
void NUActionatorsData::add(const id_t& actionatorid, const vector<vector<double> >& time, const vector<vector<float> >& data)
{
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    
    if (numids == 0)
        return;
    else if (numids == 1)
    {   // if actionatorid is a single actionator
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). Can not be used to add data to a single actionator." << endl;
    }
    else
    {   // if actionatorid is a group
        if (time.size() == numids and data.size() == numids)
        {   // if there is an entry for each member of the group
            for (size_t i=0; i<numids; i++)
                add(*m_ids[ids[i]], time[i], data[i]);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). ";
            debug << "time.size():" << time.size() << " and data.size():" << data.size() << " must be ids.size():" << ids.size() << endl;
        }
    }
}

/*! @brief Adds a sequence of [vector<time>, matrix] pairs with a single gain value to an actionatorid
 
     For a group actionator the data needs to be formatted as [id0, id1, ... , idN] [time0, time1, ... , timeN], [data0, data1, ... , dataN], 
     so each data vector is given to the group with a single time vector
     
    @param actionatorid the id of the targetted actionator
    @param time the sequence of times in ms
    @param data the sequence of vectors to give to the actionator
    @param gain the single gain value added to each data
 */
void NUActionatorsData::add(const id_t& actionatorid, const vector<vector<double> >& time, const vector<vector<float> >& data, float gain)
{
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    
    if (numids == 0)
        return;
    else if (numids == 1)
    {   // if actionatorid is a single actionator
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "," << gain << "). Can not be used to add data to a single actionator." << endl;
    }
    else
    {   // if actionatorid is a group
        if (time.size() == numids and data.size() == numids)
        {   // if there is an entry for each member of the group
            for (size_t i=0; i<numids; i++)
                add(*m_ids[ids[i]], time[i], data[i], gain);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "," << gain << "). ";
            debug << "time.size():" << time.size() << " and data.size():" << data.size() << " must be ids.size():" << ids.size() << endl;
        }
    }
}

/*! @brief Adds a sequence of [vector<time>, matrix] pairs with a single gain value to an actionatorid
 
        For a group actionator the data needs to be formatted as [id0, id1, ... , idN] [time0, time1, ... , timeN], [data0, data1, ... , dataN] [gain0, gain1, ..., gainN], 
        so each data vector is given to the group with a single time vector
 
    @param actionatorid the id of the targetted actionator
    @param time the sequence of times in ms
    @param data the sequence of vectors to give to the actionator
    @param gain the sequence of gains added to each data
 */
void NUActionatorsData::add(const id_t& actionatorid, const vector<vector<double> >& time, const vector<vector<float> >& data, const vector<float>& gain)
{
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    
    if (numids == 0)
        return;
    else if (numids == 1)
    {   // if actionatorid is a single actionator
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "," << gain << "). Can not be used to add data to a single actionator." << endl;
    }
    else
    {   // if actionatorid is a group
        if (time.size() == numids and data.size() == numids and gain.size() == numids)
        {   // if there is an entry for each member of the group
            for (size_t i=0; i<numids; i++)
                add(*m_ids[ids[i]], time[i], data[i], gain[i]);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "," << gain << "). ";
            debug << "time.size():" << time.size() << " and data.size():" << data.size() << " and gain.size():" << gain.size() << " must be ids.size():" << ids.size() << endl;
        }
    }
}

/*! @brief Adds a sequence of [vector<time>, matrix] pairs with a single gain value to an actionatorid
 
     For a group actionator the data needs to be formatted as [id0, id1, ... , idN] [time0, time1, ... , timeN], [data0, data1, ... , dataN] [gain0, gain1, ..., gainN], 
     so each data vector is given to the group with a single time vector
 
    @param actionatorid the id of the targetted actionator
    @param time the sequence of times in ms
    @param data the sequence of vectors to give to the actionator
    @param gain the sequence of gains added to each data
 */
void NUActionatorsData::add(const id_t& actionatorid, const vector<vector<double> >& time, const vector<vector<float> >& data, const vector<vector<float> >& gain)
{
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    
    if (numids == 0)
        return;
    else if (numids == 1)
    {   // if actionatorid is a single actionator
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "," << gain << "). Can not be used to add data to a single actionator." << endl;
    }
    else
    {   // if actionatorid is a group
        if (time.size() == numids and data.size() == numids and gain.size() == numids)
        {   // if there is an entry for each member of the group
            for (size_t i=0; i<numids; i++)
                add(*m_ids[ids[i]], time[i], data[i], gain[i]);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "," << gain << "). ";
            debug << "time.size():" << time.size() << " and data.size():" << data.size() << " and gain.size():" << gain.size() << " must be ids.size():" << ids.size() << endl;
        }
    }
}

/*! @brief Adds a sequence of [vector<time>, vector<matrix>] pairs with a single gain value to an actionatorid
 
     For a group actionator the data needs to be formatted as [id0, id1, ... , idN] [time0, time1, ... , timeN], [data0, data1, ... , dataN]
     so each data matrix is given to the group with a single time vector
 
    @param actionatorid the id of the targetted actionator
    @param time the sequence of times in ms
    @param data the sequence of matricies to give to the actionator
 */
void NUActionatorsData::add(const id_t& actionatorid, const vector<vector<double> >& time, const vector<vector<vector<float> > >& data)
{
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    
    if (numids == 0)
        return;
    else if (numids == 1)
    {   // if actionatorid is a single actionator
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). Can not be used to add data to a single actionator." << endl;
    }
    else
    {   // if actionatorid is a group
        if (time.size() == numids and data.size() == numids)
        {   // if there is an entry for each member of the group
            for (size_t i=0; i<numids; i++)
                add(*m_ids[ids[i]], time[i], data[i]);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). ";
            debug << "time.size():" << time.size() << " and data.size():" << data.size() << " must be ids.size():" << ids.size() << endl;
        }
    }
}

/*! @brief Adds a sequence of [vector<time>, vector<matrix>] pairs with a single gain value to an actionatorid
 
 For a group actionator the data needs to be formatted as [id0, id1, ... , idN] [time0, time1, ... , timeN], [data0, data1, ... , dataN]
 so each data matrix is given to the group with a single time vector
 
    @param actionatorid the id of the targetted actionator
    @param time the sequence of times in ms
    @param data the sequence of matricies to give to the actionator
 */
void NUActionatorsData::add(const id_t& actionatorid, const vector<vector<double> >& time, const vector<vector<vector<vector<float> > > >& data)
{
    vector<int>& ids = m_id_to_indices[actionatorid.Id];
    size_t numids = ids.size();
    
    if (numids == 0)
        return;
    else if (numids == 1)
    {   // if actionatorid is a single actionator
        debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). Can not be used to add data to a single actionator." << endl;
    }
    else
    {   // if actionatorid is a group
        if (time.size() == numids and data.size() == numids)
        {   // if there is an entry for each member of the group
            for (size_t i=0; i<numids; i++)
                add(*m_ids[ids[i]], time[i], data[i]);
        }
        else
        {
            debug << "NUActionatorsData::add(" << actionatorid.Name << "," << time << "," << data << "). ";
            debug << "time.size():" << time.size() << " and data.size():" << data.size() << " must be ids.size():" << ids.size() << endl;
        }
    }
}

/******************************************************************************************************************************************
 Displaying Contents and Serialisation
 ******************************************************************************************************************************************/

void NUActionatorsData::printMap(ostream& output)
{
    for (size_t j=0; j<m_id_to_indices.size(); j++)
    {
        output << m_ids[j]->Name << "->[";
        for (size_t k=0; k<m_id_to_indices[j].size(); k++)
            output << m_ids[m_id_to_indices[j][k]]->Name << " ";
        output << "]" << endl;
    }
}

void NUActionatorsData::printData(ostream& output)
{
    for (unsigned int i=0; i<m_available_actionators.size(); i++)
        m_actionators[m_available_actionators[i]].summaryTo(output);
}

void NUActionatorsData::summaryTo(ostream& output)
{
    printData(output);
}

void NUActionatorsData::csvTo(ostream& output)
{
    //! @todo TODO: implement this function    
}

ostream& operator<< (ostream& output, const NUActionatorsData& p_sensor)
{
    //! @todo TODO: implement this function
    return output;
}

istream& operator>> (istream& input, NUActionatorsData& p_sensor)
{
    //! @todo TODO: implement this function
    return input;
}

ostream& operator<<(ostream& output, const vector<float>& v)
{
    output << "[";
    if (not v.empty())
    {
    	for (size_t i=0; i<v.size()-1; i++)
            output << v[i] << ", ";
        output << v.back();
    }
    output << "]";
	return output;
}

ostream& operator<<(ostream& output, const vector<vector<float> >& v)
{
    output << "[";
    if (not v.empty())
    {
    	for (size_t i=0; i<v.size()-1; i++)
            output << v[i] << ", ";
        output << v.back();
    }
    output << "]";
	return output;
}

ostream& operator<<(ostream& output, const vector<vector<vector<float> > >& v)
{
    output << "[";
    if (not v.empty())
    {
    	for (size_t i=0; i<v.size()-1; i++)
            output << v[i] << ", ";
        output << v.back();
    }
    output << "]";
	return output;
}

ostream& operator<<(ostream& output, const vector<vector<vector<vector<float> > > >& v)
{
    output << "[";
    if (not v.empty())
    {
    	for (size_t i=0; i<v.size()-1; i++)
            output << v[i] << ", ";
        output << v.back();
    }
    output << "]";
	return output;
}

ostream& operator<<(ostream& output, const vector<double>& v)
{
    output << "[";
    if (not v.empty())
    {
    	for (size_t i=0; i<v.size()-1; i++)
            output << v[i] << ", ";
        output << v.back();
    }
    output << "]";
	return output;
}

ostream& operator<<(ostream& output, const vector<vector<double> >& v)
{
    output << "[";
    if (not v.empty())
    {
    	for (size_t i=0; i<v.size()-1; i++)
            output << v[i] << ", ";
        output << v.back();
    }
    output << "]";
	return output;
}

ostream& operator<<(ostream& output, const vector<string>& v)
{
    output << "[";
    if (not v.empty())
    {
    	for (size_t i=0; i<v.size()-1; i++)
            output << v[i] << ", ";
        output << v.back();
    }
    output << "]";
	return output;
}

istream& operator>>(istream& input, vector<float>& v)
{
    string wholevector;
    // get all of the data between [ ... ]
    input.ignore(128, '[');
    getline(input, wholevector, ']');
    
    v.clear();
    stringstream ss(wholevector);
    float floatBuffer;
    
    // now split the data based on the commas
    while (not ss.eof())
    {
        ss >> floatBuffer;
        ss.ignore(128, ',');
        v.push_back(floatBuffer);
    }
    
    return input;
}

