#include "MyTriangle.h"


MyTriangle::MyTriangle(const Point3D& P0, const Point3D& P1, const Point3D& P2)
{
	//Copy points instead?
	m_vertex[0] = P0;
	m_vertex[1] = P1;
	m_vertex[2] = P2;
}
