#ifndef UKF_H
#define UKF_H

#include "Matrix.h"

class UKF
{
public:
    UKF();
    UKF(unsigned int numStates);
    UKF(const UKF& source);
    ~UKF();
    Matrix GenerateSigmaWeights(float kappa = 1.0f) const;
    Matrix GenerateSigmaPoints() const;
    Matrix CalculateSigmaPointsMean(const Matrix& sigmaPoints) const;
    Matrix CalculateSigmaPointsCovariance(const Matrix& sigmaPoints, const Matrix& mean) const;
    void setMean(const Matrix& newMean) {m_mean = newMean;};
    void setCovariance(const Matrix& newCovariance) {m_covariance = newCovariance;};
    double getMean(int stateId) const;
    bool setState(Matrix mean, Matrix covariance);

protected:
   unsigned int m_numStates;
   Matrix m_mean;
   Matrix m_covariance;
   float m_kappa;
};

#endif // UKF_H
