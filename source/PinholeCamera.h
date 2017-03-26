#ifndef PINHOLECAMERA_H
#define PINHOLECAMERA_H

#include "Point3D.h";
#include "Vector3D.h"

class PinholeCamera
{
protected:
	float z_near; //Negative distance from virtual image plane to center of projection
	float verticalFieldOfView; //theta_y

public:
	void getPrimaryRay(float x, float y, int width, int height, Point3D& P, Vector3D& w) const;
};

#endif // !PINHOLECAMERA_H
