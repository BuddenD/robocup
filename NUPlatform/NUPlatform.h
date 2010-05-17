/*! @file NUPlatform.h
    @brief Declaration of a base nuplatform class.

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

#ifndef NUPLATFORM_H
#define NUPLATFORM_H

#include <string>
using namespace std;

#include "NUCamera.h"
#include "NUSensors.h"
#include "NUActionators.h"
#include "NUSystem.h"

class NUPlatform
{
// Functions:
public:
    virtual ~NUPlatform();
    
    string getName();
    int getRobotNumber();
    int getPlayerNumber();
    string getTeamColour();
    int getTeamNumber();
    
protected:
private:
    
// Members:
public:
    NUSystem* system;
    NUCamera* camera;
    NUSensors* sensors;
    NUActionators* actionators;
protected:
    string m_name;
    int m_robot_number;
    int m_player_number;
    string m_team_colour;
    int m_team_number;
private:
    
};

#endif

