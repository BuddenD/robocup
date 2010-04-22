/*! @file MotionCurves.cpp
    @brief Implementation of motion curve calculation class

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

#include "MotionCurves.h"
#include "debug.h"
#include "debugverbositynumotion.h"

using namespace std;

/*! @brief Calculates a smooth motion curve for a single position
    @param starttime the time in ms to start moving to the given position
    @param stoptime the time in ms to reach the given position
    @param startposition the start postion for the curve
    @param stopposition the stop position for the curve
    @param smoothness a fraction indicating the smoothness of the motion: 0 means linear motion curve, 1 minimises the acceleration and jerk
    @param cycletime the motion cycle time in ms. This is used to decide how many points to generate
    @param calculatedtimes the calculated times for the curve. Do not assume the times will be evenly spaced!
    @param calculatedpositions the calculated positions for the curve.
 */
void MotionCurves::calculate(double starttime, double stoptime, float startposition, float stopposition, float smoothness, int cycletime, vector<double>& calculatedtimes, vector<float>& calculatedpositions)
{
    calculateTrapezoidalCurve(starttime, stoptime, startposition, stopposition, 0, 0, smoothness, cycletime, calculatedtimes, calculatedpositions);
}

/*! @brief Calculates a smooth trapezoidal curve for a single position
    @param starttime the time in ms to start moving to the given position
    @param stoptime the time in ms to reach the given position
    @param startposition the start postion for the curve
    @param stopposition the stop position for the curve
    @param startvelocity the initial velocity
    @param stopvelocity the final velocity 
    @param smoothness a fraction indicating the smoothness of the motion: 0 means linear motion curve, 1 minimises the acceleration and jerk
    @param cycletime the motion cycle time in ms. This is used to decide how many points to generate
    @param calculatedtimes the calculated times for the curve. Do not assume the times will be evenly spaced!
    @param calculatedpositions the calculated positions for the curve.
 
 Acceleration Profile:
 As--  ---
      |   |    
      |   |    
      |   |     
  ----|   |------|    |----
                 |    |
                 |    |
                  ----  --Af
      |   |      |    |
      t0  t1     t2   tf
 where t1 and t2 move closer to 0.5*tf as the smoothness is increased to 1.
 */
void MotionCurves::calculateTrapezoidalCurve(double starttime, double stoptime, float startposition, float stopposition, float startvelocity, float stopvelocity, float smoothness, int cycletime, vector<double>& calculatedtimes, vector<float>& calculatedpositions)
{
    if (smoothness < 0)
        smoothness = - smoothness;
    if (smoothness > 1)
        smoothness = 1;
    
    double t0 = starttime;
    double t1 = starttime + 0.5*smoothness*(stoptime - starttime);
    double t2 = starttime + (stoptime - starttime)*(1 - 0.5*smoothness);
    double tf = stoptime;
    
    if (t0 > tf)
        return;
    
    float g0 = startposition;
    float gf = stopposition;
    float v0 = startvelocity;
    float vf = startvelocity;
    
    cout << "MotionCurves::calculateTrapezoidalCurve(" << t0 << ", " << tf << ", " << g0 << ", " << gf << ", " << v0 << ", " << vf << ", " << smoothness << ", " << cycletime << ")" << endl;
    
    // Calculate the required acceleration magnitudes
    float Af = 2*(gf - g0 - vf*tf - v0*t0 + 0.5*(t1 + t0)*(vf - v0))/(t2*t2 - tf*tf - (t1+t0)*(t2 - tf));
    float As = (vf - v0 - Af*tf + Af*t2)/(t1-t0);
    
    // Calculate the times to calculate the curve points at
    vector<double> times;
    for (float t = t0; t <= t1; t += cycletime)
        times.push_back(t);
    for (float t = t2; t < tf; t += cycletime)
        times.push_back(t);
    times.push_back(tf);
    
    // Now calculate the curve itself
    vector<float> positions;
    for (unsigned int i=0; i<times.size(); i++)
    {
        float t = times[i];
        if (t <= t1)
            positions.push_back(0.5*As*t*t - As*t0*t + v0*t + 0.5*As*t0*t0 + g0 + v0*t0);
        else if (t <= t2)
            positions.push_back(As*(t1 - t0)*t + v0*t - 0.5*As*t1*t1 + 0.5*As*t0*t0 + g0 + v0*t0);
        else
            positions.push_back(0.5*Af*t*t + As*(t1 - t0)*t - Af*t2*t + v0*t + 0.5*Af*t2*t2 - 0.5*As*t1*t1 + 0.5*As*t0*t0 + g0 + v0*t0);
    }

    // Finally copy the result to the output vectors
    calculatedtimes = times;
    calculatedpositions = positions;
}




