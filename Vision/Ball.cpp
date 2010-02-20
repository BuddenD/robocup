#include "Ball.h"
#include "ClassificationColours.h"
#include "TransitionSegment.h"
#include "ScanLine.h"
#include "ClassifiedSection.h"
//#include <QDebug>
#include "debug.h"

Ball::Ball()
{
    debug<< "Vision::DetectBall : Ball Class created" << endl;
}
Ball::~Ball()
{
}

//! Finds the ball segments and groups updates the ball in fieldObjects (Vision is used to further classify the object)
Circle Ball::FindBall(std::vector <ObjectCandidate> FO_Candidates, FieldObjects* AllObjects, Vision* vision,int height,int width)
{
    Circle result;
    result.centreX = 0;
    result.centreY = 0;
    result.isDefined = false;
    result.radius = 0;
    result.sd = 0;

    //! Go through all the candidates: to find a possible ball
    //qDebug() <<"FO_Candidates.size():"<< FO_Candidates.size();
    for(unsigned int i = 0; i  < FO_Candidates.size(); i++)
    {

        ObjectCandidate PossibleBall = FO_Candidates[i];

        if(!isObjectAPossibleBall(PossibleBall)) continue;

        //! Check if the ratio is correct: Height and Width ratio should be 1 as it is a circle,
        //! through can be skewed (camera moving), so we better put some threshold on it.
        if(!isCorrectCheckRatio(PossibleBall, height, width)) continue;
        //qDebug() << "Possible Ball Found ";

        //! Closely Classify the candidate: to obtain more information about the object (using closely classify function in vision)
        std::vector < Vector2<int> > ballPoints = classifyBallClosely(PossibleBall, vision,height, width);

        //! Perform Circle Fit: Must pass a threshold on fit to become a circle!
        result = isCorrectFit(ballPoints,PossibleBall);
        //! Use Circle Fit information to update the FieldObjects


        //! check if current object is larger then object before.
    }

    return result;
}

bool Ball::isObjectAPossibleBall(ObjectCandidate PossibleBall)
{
    if(PossibleBall.getColour()== ClassIndex::orange ||
       PossibleBall.getColour()== ClassIndex::red_orange ||
       PossibleBall.getColour() == ClassIndex::yellow_orange)
    {
        return true;
    }
    else{
        return false;
    }
}
std::vector < Vector2<int> > Ball::classifyBallClosely(ObjectCandidate PossibleBall,Vision* vision,int heigth, int width)
{
    Vector2<int> TopLeft = PossibleBall.getTopLeft();
    Vector2<int> BottomRight = PossibleBall.getBottomRight();
    int midX =  (int)((BottomRight.x-TopLeft.x)/2)+TopLeft.x;
    Vector2<int> SegStart;
    SegStart.x = midX;
    SegStart.y = TopLeft.y-5;
    Vector2<int> SegEnd;
    SegEnd.x = midX;
    SegEnd.y = BottomRight.y-5;
    TransitionSegment* tempSeg = new TransitionSegment(SegStart,SegEnd,ClassIndex::unclassified,PossibleBall.getColour(),ClassIndex::unclassified);
    ScanLine* tempLine = new ScanLine();

    int spacings = 1;
    int direction = ClassifiedSection::DOWN;
    vision->CloselyClassifyScanline(tempLine,tempSeg,spacings, direction);
    //qDebug() << TopLeft.y <<","<< BottomRight.y;
    //qDebug() << SegStart.y <<","<< SegEnd.y;

    //qDebug() << "Ball CLOSELY CLASSIFIED " <<tempLine->getNumberOfSegments()<< "segments";
    std::vector< Vector2<int> > BallPoints;


    //! Debug Output for small scans:
    for(int i = 0; i < tempLine->getNumberOfSegments(); i++)
    {
        tempSeg = tempLine->getSegment(i);
        //! Check if the segments are at the edge of screen
        if(!(tempSeg->getStartPoint().x < 0 || tempSeg->getStartPoint().y < 0))
        {
            BallPoints.push_back(tempSeg->getStartPoint());
        }
        if(!(tempSeg->getEndPoint().x >= heigth-1 || tempSeg->getEndPoint().x >= width-1))
        {
            BallPoints.push_back(tempSeg->getEndPoint());
        }

        /*qDebug()<< "At " <<i<<"\t Size: "<< tempSeg->getSize()<< "\t Start(x,y),End(x,y):("<< tempSeg->getStartPoint().x
                <<","<< tempSeg->getStartPoint().y << ")("<< tempSeg->getEndPoint().x
                <<","<< tempSeg->getEndPoint().y << ")";*/

    }

    return BallPoints;

}
bool Ball::isCorrectCheckRatio(ObjectCandidate PossibleBall,int height, int width)
{
    //qDebug() << "Checking Ratio: " << PossibleBall.aspect();

    //! Check if at Edge of Screen, if so continue with other checks, otherwise, look at ratio and check if in thresshold
    if (PossibleBall.getBottomRight().x <= width-5 &&
        PossibleBall.getBottomRight().y <= height-5 &&
        PossibleBall.getTopLeft().x >=0+5  &&
        PossibleBall.getTopLeft().y >=0+5  )
    {
        //POSSIBLE BALLS ARE:
        //      Objects which have grouped segments,
        //      or objects with one segment, but very small (still like to consider).
        if((PossibleBall.aspect() > 0.5 && PossibleBall.aspect() < 1.5 )|| PossibleBall.aspect()==0)
        {
            return true;
        }
        else
        {
            //qDebug() << "Thrown out due to incorrect ratio";
            return false;
        }
    }
    else
    {
        //qDebug() << "Returned True at edge of screen";
        return true;
    }
}
Circle Ball::isCorrectFit(std::vector < Vector2<int> > ballPoints, ObjectCandidate PossibleBall)
{
    Circle circ;
    /*
    //qDebug() << "Points:";
    for(int i =0; i < ballPoints.size(); i++)
    {
        //qDebug() << "("<<ballPoints[i].x << ","<<ballPoints[i].y<< ")";
    }*/
    if(ballPoints.size() > 5)
    {
            CircleFitting* CircleFit = new CircleFitting();
            circ = CircleFit->FitCircleLMA(ballPoints);
            //qDebug() << "Circle found " << circ.isDefined<<": (" << circ.centreX << "," << circ.centreY << ") Radius: "<< circ.radius << " Fitting: " << circ.sd;
    }
    else
    {
        Vector2<int> bottomRight = PossibleBall.getBottomRight();
        Vector2<int> topLeft = PossibleBall.getTopLeft();
        //! find midPoints of the Candidate:
        circ.centreX = (bottomRight.x + topLeft.x)/2;
        circ.centreY = (bottomRight.y + topLeft.y)/2;

        //! Select the Largest side as radius:
        if(fabs(bottomRight.x - topLeft.x) > fabs(bottomRight.y - topLeft.y))
        {
             circ.radius= fabs(bottomRight.x - topLeft.x)/2;
        }
        else
        {
            circ.radius = fabs(bottomRight.y - topLeft.y)/2;
        }
        //qDebug() << "Circle cannot be fitted: Used Candidate information";
    }


    return circ;
}
