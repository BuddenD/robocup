/*! @file actionator_t.h
    @brief Declaration of a single set of actionators class
    @author Jason Kulk
 
    @class actionator_t
    @brief A single set of actionators class to store actuator data in a platform independent way
 
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

#ifndef ACTIONATOR_T_H
#define ACTIONATOR_T_H

#include <vector>
#include <string>
using namespace std;

class actionator_t 
{
public:
    /*! @brief A simple struct for storing an actionator control point
     */
    struct actionator_point_t 
    {
        double Time;                //!< a single time for each actuator point
        vector<bool> IsValid;       //!< use this flag to effect only selected subactionators in this group    
        vector<float> Values;       //!< the actual values to be given to the group of actionators
        vector<bool> IsGainValid;   //!< use this flag to only change the gain on selected subactionators in this group
        vector<float> Gains;        //!< the gains/strength/stiffness/brightness etc for the actionator if applicable
    };
    
    /*! @brief A enum type to perform run time actionator type checking without using
     string compares. Unfortunately, to add a new actionator you need to add an
     id to this list.
     */
    enum actionator_id_t 
    {
        JOINT_POSITIONS,
        JOINT_VELOCITIES,
        JOINT_CURRENTS,
        JOINT_TORQUES,
        CAMERA_CONTROL,
        LEDS,
        SOUND,
        UNDEFINED
    };
public:
    actionator_t();
    actionator_t(string actionatorname, actionator_id_t actionatorid);

    void addActionGains(double time, const vector<bool>& isvalid, const vector<float>& gains);
    void addActionValues(double time, const vector<bool>& isvalid, const vector<float>& values);
    void addAction(double time, const vector<bool>& isvalid, const vector<float>& values, const vector<float>& gains);
    void addAction(double time, const vector<bool>& isvalid, const vector<float>& values, const vector<bool>& isgainvalid, const vector<float>& gains);
    
    void removeCompleted(double currenttime);
    
    void summaryTo(ostream& output);
    void csvTo(ostream& output);
    
    friend ostream& operator<< (ostream& output, const actionator_t& p_actionator);
    friend istream& operator>> (istream& input, actionator_t& p_actionator);
public:
    string Name;                                //!< the name of the actionator group
    actionator_id_t ActionatorID;               //!< the actionator id
    vector<actionator_point_t*> m_points;       //!< the actionator points
    bool IsAvailable;                           //!< true if the actionator is avaliable, false if it is absent
};

#endif

