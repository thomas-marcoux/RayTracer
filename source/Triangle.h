#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Point3D.h"

class MyTriangle
{
private:
	Point3D	m_vertex[3];

public:
	MyTriangle(const Point3D& P0, const Point3D& P1, const Point3D& P2);
	const Point3D& vertex(int i) const;
};

#endif // !TRIANGLE_H
