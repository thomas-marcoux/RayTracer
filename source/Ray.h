#ifndef RAY_H
#define RAY_H

#include "Point3D.h"
#include "Vector3D.h"

class Ray
{
public:
	Point3D	P; //origin
	Vector3D w; //direction

public:
	Ray() {}
};

#endif // !RAY_H
