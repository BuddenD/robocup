/**
 * @file Math/General.h
 *
 */


#ifndef MATH_GENERAL_H
#define MATH_GENERAL_H

#include <cmath>
#include <vector>
#include <iostream>
using namespace std;

namespace mathGeneral
{

const double PI = 4.0*atan(1.0);

template <class T>
inline T sqr(T x){
  return x*x;
}

template <class T>
inline T abs(T x){
  if(x > 0) return x;
  else return -x;
}

template <class T>
inline T max(T x, T y){
  if(x > y) return x;
  else return y;
}

template <class T>
inline T min(T x, T y){
  if(x < y) return x;
  else return y;
}

template <class T>
inline T crop(T num, T low, T high){
  if (num < low) num = low;
  else if (num > high) num = high;
  return num;
}

template <class T>
inline int sign(T x){
  if(x < 0.0) return -1;
  else if(x > 0.0) return 1;
  else return 0;
}

inline double rad2deg(double x){
  return ((x)*180.0)/PI;
}

inline double deg2rad(double x){
  return ((x)*PI)/180.0;
}

inline double normaliseAngle(double theta){
    return atan2(sin(theta), cos(theta));
}

/**
* Round to the next integer
* @param d A number
* @return The number as integer
*/
inline int roundNumberToInt(double d)
{
  return static_cast<int>(floor(d+0.5));
}

inline std::vector<float> Spherical2Cartesian(const std::vector<float>& sphericalCoordinates)
{
    const float distance = sphericalCoordinates[0];
    const float bearingcos = cos(sphericalCoordinates[1]);
    const float bearingsin = sin(sphericalCoordinates[1]);
    const float elevationcos = cos(sphericalCoordinates[2]);
    const float elevationsin = sin(sphericalCoordinates[2]);

    std::vector<float> result(3,0.0f);
    result[0] = distance * bearingcos * elevationcos;
    result[1] = distance * bearingsin * elevationcos;
    result[2] = distance * elevationsin;
    return result;
}

inline std::vector<float> Cartesian2Spherical(const std::vector<float>& cartesianCoordinates)
{
    const float x = cartesianCoordinates[0];
    const float y = cartesianCoordinates[1];
    const float z = cartesianCoordinates[2];
    std::vector<float> result(3,0.0f);

    result[0] = sqrt(x*x + y*y + z*z);
    result[1] = atan2(y,x);
    result[2] = asin(z/(result[0]));
    return result;
}

/*! @brief Determines whether the point is inside the convex hull specifed by vertices.
    @param x the x value of the point to test
    @param y the y value of the point to test
    @param vertices the [[x0,y0], [x1,y1], ... , [xn,yn]] specifying the convex hull. 
           If x is up, y is left, then the vertices need to be in clockwise order.
           If x is up, y is right, then the vertices need to be in anti-clockwise order
    @param margin the amount to 'shrink' the convex hull and then test if the point is inside it
 */
inline bool PointInsideConvexHull(float x, float y, const std::vector<std::vector<float> >& vertices, float margin = 0)
{
    if (x > 0)
        x += margin;
    else
        x -= margin;
    if (y > 0)
        y += margin;
    else
        y -= margin;
    
    size_t n = vertices.size();
    for (size_t i=0; i<n; i++)
    {
        if ((vertices[(i+1)%n][0] - x)*(vertices[i][1] - y) - (vertices[i][0] - x)*(vertices[(i+1)%n][1] - y) < 0)
            return false;
    }
    return true;
}


} // End namespace

#endif //MATH_GENERAL_H
