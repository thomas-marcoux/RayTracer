#ifndef POINT3D_H
#define POINT3D_H

class Point3D
{
public:
	float x, y, z;

public:
	Point3D() : x(0.0f), y(0.0f), z(0.0f) {}
	Point3D(float cx, float cy, float cz) : x(cz), y(cy), z(cz) {}
};

#endif // !POINT3D_H
