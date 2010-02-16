#include "AmbiguousObject.h"

AmbiguousObject::AmbiguousObject()
{
}
AmbiguousObject::~AmbiguousObject()
{
}

void AmbiguousObject::addPossibleObjectID(int ID)
{
        PossibleObjectIDs.push_back(ID);
}

void AmbiguousObject::setPossibleObjectIDs(vector<int> VectorOfIDs)
{
	PossibleObjectIDs = VectorOfIDs;
}

bool AmbiguousObject::isObjectAPossibility(int ID)
{
	bool isInVector = false;
        for(int i = 0; i < PossibleObjectIDs.size(); i++)
	{
                if(PossibleObjectIDs[i] == ID)
		{
			return true;
		}
	}
	return isInVector;
}
