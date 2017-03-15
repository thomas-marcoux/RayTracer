#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <G3D/G3DAll.h>
#include "Point3D.h"

class MyTriangle
{
private:
	Point3D	m_vertex[3];

public:
	MyTriangle(const Point3D&, const Point3D&, const Point3D&);
	const Point3D& vertex(int i) const { return m_vertex[i]; }
};

#endif // !TRIANGLE_H
