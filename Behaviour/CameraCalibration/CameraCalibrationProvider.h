/*! @file CameraCalibrationProvider.h
    @brief Declaration of a behaviour provider for pose purposes
 
    @class CameraCalibrationProvider
    @brief A special behaviour for calibrating images.
 

    @author Aaron Wong
 
  Copyright (c) 2010 Aaron Wong
 
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

#ifndef CAMERACALBRATIONPROVIDER_H
#define CAMERACALBRATIONPROVIDER_H

#include "../BehaviourProvider.h"
#include "Infrastructure/NUImage/NUImage.h"

class JobList;
class NUSensorsData;
class NUActionatorsData;
class FieldObjects;
class GameInformation;
class TeamInformation;

#include <vector>
#include <string>

class CameraCalibrationProvider : public BehaviourProvider
{
public:
    CameraCalibrationProvider(Behaviour* manager);
    ~CameraCalibrationProvider();
protected:
    void doBehaviour();
    void doSelectedMotion();
    //float calculatePitchPosition();
    //float calculateYawPosition();
    //void sayPosition(float degree);

private:
    float m_pitch_index;
    float m_yaw_index;
    int isStart;
    bool m_saving_images;
    NUImage refImage;
    bool topcam;
    float angleOffset;

};


#endif

