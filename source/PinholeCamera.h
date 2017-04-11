#ifndef PINHOLECAMERA_H
#define PINHOLECAMERA_H

#include <G3D/G3DAll.h>

class PinholeCamera
{
protected:
	float z_near; //Negative distance from virtual image plane to center of projection
	float verticalFieldOfView; //theta_y

public:
	PinholeCamera(float, float);
	void getPrimaryRay(float x, float y, int width, int height, Point3& P, Vector3& w) const;
	void setZNear(float z) { z_near = z; }
};

#endif // !PINHOLECAMERA_H
