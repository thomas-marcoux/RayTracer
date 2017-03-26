#ifndef POINT3D_H
#define POINT3D_H

#include "Vector3D.h"

class Point3D
{
public:
	float _x;
	float _y;
	float _z;

public:
	Point3D() : _x(0.0f), _y(0.0f), _z(0.0f) {}
	Point3D(float x, float y, float z) : _x(x), _y(y), _z(z) {}
	Vector3D direction();
};

#endif // !POINT3D_H
