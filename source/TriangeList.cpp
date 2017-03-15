#include "MyTriangle.h"
#include "TriangeList.h"

MyTriangle TriangleList::triangle(int t) const
{
		return MyTriangle (vertexArray[indexArray[3 * t]],
			vertexArray[indexArray[3 * t + 1]],
			vertexArray[indexArray[3 * t + 2]]);
}
