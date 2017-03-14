#ifndef TRIANGLELIST_H
#define TRIANGLELIST_H

#include <G3D/G3DAll.h>
#include "Point3D.h"
#include "Triangle.h"

class TriangleList
{
public:
		Array<Point3> vertexArray;

		MyTriangle triangle(int t) const {
			return MyTriangle
			(vertexArray[3 * t],
				vertexArray[3 * t + 1],
				vertexArray[3 * t + 2]);
		}

		int size() const {
			return vertexArray.size() / 3;
		}
	};

};

#endif // !TRIANGLELIST_H
