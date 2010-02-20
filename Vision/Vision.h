/*!
  @file Vision.h
  @brief Declaration of NUbots Vision class.
*/

#ifndef VISION_H
#define VISION_H

#include <vector>
#include <boost/circular_buffer.hpp>
#include "Tools/Math/Vector2.h"
#include "Kinematics/Horizon.h"
#include "Tools/Image/ClassifiedImage.h"
#include "ClassifiedSection.h"
#include "ScanLine.h"
#include "TransitionSegment.h"
#include "RobotCandidate.h"
#include "LineDetection.h"
#include "FieldObjects/FieldObjects.h"
#include "ObjectCandidate.h"
#include "NUPlatform/NUSensors/NUSensorsData.h"
#include "debug.h"

#define ORANGE_BALL_DIAMETER 6.5 //IN CM for NEW BALL


class Circle;
class NUimage;

//! Contains vision processing tools and functions.
class Vision
{
    private:
    const NUimage* currentImage; //!< Storage of a pointer to the raw colour image.
    const unsigned char* currentLookupTable; //!< Storage of a pointer to the current colour lookup table.

    int findYFromX(std::vector<Vector2<int> >&points, int x);
    bool checkIfBufferSame(boost::circular_buffer<unsigned char> cb);
    
    double CalculateBearing(double cx);
    double CalculateElevation(double cy);
    double EFFECTIVE_CAMERA_DISTANCE_IN_PIXELS();

    public:
    //! FieldObjects Container
    FieldObjects* AllFieldObjects;
    int classifiedCounter;
    //! Default constructor.
    Vision();
    //! Destructor.
    ~Vision();

    //void ProcessFrame(NUimage& image, Horizon horizonLine);
    void ProcessFrame(NUimage& image, NUSensorsData* data);

    void setLUT(unsigned char* newLUT);


    void setImage(const NUimage* sourceImage);


    void classifyPreviewImage(ClassifiedImage &target,unsigned char* tempLut);
    /*!
      @brief Produce a classified.

      Primarily used for display when debugging and constructing a lookup table.
      @param targetImage The target classification image that will be written to.
      @param sourceImage The raw image to be classified.
      @param lookUpTable The colour classification lookup table. This table maps colours
      from the raw source image into the classified colour space.
      */
    void classifyImage(ClassifiedImage &targetImage);
    /*!
      @brief Classifies an individual pixel.
      @param x The x coordinate of the pixel to be classified.
      @param y The y coordinate of the pixel to be classified.
      @return Returns the classfied colour index for the given pixel.
      */
    inline unsigned char classifyPixel(int x, int y);

    enum tCLASSIFY_METHOD
    {
        PRIMS,
        DBSCAN
    };

    /*!
      @brief Joins segments to create a joined segment clusters that represent candidate robots
      @param segList The segList is a vector of TransitionSegments after field lines have been rejected
      @returns A list of ObjectCanidates
    */

    std::vector<ObjectCandidate> classifyCandidates(std::vector< TransitionSegment > segments,
                                                    std::vector<Vector2<int> >&fieldBorders,
                                                    std::vector<unsigned char> validColours,
                                                    int spacing,
                                                    float min_aspect, float max_aspect, int min_segments,
                                                    tCLASSIFY_METHOD method);

    std::vector<ObjectCandidate> classifyCandidatesPrims(std::vector< TransitionSegment > segments,
                                                         std::vector<Vector2<int> >&fieldBorders,
                                                         std::vector<unsigned char> validColours,
                                                         int spacing,
                                                         float min_aspect, float max_aspect, int min_segments);

    std::vector<ObjectCandidate> classifyCandidatesDBSCAN(std::vector< TransitionSegment > segments,
                                                          std::vector<Vector2<int> >&fieldBorders,
                                                          std::vector<unsigned char> validColours,
                                                          int spacing,
                                                          float min_aspect, float max_aspect, int min_segments);

    /*!
      @brief Defines a finer scan area of where a candidate robot exists
      @param robotCandidates The vector of RobotCandidates that contain the location of where the robot feet should be
      @param horizonLine The HorizonLine is where the robotScanArea will scan upto and just a little bit beyond since this is theoretically the maximum height of a robot.
      @param scanSpacing This value defines how fine the scan resolution should be when creating scan lines.
      @returns
    */
    //std::vector< ClassifiedSection > robotScanAreas(std::vector<RobotCandidate> robotCandidates, std::vector<Vector2<int> >&fieldBorders, Horizon horizonLine);




    /*!
      @brief Returns true when the colour passed in is a valid colour from the list passed in
      @param colour The colour value that needs to be checked if it is a robot colour
      @param colourList The vector of valid colours to match against
      @return bool True when the colour passed in is an assigned robot colour
    */
    bool isValidColour(unsigned char colour, std::vector<unsigned char> colourList);

    int findInterceptFromPerspectiveFrustum(std::vector<Vector2<int> >&points, int current_x, int target_x, int spacing);
    static bool sortTransitionSegments(TransitionSegment a, TransitionSegment b);

    std::vector<Vector2<int> > findGreenBorderPoints(int scanSpacing, Horizon* horizonLine);
    std::vector<Vector2<int> > getConvexFieldBorders(std::vector<Vector2<int> >& fieldBorders);
    std::vector<Vector2<int> > interpolateBorders(std::vector<Vector2<int> >& fieldBorders, int scanSpacing);


    ClassifiedSection* horizontalScan(std::vector<Vector2<int> >&fieldBoarders, int scanSpacing);
    ClassifiedSection* verticalScan(std::vector<Vector2<int> >&fieldBoarders, int scanSpacing);
    void ClassifyScanArea(ClassifiedSection* scanArea);
    void CloselyClassifyScanline(ScanLine* tempLine, TransitionSegment* tempSeg, int spacing, int direction);
    std::vector<LSFittedLine> DetectLines(ClassifiedSection* scanArea, int spacing);



    Circle DetectBall(std::vector<ObjectCandidate> FO_Candidates);






};
#endif // VISION_H
