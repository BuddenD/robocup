#ifndef MOBILEOBJECTS_H
#define MOBILEOBJECTS_H


#include "Object.h"
#include "../../Tools/Math/Vector2.h"
#include "../../Tools/Math/Vector3.h"

class MobileObject : public Object{
	private:
                Vector2<float> estimatedFieldLocation;
                Vector2<float> estimatedFieldLocationError;
                Vector2<float> estimatedVelocity;
                Vector2<float> estimatedVelocityError;
                Vector3<float> estimatedRelativeLocation;
                Vector3<float> estimatedRelativeLocationError;
	public:
		~MobileObject();
                MobileObject(float x = 0, float y = 0, int initID = -1, const std::string& initName = "Unknown");
                MobileObject(const Vector2<float>& newAbsoluteLocation, int initID = -1, const std::string& initName = "Unknown");
                MobileObject(const MobileObject& srcObj);
                void updateAbsoluteLocation(const Vector2<float>& newAbsoluteLocation);
                void updateAbsoluteLocationError(const Vector2<float>& newAbsoluteLocationError);
		void updateObjectLocation(float x, float y, float sdx, float sdy);
		
                void updateVelocity(const Vector2<float>& newVelocity);
                void updateVelocityError(const Vector2<float>& newVelocityError);
		void updateObjectVelocities(float velX, float velY, float sdVelX, float sdVelY);
	
                void updateWorldModelRelativeLocation(const Vector3<float>& newWMRelLoc);
                void updateWorldModelRelativeLocationError(const Vector3<float>& newWNRelLocError);
		void updateWorldModelRelativeVaribles(float wmDistance, float wmBearing, float wmElevation, float sdWmDistance, float sdWmBearing, float sdWmElevation );

		//Access:
                Vector2<float> getEstimatedFieldLocation() const {return estimatedFieldLocation;}
                Vector2<float> getEstimatedFieldLocationError() const {return estimatedFieldLocationError;}
                Vector2<float> getEstimatedVelocity() const {return estimatedVelocity;}
                Vector2<float> getEstimatedVelocityError() const {return estimatedVelocityError;}
                Vector3<float> getEstimatedRelativeLocation() const {return estimatedRelativeLocation;}
		
		//ShortCuts (single variableAccess):
                float X() const {return estimatedFieldLocation.x;}
                float Y() const {return estimatedFieldLocation.y;}
                float sdX() const {return estimatedFieldLocationError.x;}
                float sdY() const {return estimatedFieldLocationError.y;}
                float velX() const {return estimatedVelocity.x;}
                float velY() const {return estimatedVelocity.y;}
                float sdVelX() const {return estimatedVelocityError.x;}
                float sdVelY() const {return estimatedVelocityError.y;}
                float estimatedDistance() const {return estimatedRelativeLocation.x;}
                float estimatedBearing() const {return estimatedRelativeLocation.y;}
                float estimatedElevation() const {return estimatedRelativeLocation.z;}
                float sdEstimatedDistance() const {return estimatedRelativeLocationError.x;}
                float sdEstimatedBearing() const {return estimatedRelativeLocationError.y;}
                float sdEstimatedElevation() const {return estimatedRelativeLocationError.z;}
		
};
#endif
