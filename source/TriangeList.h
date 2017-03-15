#ifndef TRIANGLELIST_H
#define TRIANGLELIST_H

#include <G3D/G3DAll.h>
#include "Point3D.h"
#include "MyTriangle.h"

class TriangleList
{
public:
		Array<Point3D>	vertexArray;
		Array<int>		indexArray;

public:
		MyTriangle	triangle(int t) const;
		int			size() const { return indexArray.size() / 3; }
};

#endif // !TRIANGLELIST_H
