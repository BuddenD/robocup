#include "Vision.h"
#include "ObjectCandidate.h"
#include "CircleFitting.h"

class Ball
{
  public:
	Ball();
        ~Ball();

        Circle FindBall(const std::vector<ObjectCandidate> FO_Candidates,
			FieldObjects* AllObjects,
                        Vision* vision,
                        int height,
                        int width);
  private:
        bool isObjectAPossibleBall(const ObjectCandidate &PossibleBall);
        std::vector < Vector2<int> > classifyBallClosely(const ObjectCandidate &PossibleBall,Vision* vision,int height,int width);
        bool isCorrectCheckRatio(const ObjectCandidate &PossibleBall,int height,int width);
        Circle isCorrectFit(const std::vector < Vector2<int> > &ballPoints, const ObjectCandidate &PossibleBall);
	
};

