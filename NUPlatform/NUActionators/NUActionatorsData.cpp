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
#include "Tools/debug.h"

NUActionatorsData::joint_id_t NUActionatorsData::HeadYaw = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::HeadPitch = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::LShoulderPitch = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::LShoulderRoll = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::LElbowYaw = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::LElbowRoll = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::RShoulderPitch = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::RShoulderRoll = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::RElbowYaw = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::RElbowRoll = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::TorsoYaw = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::TorsoPitch = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::TorsoRoll = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::LHipYaw = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::LHipYawPitch = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::LHipPitch = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::LHipRoll = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::LKneePitch = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::LAnklePitch = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::LAnkleRoll = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::RHipYaw = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::RHipYawPitch = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::RHipPitch = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::RHipRoll = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::RKneePitch = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::RAnklePitch = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::joint_id_t NUActionatorsData::RAnkleRoll = NUActionatorsData::ACTIONATOR_MISSING;

NUActionatorsData::led_id_t NUActionatorsData::LEar = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::led_id_t NUActionatorsData::REar = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::led_id_t NUActionatorsData::LEye = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::led_id_t NUActionatorsData::REye = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::led_id_t NUActionatorsData::Chest = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::led_id_t NUActionatorsData::LFoot = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::led_id_t NUActionatorsData::RFoot = NUActionatorsData::ACTIONATOR_MISSING;

NUActionatorsData::camera_setting_id_t NUActionatorsData::Resolution = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::camera_setting_id_t NUActionatorsData::FramesPerSecond = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::camera_setting_id_t NUActionatorsData::AutoExposure = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::camera_setting_id_t NUActionatorsData::AutoWhiteBalance = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::camera_setting_id_t NUActionatorsData::AutoGain = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::camera_setting_id_t NUActionatorsData::Brightness = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::camera_setting_id_t NUActionatorsData::Saturation = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::camera_setting_id_t NUActionatorsData::RedChroma = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::camera_setting_id_t NUActionatorsData::BlueChroma = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::camera_setting_id_t NUActionatorsData::Gain = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::camera_setting_id_t NUActionatorsData::Exposure = NUActionatorsData::ACTIONATOR_MISSING;
NUActionatorsData::camera_setting_id_t NUActionatorsData::SelectCamera = NUActionatorsData::ACTIONATOR_MISSING;

/*! @brief Default constructor for a NUActionatorsData storage class.
 */
NUActionatorsData::NUActionatorsData()
{
#if DEBUG_NUSENSORS_VERBOSITY > 4
    debug << "NUActionatorsData::NUActionatorsData" << endl;
#endif
    m_positionactionation = false;
    m_torqueactionation = false;
    
    m_num_head_joints = 0;
    m_num_arm_joints = 0;
    m_num_torso_joints = 0;
    m_num_leg_joints = 0;
    m_num_body_joints = 0;
    m_num_joints = 0;
}

NUActionatorsData::~NUActionatorsData()
{
    m_all_actionators.clear();
    m_head_ids.clear();
    m_larm_ids.clear();
    m_rarm_ids.clear();
    m_torso_ids.clear();
    m_lleg_ids.clear();
    m_rleg_ids.clear();
}

/******************************************************************************************************************************************
 Initialisation and Availability Setting Methods
 ******************************************************************************************************************************************/

/*! @brief Sets the available joint control methods, that is whether the joints can be position, torque or both controlled.
 @param methodnames a vector of strings where each string names a method
 */
void NUActionatorsData::setAvailableJointControlMethods(const vector<string>& methodnames)
{
    vector<string> simplemethodnames;
    simplifyNames(methodnames, simplemethodnames);
    
    for (int i=0; i<simplemethodnames.size(); i++)
    {
        if (simplemethodnames[i].compare("position") || simplemethodnames[i].compare("positions") || simplemethodnames[i].compare("jointposition") || simplemethodnames[i].compare("jointpositions"))
            m_positionactionation = true;
        else if (simplemethodnames[i].compare("torque") || simplemethodnames[i].compare("torques") || simplemethodnames[i].compare("jointtorque") || simplemethodnames[i].compare("jointtorques"))
            m_torqueactionation = true;
        else
            debug << "NUActionatorsData::setAvailableJointControlMethods. You have specified an unrecognised joint control method: " << methodnames[i] << endl;
    }
}


/*! @brief Adds the joint actionators and sets each of the static joint_id_t if the joint is in the list. Also sets id lists for accessing limbs. 
 @param jointnames a vector of strings where each string is a name of a joint
 */
void NUActionatorsData::setAvailableJoints(const vector<string>& jointnames)
{
    // NOTE: This has been copied directly from NUSensorsData; so if your changing this you probably need to change that as well!
    vector<string> simplejointnames;
    simplifyNames(jointnames, simplejointnames);
    
    for (int i=0; i<simplejointnames.size(); i++) 
    {
        addJointActionator(jointnames[i]);
        if (simplejointnames[i].compare("headyaw") == 0)
        {
            HeadYaw = i;
            m_head_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("headpitch") == 0)
        {
            HeadPitch = i;
            m_head_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("lshoulderpitch") == 0)
        {
            LShoulderPitch = i;
            m_larm_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("lshoulderroll") == 0)
        {
            LShoulderRoll = i;
            m_larm_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("lelbowyaw") == 0)
        {
            LElbowYaw = i;
            m_larm_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("lelbowroll") == 0)
        {
            LElbowRoll = i;
            m_larm_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("rshoulderpitch") == 0)
        {
            RShoulderPitch = i;
            m_rarm_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("rshoulderroll") == 0)
        {
            RShoulderRoll = i;
            m_rarm_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("relbowyaw") == 0)
        {
            RElbowYaw = i;
            m_rarm_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("relbowroll") == 0)
        {
            RElbowRoll = i;
            m_rarm_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("torsoyaw") == 0)
        {
            TorsoYaw = i;
            m_torso_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("torsopitch") == 0)
        {
            TorsoPitch = i;
            m_torso_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("torsoroll") == 0)
        {
            TorsoRoll = i;
            m_torso_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("lhipyaw") == 0)
        {
            LHipYaw = i;
            m_lleg_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("lhipyawpitch") == 0)
        {
            LHipYawPitch = i;
            m_lleg_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("lhippitch") == 0)
        {
            LHipPitch = i;
            m_lleg_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("lhiproll") == 0)
        {
            LHipRoll = i;
            m_lleg_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("lkneepitch") == 0)
        {
            LKneePitch = i;
            m_lleg_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("lanklepitch") == 0)
        {
            LAnklePitch = i;
            m_lleg_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("lankleroll") == 0)
        {
            LAnkleRoll = i;
            m_lleg_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("rhipyaw") == 0)
        {
            RHipYaw = i;
            m_lleg_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("rhipyawpitch") == 0)
        {
            RHipYawPitch = i;
            m_lleg_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("rhippitch") == 0)
        {
            RHipPitch = i;
            m_lleg_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("rhiproll") == 0)
        {
            RHipRoll = i;
            m_lleg_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("rkneepitch") == 0)
        {
            RKneePitch = i;
            m_lleg_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("ranklepitch") == 0)
        {
            RAnklePitch = i;
            m_lleg_ids.push_back(i);
        }
        else if (simplejointnames[i].compare("rankleroll") == 0)
        {
            RAnkleRoll = i;
            m_lleg_ids.push_back(i);
        }
        else 
        {
            debug << "NUActionatorsData::setAvailableJoints. This platform has an unrecognised joint: " << jointnames[i] << endl;
        }
        
    }
    // add the arms, torso and legs to the body_ids
    m_body_ids.insert(m_body_ids.end(), m_larm_ids.begin(), m_larm_ids.end());
    m_body_ids.insert(m_body_ids.end(), m_rarm_ids.begin(), m_rarm_ids.end());
    m_body_ids.insert(m_body_ids.end(), m_torso_ids.begin(), m_torso_ids.end());
    m_body_ids.insert(m_body_ids.end(), m_lleg_ids.begin(), m_lleg_ids.end());
    m_body_ids.insert(m_body_ids.end(), m_rleg_ids.begin(), m_rleg_ids.end());
    // add the head and the body_ids to the all_joint_ids
    m_all_joint_ids.insert(m_all_joint_ids.end(), m_head_ids.begin(), m_head_ids.end());
    m_all_joint_ids.insert(m_all_joint_ids.end(), m_body_ids.begin(), m_body_ids.end());
    
    // set total numbers in each limb
    m_num_head_joints = m_head_ids.size();
    m_num_arm_joints = m_larm_ids.size();
    m_num_torso_joints = m_torso_ids.size();
    m_num_leg_joints = m_lleg_ids.size();
    m_num_body_joints = m_body_ids.size();
    m_num_joints = m_all_joint_ids.size();
}

/*! @brief Adds the led actionators and sets each of the static led_id_t if the led is in the list.
 @param leds a vector of strings where each string is a name of a led
 */
void NUActionatorsData::setAvailableLeds(const vector<string>& lednames)
{
    vector<string> simplelednames;
    simplifyNames(lednames, simplelednames);
    
    for (int i=0; i<simplelednames.size(); i++) 
    {
        addLedActionator(lednames[i]);
        if (simplelednames[i].compare("lear") == 0 || simplelednames[i].compare("earsledleft") == 0)
            LEar = i;
        else if (simplelednames[i].compare("rear") == 0 || simplelednames[i].compare("earsledright") == 0)
            REar = i;
        else if (simplelednames[i].compare("leye") == 0 || simplelednames[i].compare("faceledleft") == 0)
            LEye = i;
        else if (simplelednames[i].compare("reye") == 0 || simplelednames[i].compare("faceledright") == 0)
            REye = i;
        else if (simplelednames[i].compare("chest") == 0 || simplelednames[i].compare("chestboardled") == 0)
            Chest = i;
        else if (simplelednames[i].compare("lfoot") == 0 || simplelednames[i].compare("lfootled") == 0)
            LFoot = i;
        else if (simplelednames[i].compare("rfoot") == 0 || simplelednames[i].compare("rfootled") == 0)
            RFoot = i;
    }
}

/*! @brief Adds the camera setting actionator and sets each of the static camera_setting_id_t if the setting is in the list.
 @param camerasettingnames a vector of strings where each string is a name of a setting
 */
void NUActionatorsData::setAvailableCameraSettings(const vector<string>& camerasettingnames)
{
    vector<string> simplecamerasettingnames;
    simplifyNames(camerasettingnames, simplecamerasettingnames);
    
    for (int i=0; i<simplecamerasettingnames.size(); i++) 
    {
        addCameraSettingActionator(camerasettingnames[i]);
        if (simplecamerasettingnames[i].compare("resolution") == 0)
            Resolution = i;
        else if (simplecamerasettingnames[i].compare("framespersecond") == 0 || simplecamerasettingnames[i].compare("fps") == 0 )
            FramesPerSecond = i;
        else if (simplecamerasettingnames[i].compare("autoexposure") == 0)
            AutoExposure = i;
        else if (simplecamerasettingnames[i].compare("autowhitebalance") == 0)
            AutoWhiteBalance = i;
        else if (simplecamerasettingnames[i].compare("autogain") == 0)
            AutoGain = i;
        else if (simplecamerasettingnames[i].compare("brightness") == 0)
            Brightness = i;
        else if (simplecamerasettingnames[i].compare("saturation") == 0)
            Saturation = i;
        else if (simplecamerasettingnames[i].compare("redchroma") == 0)
            RedChroma = i;
        else if (simplecamerasettingnames[i].compare("bluechroma") == 0)
            BlueChroma = i;
        else if (simplecamerasettingnames[i].compare("gain") == 0)
            Gain = i;
        else if (simplecamerasettingnames[i].compare("exposure") == 0)
            Exposure = i;
        else if (simplecamerasettingnames[i].compare("selectcamera") == 0)
            SelectCamera = i;
        else
            debug << "NUActionatorsData::setAvailableCameraSettings. You have added an unrecognised camera setting: " << camerasettingnames[i] << endl;
    }
}

/*! @brief Sets the available actionators based on the names found in the passed in strings
 
 @param actionators a vector of names for each of the available actionators.
 */
void NUActionatorsData::setAvailableOtherActionators(const vector<string>& actionatornames)
{
    vector<string> simpleactionatornames;
    simplifyNames(actionatornames, simpleactionatornames);
    
    for (int i=0; i<simpleactionatornames.size(); i++) 
    {
        if (simpleactionatornames[i].compare("sound") == 0)
            addActionator(Sound, actionatornames[i], actionator_t::SOUND);
        else
            debug << "NUActionatorsData::setAvailableOtherActionators. You have added an unrecognised other actionator: " << actionatornames[i] << endl;
    }
}

/*! @brief Adds a joint actionator with the specified name
 
 @param actionatorname the name of the actionator to be added
 */
void NUActionatorsData::addJointActionator(string actionatorname)
{
    if (m_positionactionation == true)
        addActionator(PositionActionators, actionatorname, actionator_t::JOINT_POSITION);
    if (m_torqueactionation == true)
        addActionator(TorqueActionators, actionatorname, actionator_t::JOINT_TORQUE);
}

/*! @brief Adds a camera setting actionator with the specified name
 
 @param actionatorname the name of the actionator to be added
 */
void NUActionatorsData::addCameraSettingActionator(string actionatorname)
{
    addActionator(CameraActionators, actionatorname, actionator_t::CAMERA_SETTING);
}

/*! @brief Adds a led actionator with the specified name
 
 @param actionatorname the name of the actionator to be added
 */
void NUActionatorsData::addLedActionator(string actionatorname)
{
    addActionator(LedActionators, actionatorname, actionator_t::LEDS);
}

/*! @brief Adds an actionator to the actionator group with the specified name and type
 
 @param actionatorgroup the vector of similar actionator to which this one will be added
 @param actionatorname the name of the actionator to be added
 @param actionatortype the type of the actionator to be added
 */
void NUActionatorsData::addActionator(vector<actionator_t*>& actionatorgroup, string actionatorname, actionator_t::actionator_type_t actionatortype)
{
    actionator_t* newactionator = new actionator_t(actionatorname, actionatortype);
    actionatorgroup.push_back(newactionator);
    m_all_actionators.push_back(newactionator);
}

/*! @brief Adds an actionator with the specified name and type
 
 @param p_actionator the actionator pointer for the new actionator
 @param actionatorname the name of the actionator to be added
 @param actionatortype the type of the actionator to be added
 */
void NUActionatorsData::addActionator(actionator_t*& p_actionator, string actionatorname, actionator_t::actionator_type_t actionatortype)
{
    p_actionator = new actionator_t(actionatorname, actionatortype);
    m_all_actionators.push_back(p_actionator);
}

/*! @brief Simplifies a name
 @param input the name to be simplified
 @return the simplified string
 */
string NUActionatorsData::simplifyName(const string& input)
{
    string namebuffer, currentletter;
    // compare each letter to a space and an underscore and a forward slash
    for (int j=0; j<input.size(); j++)
    {
        currentletter = input.substr(j, 1);
        if (currentletter.compare(string(" ")) != 0 && currentletter.compare(string("_")) != 0 && currentletter.compare(string("/")) != 0 && currentletter.compare(string("\\")) != 0 && currentletter.compare(string(".")) != 0)
            namebuffer += tolower(currentletter[0]);            
    }
    return namebuffer;
}

/*! @brief Simplifies a vector of strings
 
 @param input the vector of strings to be simplified
 @param ouput the vector that will be updated to contain the simplified names
 */
void NUActionatorsData::simplifyNames(const vector<string>& input, vector<string>& output)
{
    vector<string> simplifiednames;
    for (int i=0; i<input.size(); i++)
        simplifiednames.push_back(simplifyName(input[i]));
    output = simplifiednames;
}

/******************************************************************************************************************************************
 Get Methods
 ******************************************************************************************************************************************/

/*! @brief Remove all of the completed actionator points
 @param currenttime all actionator points that have times before this one are assumed to have been completed, and they will be removed
 */
void NUActionatorsData::removeCompletedPoints(double currenttime)
{
    for (int i=0; i<m_all_actionators.size(); i++)
        m_all_actionators[i]->removeCompletedPoints(currenttime);
}

/*! @brief Gets the next position control point
    
    @param isvalid a vector of bools that indicates whether there is a new target for each joint.
    @param time the time each position control should be completed will be put in this vector
    @param positions the target position for each joint will be put in this vector
    @param velocities the target velocities for each joint will be put in this vector
    @param gains the target gains for each joint will be put in this vector
 */
bool NUActionatorsData::getNextJointPositions(vector<bool>& isvalid, vector<double>& time, vector<float>& positions, vector<float>& velocities, vector<float>& gains)
{
    static int l_num_joints = PositionActionators.size();
    static vector<bool> l_isvalid(l_num_joints, false);
    static vector<double> l_time(l_num_joints, 0);
    static vector<float> l_positions(l_num_joints, 0);
    static vector<float> l_velocities(l_num_joints, 0);
    static vector<float> l_gains(l_num_joints, 0);
    
    // loop through each actionator in PostionActionators looking for non-empty actionators with the right datalength
    for (int i=0; i<l_num_joints; i++)
    {
        if(PositionActionators[i]->isEmpty() || PositionActionators[i]->m_points[0]->Data.size() != 3)
        {
            l_isvalid[i] = false;
        }
        else
        {
            l_isvalid[i] = true;
            l_time[i] = PositionActionators[i]->m_points[0]->Time;
            l_positions[i] = PositionActionators[i]->m_points[0]->Data[0];
            l_velocities[i] = PositionActionators[i]->m_points[0]->Data[1];
            l_gains[i] = PositionActionators[i]->m_points[0]->Data[2];
        }
    }
    
    // now copy the results to the output vectors
    isvalid = l_isvalid;
    time = l_time;
    positions = l_positions;
    velocities = l_velocities;
    gains = l_gains;
    
    if (l_num_joints > 0)
        return true;
    else
        return false;
}

/*! @brief Gets the next torque control point
 
 @param isvalid a vector of bools that indicates whether there is a new target for each joint.
 @param time the time each torque control should be completed will be put in this vector
 @param torques the target torques for each joint will be put in this vector
 @param gains the target gains for each joint will be put in this vector
 */
bool NUActionatorsData::getNextJointTorques(vector<bool>& isvalid, vector<double>& time, vector<float>& torques, vector<float>& gains)
{
    static int l_num_joints = TorqueActionators.size();
    static vector<bool> l_isvalid(l_num_joints, false);
    static vector<double> l_time(l_num_joints, 0);
    static vector<float> l_torques(l_num_joints, 0);
    static vector<float> l_gains(l_num_joints, 0);
    
    // loop through each actionator in TorqueActionators looking for non-empty actionators with the right datalength
    for (int i=0; i<l_num_joints; i++)
    {
        if(TorqueActionators[i]->isEmpty() || TorqueActionators[i]->m_points[0]->Data.size() != 2)
        {
            l_isvalid[i] = false;
        }
        else
        {
            l_isvalid[i] = true;
            l_time[i] = TorqueActionators[i]->m_points[0]->Time;
            l_torques[i] = TorqueActionators[i]->m_points[0]->Data[0];
            l_gains[i] = TorqueActionators[i]->m_points[0]->Data[1];
        }
    }
    
    // now copy the results to the output vectors
    isvalid = l_isvalid;
    time = l_time;
    torques = l_torques;
    gains = l_gains;
    
    if (l_num_joints > 0)
        return true;
    else
        return false;
}

bool NUActionatorsData::getNextCameraControl(vector<bool>& isvalid, vector<double>& time, vector<float>& data)
{
    //!< @todo TODO: implement this function. Or decide whether it needs to be implemented
    return false;
}

/*! @brief Gets the next led point
 
 @param isvalid a vector of bools that indicates whether there is a new target for each led.
 @param time the time each led should be completed will be put in this vector
 @param redvalues the target red value for each led will be put in this vector
 @param greenvalues the target green value for each led will be put in this vector
 @param bluevalues the target blue value for each led will be put in this vector
 */
bool NUActionatorsData::getNextLeds(vector<bool>& isvalid, vector<double>& time, vector<float>& redvalues, vector<float>& greenvalues, vector<float>& bluevalues)
{
    static int l_num_leds = LedActionators.size();
    static vector<bool> l_isvalid(l_num_leds, false);
    static vector<double> l_time(l_num_leds, 0);
    static vector<float> l_redvalues(l_num_leds, 0);
    static vector<float> l_greenvalues(l_num_leds, 0);
    static vector<float> l_bluevalues(l_num_leds, 0);
    
    // loop through each actionator in LedActionators looking for non-empty actionators with the right datalength
    for (int i=0; i<l_num_leds; i++)
    {
        if(LedActionators[i]->isEmpty() || LedActionators[i]->m_points[0]->Data.size() != 3)
        {
            l_isvalid[i] = false;
        }
        else
        {
            l_isvalid[i] = true;
            l_time[i] = LedActionators[i]->m_points[0]->Time;
            l_redvalues[i] = LedActionators[i]->m_points[0]->Data[0];
            l_greenvalues[i] = LedActionators[i]->m_points[0]->Data[1];
            l_bluevalues[i] = LedActionators[i]->m_points[0]->Data[2];
        }
    }
    
    // now copy the results to the output vectors
    isvalid = l_isvalid;
    time = l_time;
    l_redvalues = redvalues;
    l_greenvalues = greenvalues;
    l_bluevalues = bluevalues;
    
    if (l_num_leds > 0)
        return true;
    else
        return false;
}

bool NUActionatorsData::getNextSound(bool& isvalid, double& time, int& soundid, string& text)
{
    //!< @todo TODO: implement this function
    return false;
}


/******************************************************************************************************************************************
 Set Methods
 ******************************************************************************************************************************************/

bool NUActionatorsData::addJointPosition(joint_id_t jointid, double time, float position, float velocity, float gain)
{
}

bool NUActionatorsData::addJointTorque(joint_id_t jointid, double time, float torque, float gain)
{
}

bool NUActionatorsData::addJointPositions(bodypart_id_t partid, double time, const vector<float>& positions, const vector<float>& velocities, const vector<float>& gains)
{
}

bool NUActionatorsData::addJointTorques(bodypart_id_t partid, double time, const vector<float>& torques, const vector<float>& gains)
{
}

/******************************************************************************************************************************************
 Displaying Contents and Serialisation
 ******************************************************************************************************************************************/

void NUActionatorsData::summaryTo(ostream& output)
{
    if (m_all_actionators.size() == 0)
        output << "NONE!" << endl;
    for (int i=0; i<m_all_actionators.size(); i++)
        m_all_actionators[i]->summaryTo(output);
}

void NUActionatorsData::csvTo(ostream& output)
{
    //! @todo TODO: implement this function    
}

ostream& operator<< (ostream& output, const NUActionatorsData& p_sensor)
{
    //! @todo TODO: implement this function
}

istream& operator>> (istream& input, NUActionatorsData& p_sensor)
{
    //! @todo TODO: implement this function
}


