#ifndef ENDEFFECTOR_H
#define ENDEFFECTOR_H
#include <vector>
#include "Tools/Math/Matrix.h"
#include "Joint.h"

class EndEffector
{
    Matrix startTransform;
    std::vector<Joint> joints;
    Matrix endTransform;

public:
    EndEffector();
    Matrix CalculateTransform(std::vector<float> jointValues);
};

#endif // ENDEFFECTOR_H
