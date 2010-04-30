#include "Kinematics.h"
#include <cmath>
#include "Tools/Math/TransformMatrices.h"
#include "Tools/Math/General.h"
#include "debug.h"

using namespace TransformMatrices;

//! TODO: Make this function load from a file.
bool Kinematics::LoadModel(const std::string& fileName)
{
    // Constant distances (in cm) From documentation

    // Top camera
    const float cameraTopOffsetZ = 6.79;
    const float cameraTopOffsetX = 5.39;
    const float cameraTopOffsetAngle = mathGeneral::deg2rad(0.0);

    // Bottom Camera
    const float cameraBottomOffsetZ = 2.381;
    const float cameraBottomOffsetX = 4.88;
    const float cameraBottomOffsetAngle = mathGeneral::deg2rad(40.0);

    // Neck
    const float neckOffsetZ = 12.65;

    // Hips
    const float hipOffsetY = 5.0;
    const float hipOffsetZ = 8.5;

    // Legs
    const float thighLength = 10.0;
    const float tibiaLength = 10.0;
    const float footHeight = 4.6;

    DHParameters tempParam;
    std::vector<Link> links;
    Matrix startTrans;
    Matrix endTrans;

    // ---------------
    // BOTTOM CAMERA
    // ---------------
    startTrans = Translation( 0, 0, neckOffsetZ );
    endTrans = RotX( mathGeneral::PI/2.0 )*RotY( mathGeneral::PI/2.0 );
    endTrans = endTrans * Translation(cameraBottomOffsetX,0,cameraBottomOffsetZ) * RotY(cameraBottomOffsetAngle);

    tempParam.alpha = 0.0;
    tempParam.a = 0;
    tempParam.thetaOffset = 0.0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"Head Yaw"));

    tempParam.alpha = -mathGeneral::PI/2.0;
    tempParam.a = 0;
    tempParam.thetaOffset = -mathGeneral::PI/2.0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"Head Pitch"));

    m_endEffectors.push_back(EndEffector(startTrans, links, endTrans, "Bottom Camera"));

    // ---------------
    // TOP CAMERA
    // ---------------
    startTrans = Translation( 0, 0, neckOffsetZ );
    endTrans = RotX( mathGeneral::PI/2.0 )*RotY( mathGeneral::PI/2.0 );
    endTrans = endTrans * Translation(cameraTopOffsetX,0,cameraTopOffsetZ);

    links.clear();

    tempParam.alpha = 0.0;
    tempParam.a = 0;
    tempParam.thetaOffset = 0.0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"Head Yaw"));

    tempParam.alpha = -mathGeneral::PI/2.0;
    tempParam.a = 0;
    tempParam.thetaOffset = -mathGeneral::PI/2.0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"Head Pitch"));

    m_endEffectors.push_back(EndEffector(startTrans, links, endTrans, "Top Camera"));

    // ---------------
    // LEFT FOOT
    // ---------------
    startTrans = Translation(0.0f, hipOffsetY, -hipOffsetZ);
    endTrans = RotZ(mathGeneral::PI)*RotY(-mathGeneral::PI/2.0)*Translation(0,0,-footHeight);    // To foot
    links.clear();

    tempParam.alpha = -3.0*mathGeneral::PI/4.0;
    tempParam.a = 0;
    tempParam.thetaOffset = -mathGeneral::PI/2.0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"HipYawPitch"));

    tempParam.alpha = -mathGeneral::PI/2.0;
    tempParam.a = 0;
    tempParam.thetaOffset = mathGeneral::PI/4.0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"LeftHipRoll"));

    tempParam.alpha = mathGeneral::PI/2.0;
    tempParam.a = 0;
    tempParam.thetaOffset = 0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"LeftHipPitch"));

    tempParam.alpha = 0;
    tempParam.a = -thighLength;
    tempParam.thetaOffset = 0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"LeftKneePitch"));

    tempParam.alpha = 0;
    tempParam.a = -tibiaLength;
    tempParam.thetaOffset = 0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"LeftAnklePitch"));

    tempParam.alpha = -mathGeneral::PI/2.0;
    tempParam.a = 0;
    tempParam.thetaOffset = 0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"LeftAnkleRoll"));

    m_endEffectors.push_back(EndEffector(startTrans, links, endTrans, "Left Foot"));

    // ---------------
    // RIGHT FOOT
    // ---------------
    startTrans = Translation(0.0f, -hipOffsetY, -hipOffsetZ);
    endTrans = RotZ(mathGeneral::PI)*RotY(-mathGeneral::PI/2.0)*Translation(0,0,-footHeight);    // To foot
    links.clear();

    tempParam.alpha = -mathGeneral::PI/4.0;
    tempParam.a = 0;
    tempParam.thetaOffset = -mathGeneral::PI/2.0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"HipYawPitch"));

    tempParam.alpha = -mathGeneral::PI/2.0;
    tempParam.a = 0;
    tempParam.thetaOffset = -mathGeneral::PI/4.0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"RightHipRoll"));

    tempParam.alpha = mathGeneral::PI/2.0;
    tempParam.a = 0;
    tempParam.thetaOffset = 0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"RightHipPitch"));

    tempParam.alpha = 0;
    tempParam.a = -thighLength;
    tempParam.thetaOffset = 0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"RightKneePitch"));

    tempParam.alpha = 0;
    tempParam.a = -tibiaLength;
    tempParam.thetaOffset = 0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"RightAnklePitch"));

    tempParam.alpha = -mathGeneral::PI/2.0;
    tempParam.a = 0;
    tempParam.thetaOffset = 0;
    tempParam.d = 0;
    links.push_back(Link(tempParam,"RightAnkleRoll"));

    m_endEffectors.push_back(EndEffector(startTrans, links, endTrans, "Left Foot"));
    return true;
}


Matrix Kinematics::CalculateTransform(Effector effectorId, const std::vector<float>& jointValues)
{
    std::vector<float> modifiedJointValues;

    switch(effectorId)
    {
        case bottomCamera:
        case topCamera:
            modifiedJointValues = ReOrderKneckJoints(jointValues);
            break;
        case leftFoot:
        case rightFoot:
            modifiedJointValues = ReOrderLegJoints(jointValues);
            break;
        default:
            modifiedJointValues = jointValues;
    }
    return m_endEffectors[effectorId].CalculateTransform(modifiedJointValues);
}

double Kinematics::DistanceToPoint(const Matrix& Camera2GroundTransform, double angleFromCameraCentreX, double angleFromCameraCentreY)
{
    const double nearDistance = 10;     // 10 cm
    const double farDistance = 3000;   // 3,000 cm (30 metres)

    std::vector<double> result(3);

    // pre-calculate required trig values.
    double xcos = cos(angleFromCameraCentreX);
    double xsin = sin(angleFromCameraCentreX);
    double ycos = cos(angleFromCameraCentreY);
    double ysin = sin(angleFromCameraCentreY);

    // Build the near measurement vector
    Matrix nearCartCol(4,1);
    nearCartCol[0][0] = nearDistance * xcos * ycos;
    nearCartCol[1][0] = nearDistance * xsin * ycos;
    nearCartCol[2][0] = nearDistance * ysin;
    nearCartCol[3][0] = 1.0;
    debug << "nearCartCol:" << std::endl << nearCartCol << std::endl;

    // Build the far measurement vector
    Matrix farCartCol(4,1);
    farCartCol[0][0] = farDistance * xcos * ycos;
    farCartCol[1][0] = farDistance * xsin * ycos;
    farCartCol[2][0] = farDistance * ysin;
    farCartCol[3][0] = 1.0;
    debug << "farCartCol:" << std::endl << farCartCol << std::endl;

    // Caluculate The Transformed positions of both the near and far values.
    Matrix nearResult = Camera2GroundTransform * nearCartCol;
    Matrix farResult = Camera2GroundTransform * farCartCol;
    debug << "close:" << std::endl << nearResult << std::endl;
    debug << "far:" << std::endl << farResult << std::endl;

    // Interpolate between near and far values to find the point at which z = 0 (the ground)
    double zScaleFactor = nearResult[2][0] / (nearResult[2][0] - farResult[2][0]);
    result[0] = nearResult[0][0] + (farResult[0][0] - nearResult[0][0]) * zScaleFactor;
    result[1] = nearResult[1][0] + (farResult[1][0] - nearResult[1][0]) * zScaleFactor;
    result[2] = 0.0;

    debug << "point:" << std::endl;
    for(int j=0; j<result.size(); j++)
    {
        debug<< "[ ";
        debug << std::setw(12) << std::setprecision(4) << result[j];
        debug << "]" << std::endl;
    }

    // Convert back to polar coodinates.
    float x = result[0];
    float y = result[1];
    float z = result[2];
    float distance = sqrt(x*x + y*y + z*z);
    float bearing = atan2(y,x);
    float elevation = asin(z/(distance));

    debug << "Distance: " << distance << std::endl;
    debug << "Bearing: " << bearing << std::endl;
    debug << "Elevation: " << elevation << std::endl;
    return result[2];
}

Matrix Kinematics::CalculateCamera2GroundTransform(const Matrix& origin2SupportLegTransform, const Matrix& origin2CameraTransform)
{
    double legOffsetX = origin2SupportLegTransform[0][3];
    double legOffsetY = origin2SupportLegTransform[1][3];
    return Translation(legOffsetX,legOffsetY,0)* InverseMatrix(origin2SupportLegTransform) * origin2CameraTransform;
}

std::vector<float> Kinematics::TransformPosition(const std::vector<float>& cameraBasedPosition)
{
    std::vector<float> result;
    return result;
}

std::vector<float> Kinematics::LookToPoint(const std::vector<float>& pointFieldCoordinates)
{
    std::vector<float> result;
    return result;
}
