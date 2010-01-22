/*! @file WalkOptimiserBehaviour.cpp
    @brief Implementation of WalkOptimiserBehaviour class

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

#include "WalkOptimiserBehaviour.h"

#include "NUPlatform/NUSystem.h"
#include "Tools/debug.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

//! @todo TODO: put M_PI and NORMALISE somewhere else
#ifndef M_PI
    #define M_PI 3.1415926535
#endif

template <class T>
inline T NORMALISE(T theta){
    return atan2(sin(theta), cos(theta));
}

/*! @brief Constructs a walk optimiser
    @param p_platform I need this to access the player number and team number. I think that should be moved into sensorsdata at some stage
 */
WalkOptimiserBehaviour::WalkOptimiserBehaviour(NUPlatform* p_platform, NUWalk* p_walk)
{
    // get initial walk parameters from the walk engine itself.
    m_walk = p_walk;
    m_walk->getWalkParameters(m_walk_parameters);
    m_metric_type = Cost;
    if (m_metric_type == Speed || m_metric_type == SpeedAndPushes)
        m_optimiser = new WalkOptimiser(m_walk_parameters, false);
    else
        m_optimiser = new WalkOptimiser(m_walk_parameters);

    int playernum, teamnum;
    p_platform->getNumber(playernum);
    p_platform->getTeamNumber(teamnum);
    // check to see if there is an existing saved optimiser. I do this by trying to open it
    stringstream filenamestream;
    filenamestream << "optimiser" << teamnum << playernum << ".log";
    m_saved_optimiser_filename = filenamestream.str();
    loadOptimiser();
    
    // specify respawn location based on the player and team number
    m_respawn_x = -270;
    m_respawn_bearing = 0;
    if (teamnum == 0)
    {
        if (playernum == 1)
            m_respawn_y = 150;
        else
            m_respawn_y = 50;
    }
    else 
    {
        if (playernum == 1)
            m_respawn_y = -50;
        else
            m_respawn_y = -150;
    }
    
    // start the behaviour in the initial state
    m_state = Initial;
    m_previous_state = m_state;
    m_target_speed = 0.1;
    m_target_trial_duration = 20000;
    
    m_trial_energy_used = 0;
    m_trial_perturbation_mag = 0;
}

WalkOptimiserBehaviour::~WalkOptimiserBehaviour()
{
    if (m_optimiser != NULL)
        delete m_optimiser;
}

/*! @brief Processes the sensor data, and produces actions (if necessary)
    @param data the sensorsdata
    @param actions the actionatorsdata
 */
void WalkOptimiserBehaviour::process(NUSensorsData* data, NUActionatorsData* actions)
{   
    if (data == NULL || actions == NULL)
        return;
    m_data = data;
    m_actions = actions;
    
    if (m_state == MeasureCost)
        measureCost();
    else if (m_state == MeasureRobust)
        measureRobust();
}

/*! @brief Jobs created by the WalkOptimiserBehaviour are added here.
 
    The behaviour has 5 states:
        1. Initial where we are just waiting for the simulator to go into 'playing'
        2. StartCost where we accelerate the robot up to the maximum speed in preparation for the measuring the cost
        3. MeasureCost where we actually perform the measurement of the cost
        4. StartRobust where we accelerate the robot up to maximim speed in preparation for measuring the robustness
        5. MeasureRobust where we actually perform the measurement of the robustness
 
    @param joblist the list of jobs to which job(s) will be added
 */
void WalkOptimiserBehaviour::process(JobList& joblist)
{
    if (m_data == NULL || m_actions == NULL)
        return;

    m_previous_state = m_state;
    if (m_state == Initial)
    {   // In the initial state we wait until the simulator puts the game in 'playing'
        if (m_data->CurrentTime > 15000)
        {
            cout << "Starting Optimisation" << endl;
            m_state = StartCost;
            respawn();
        }
    }
    else if (m_data->isFallen())
    {
        if (m_state == MeasureRobust)
            finishMeasureRobust();
        else
            respawn();
        m_target_speed = 0.1;
        m_state = StartCost;
    }
    else 
    {
        if (m_state == StartCost || m_state == StartRobust)
        {   // accelerate up to 'target' speed
            const float acceleration = 0.75/25.0;       // the acceleration in cm/s/s
            static vector<float> speed(3,0);
            static WalkJob* walkjob = new WalkJob(speed);
            if (m_state == StartCost)
                m_target_speed += acceleration;
            else
                m_target_speed += 3.0*acceleration;
            
            if (m_target_speed > m_walk_parameters[0])
            {
                m_target_speed = m_walk_parameters[0];
                if (m_state == StartCost) 
                {
                    cout << "Starting MeasureCost" << endl;
                    m_state = MeasureCost;
                }
                else 
                {
                    cout << "Starting MeasureRobust" << endl;
                    m_state = MeasureRobust;
                }
                startTrial();
            }

            speed[0] = m_target_speed;
            walkjob->setSpeed(speed);
            joblist.addMotionJob(walkjob);
        }
        else if (m_state == MeasureCost)
        {
            if ((m_data->CurrentTime - m_trial_start_time) > m_target_trial_duration)
            {
                m_state = StartRobust;
                finishMeasureCost();
            }
        }
        else if (m_state == MeasureRobust)
        {
            if ((m_data->CurrentTime - m_trial_start_time) > 2*m_target_trial_duration)
            {
                m_state = StartCost;
                finishMeasureRobust();
            }
        }
    }
}

/*! @brief Teleports the robot back to its starting position. The robot is always placed upright
 */
void WalkOptimiserBehaviour::respawn()
{
    m_target_speed = 0;
    m_actions->addTeleportation(m_data->CurrentTime, m_respawn_x, m_respawn_y, m_respawn_bearing);
    m_optimiser->getNewParameters(m_walk_parameters);
    m_walk->setWalkParameters(m_walk_parameters);
}

/*! @brief Start the trial
 */
void WalkOptimiserBehaviour::startTrial()
{
    m_trial_start_time = m_data->CurrentTime;
    static vector<float> gps(3,0);
    m_data->getGPSValues(gps);
    m_trial_start_x = gps[0];
    m_trial_start_y = gps[1];
    
    m_trial_energy_used = 0;
    m_trial_perturbation_mag = 0;
    m_perturbation_direction = -1;
}

/*! @brief Measures the cost of transport of the robot while it is walking
 
    m_trial_energy_used is incremented by the amount of energy used in this cycle
 */
void WalkOptimiserBehaviour::measureCost()
{
    static vector<float> previouspositions;
    static vector<float> positions;
    static vector<float> torques;
    m_data->getJointPositions(NUSensorsData::BodyJoints, positions);
    m_data->getJointTorques(NUSensorsData::BodyJoints, torques);
    
    if (previouspositions.size() != 0)
    {
        for (int i=0; i<positions.size(); i++)
            m_trial_energy_used += fabs(torques[i]*(positions[i] - previouspositions[i]));
    }
    
    previouspositions = positions;
    perturbRobot();
}

/*! @brief Finish the cost measurement
 
    m_measured_speed and m_measured_cost are updated and the robot is respawned. The optimiser is not updated at this stage.
 */
void WalkOptimiserBehaviour::finishMeasureCost()
{
    cout << "Finishing Cost Measurement" << endl;
    static vector<float> gps(3,0);
    m_data->getGPSValues(gps);
    float distance = sqrt(pow(gps[0] - m_trial_start_x,2) + pow(gps[1] - m_trial_start_y,2));
    float time = (m_data->CurrentTime - m_trial_start_time)/1000.0;
    if (time < 15)
        time = 15;
    m_measured_speed = distance/time;
    m_measured_cost = (2*m_trial_energy_used+20*time)*9.81*4.8/(distance*100);       // assume CPU draws 20W and the motor gears are 50% efficient
    respawn();
}

/*! @brief Measure the robustness of the walk
 */
void WalkOptimiserBehaviour::measureRobust()
{
    perturbRobot();
}

/*! @brief Finish the robustness measurement
 */
void WalkOptimiserBehaviour::finishMeasureRobust()
{
    cout << "Finishing Robust Measurement" << endl;
    static vector<float> gps(3,0);
    m_data->getGPSValues(gps);
    m_measured_robustness = sqrt(pow(gps[0] - m_trial_start_x,2) + pow(gps[1] - m_trial_start_y,2));
    
    if (m_metric_type == Speed)
        tickOptimiser(m_measured_speed);
    else if (m_metric_type == Cost)
        tickOptimiser(m_measured_cost);
    else if (m_metric_type == SpeedAndPushes)
        tickOptimiser(m_measured_speed);
    else
        tickOptimiser(m_measured_cost);
    
    respawn();
}

/*! @brief Perturbs the robot while it is walking
 
    We are going to be 'perturbing' the robot by applying an additional torque to the ankles and/or hips.
 
    I need to decide how I am going to fit the results in with the optimisation.
 
    Option 1. Measure cost over 20s. Then quickly perturbed the robot in each direction and see if it falls.
              This will short of result in a pass/fail.
 
    Option 2. Measure cost over 20s. Perturb the robot more and more until it falls over. This will take a long time,
              and at least in the simulator can only last for 20s or so before it runs out of field.
 
 
 */
void WalkOptimiserBehaviour::perturbRobot()
{
    static int stepcount = 0;
    
    // increment the step count every time there is a foot impact
    if (m_data->footImpact(NUSensorsData::LeftFoot, m_left_impact_time) || m_data->footImpact(NUSensorsData::RightFoot, m_right_impact_time))
    {
        stepcount++;
        if (stepcount%3 == 0)
        {
            m_trial_perturbation_mag += 0.2;
            m_perturbation_direction = (m_perturbation_direction + 1) % 4;
            cout << "Robot will be perturbed on this step by " << m_trial_perturbation_mag << endl;
        }
    }
    
    // perturb the robot on every third step
    if (stepcount%3 == 0)
    {   
        // estimate the time to perturb the robot (assume midstep is half of the previous step period)
        float steptime = fabs(m_left_impact_time - m_right_impact_time);
        float perturbationtime = 0;
        if (m_left_impact_time > m_right_impact_time)       // if the we are on the left foot
            perturbationtime = m_left_impact_time + 0.10*steptime;
        else
            perturbationtime = m_right_impact_time + 0.10*steptime;
        
        if (m_data->CurrentTime - perturbationtime > 0)
        {
            if (m_perturbation_direction == 0)
                pushInward();
            else if (m_perturbation_direction == 1)
                pushOutward();
            else if (m_perturbation_direction == 2)
                pushForward();
            else
                pushBackward();
        }
    }
}

void WalkOptimiserBehaviour::pushInward()
{
    // negative movements on hips are left
    // positive movements on ankle are left
    static float target = 0;
    static float stiffness = 0;
    cout << "pushInward()" << endl;
    if (m_left_impact_time > m_right_impact_time) 
    {
        m_data->getJointTarget(NUSensorsData::LAnkleRoll, target);
        m_data->getJointStiffness(NUSensorsData::LAnkleRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::LAnkleRoll, 0, target - (100*m_trial_perturbation_mag/stiffness), 0, stiffness);
        /*m_data->getJointTarget(NUSensorsData::LHipRoll, target);
        m_data->getJointStiffness(NUSensorsData::LHipRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::LHipRoll, 0, target + (100*m_trial_perturbation_mag/stiffness), 0, stiffness);
        m_data->getJointTarget(NUSensorsData::RAnkleRoll, target);
        m_data->getJointStiffness(NUSensorsData::RAnkleRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::RAnkleRoll, 0, target - (100*m_trial_perturbation_mag/stiffness), 0, stiffness);
        m_data->getJointTarget(NUSensorsData::RHipRoll, target);
        m_data->getJointStiffness(NUSensorsData::RHipRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::RHipRoll, 0, target + (100*m_trial_perturbation_mag/stiffness), 0, stiffness);*/
    }
    else
    {
        m_data->getJointTarget(NUSensorsData::RAnkleRoll, target);
        m_data->getJointStiffness(NUSensorsData::RAnkleRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::RAnkleRoll, 0, target + (100*m_trial_perturbation_mag/stiffness), 0, stiffness);
        /*m_data->getJointTarget(NUSensorsData::LHipRoll, target);
        m_data->getJointStiffness(NUSensorsData::LHipRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::LHipRoll, 0, target - (100*m_trial_perturbation_mag/stiffness), 0, stiffness);
        m_data->getJointTarget(NUSensorsData::RAnkleRoll, target);
        m_data->getJointStiffness(NUSensorsData::RAnkleRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::RAnkleRoll, 0, target + (100*m_trial_perturbation_mag/stiffness), 0, stiffness);
        m_data->getJointTarget(NUSensorsData::RHipRoll, target);
        m_data->getJointStiffness(NUSensorsData::RHipRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::RHipRoll, 0, target - (100*m_trial_perturbation_mag/stiffness), 0, stiffness);*/
    }
}

void WalkOptimiserBehaviour::pushOutward()
{
    static float target = 0;
    static float stiffness = 0;
    if (m_left_impact_time > m_right_impact_time) 
    {   // suspect
        // both +ve results in a nice push inward!
        cout << "pushOutward() on left foot" << endl;
        /*m_data->getJointTarget(NUSensorsData::LAnkleRoll, target);
        m_data->getJointStiffness(NUSensorsData::LAnkleRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::LAnkleRoll, 0, target - (100*m_trial_perturbation_mag/stiffness), 0, stiffness);*/
        m_data->getJointTarget(NUSensorsData::RHipRoll, target);
        m_data->getJointStiffness(NUSensorsData::RHipRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::RHipRoll, 0, target - (100*m_trial_perturbation_mag/stiffness), 0, stiffness);
        /*m_data->getJointTarget(NUSensorsData::RAnkleRoll, target);
        m_data->getJointStiffness(NUSensorsData::RAnkleRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::RAnkleRoll, 0, target + (100*m_trial_perturbation_mag/stiffness), 0, stiffness);
        m_data->getJointTarget(NUSensorsData::RHipRoll, target);
        m_data->getJointStiffness(NUSensorsData::RHipRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::RHipRoll, 0, target - (100*m_trial_perturbation_mag/stiffness), 0, stiffness);*/
    }
    else
    {
        cout << "pushOutward() on right foot" << endl;
        /*m_data->getJointTarget(NUSensorsData::RAnkleRoll, target);
        m_data->getJointStiffness(NUSensorsData::RAnkleRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::RAnkleRoll, 0, target + (100*m_trial_perturbation_mag/stiffness), 0, stiffness);*/
        m_data->getJointTarget(NUSensorsData::LHipRoll, target);
        m_data->getJointStiffness(NUSensorsData::LHipRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::LHipRoll, 0, target + (100*m_trial_perturbation_mag/stiffness), 0, stiffness);
        /*m_data->getJointTarget(NUSensorsData::RAnkleRoll, target);
        m_data->getJointStiffness(NUSensorsData::RAnkleRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::RAnkleRoll, 0, target - (100*m_trial_perturbation_mag/stiffness), 0, stiffness);
        m_data->getJointTarget(NUSensorsData::RHipRoll, target);
        m_data->getJointStiffness(NUSensorsData::RHipRoll, stiffness);
        m_actions->addJointPosition(NUSensorsData::RHipRoll, 0, target + (100*m_trial_perturbation_mag/stiffness), 0, stiffness);*/
    }
}

void WalkOptimiserBehaviour::pushForward()
{
    cout << "pushForward()" << endl;
    static float target = 0;
    static float stiffness = 0;
    float magnitude = 0.5*m_trial_perturbation_mag;
    if (m_left_impact_time > m_right_impact_time) 
    {
        m_data->getJointTarget(NUSensorsData::LAnklePitch, target);
        m_data->getJointStiffness(NUSensorsData::LAnklePitch, stiffness);
        m_actions->addJointPosition(NUSensorsData::LAnklePitch, 0, target - (100*magnitude/stiffness), 0, stiffness);
        /*m_data->getJointTarget(NUSensorsData::LHipPitch, target);
        m_data->getJointStiffness(NUSensorsData::LHipPitch, stiffness);
        m_actions->addJointPosition(NUSensorsData::LHipPitch, 0, target - (100*magnitude/stiffness), 0, stiffness);*/
    }
    else
    {
        m_data->getJointTarget(NUSensorsData::RAnklePitch, target);
        m_data->getJointStiffness(NUSensorsData::RAnklePitch, stiffness);
        m_actions->addJointPosition(NUSensorsData::RAnklePitch, 0, target - (100*magnitude/stiffness), 0, stiffness);
        /*m_data->getJointTarget(NUSensorsData::RHipPitch, target);
        m_data->getJointStiffness(NUSensorsData::RHipPitch, stiffness);
        m_actions->addJointPosition(NUSensorsData::RHipPitch, 0, target - (100*magnitude/stiffness), 0, stiffness);*/
    }
}

void WalkOptimiserBehaviour::pushBackward()
{
    cout << "pushBackward()" << endl;
    static float target = 0;
    static float stiffness = 0;
    float magnitude = 0.5*m_trial_perturbation_mag;
    if (m_left_impact_time > m_right_impact_time) 
    {
        m_data->getJointTarget(NUSensorsData::LAnklePitch, target);
        m_data->getJointStiffness(NUSensorsData::LAnklePitch, stiffness);
        m_actions->addJointPosition(NUSensorsData::LAnklePitch, 0, target + (100*magnitude/stiffness), 0, stiffness);
        m_data->getJointTarget(NUSensorsData::LHipPitch, target);
        m_data->getJointStiffness(NUSensorsData::LHipPitch, stiffness);
        m_actions->addJointPosition(NUSensorsData::LHipPitch, 0, target + (100*magnitude/stiffness), 0, stiffness);
    }
    else
    {
        m_data->getJointTarget(NUSensorsData::RAnklePitch, target);
        m_data->getJointStiffness(NUSensorsData::RAnklePitch, stiffness);
        m_actions->addJointPosition(NUSensorsData::RAnklePitch, 0, target + (100*magnitude/stiffness), 0, stiffness);
        m_data->getJointTarget(NUSensorsData::RHipPitch, target);
        m_data->getJointStiffness(NUSensorsData::RHipPitch, stiffness);
        m_actions->addJointPosition(NUSensorsData::RHipPitch, 0, target + (100*magnitude/stiffness), 0, stiffness);
    }
}

/*! @brief Ticks the optimiser
 */
void WalkOptimiserBehaviour::tickOptimiser(float metric)
{
    cout << "Ticking Optimiser" << endl;
    m_optimiser->tick(metric, m_walk_parameters);
    saveOptimiser();
}

/*! @brief Loads a saved WalkOptimiser from a file. If there is no file then we start from scratch
 */
void WalkOptimiserBehaviour::loadOptimiser()
{
    ifstream savedoptimiser(m_saved_optimiser_filename.c_str(), ios_base::in);
    if (savedoptimiser.is_open())
    {   // if it exists start from where we left off
        cout << "WalkOptimiserBehaviour::WalkOptimiserBehaviour. Loading previous WalkOptimiser from file." << endl;
        savedoptimiser >> (*m_optimiser);
        m_optimiser->getNewParameters(m_walk_parameters);
        m_walk->setWalkParameters(m_walk_parameters);
        m_optimiser->summaryTo(cout);
    }
}

/*! @brief Saves the WalkOptimiser to a file so that we can continue later
 */
void WalkOptimiserBehaviour::saveOptimiser()
{
    ofstream savedoptimiser(m_saved_optimiser_filename.c_str(), ios_base::trunc);
    if (savedoptimiser.is_open())
        savedoptimiser << (*m_optimiser);
    m_optimiser->summaryTo(cout);
}