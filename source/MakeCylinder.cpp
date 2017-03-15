#include <G3D/G3DAll.h>
#include "TriangeList.h"

#define PI 3.14159265358979323846 
#define PI2 PI * 2


void	writeVertices(TextOutput& file, float radius, float height, int precision)
{
	float rotation = PI / precision;
	float x = 0;
	float y = height;
	float z = 0;

	for (float angle = 0; angle <= PI2; angle += rotation)
	{
		x = cos(angle) * radius;
		z = sin(angle) * radius;
		file.printf("%f %f %f\n", x, y, z);
	}
}

void	writeIndex(TextOutput& file, int nChords)
{
	int	nVertices = 3;
	int i;
	int j = nChords * 2;
	int y = 0;

	for (i = 1; i < nChords; ++i)
		file.printf("%d %d %d %d\n", nVertices, y, i, i+1);
	file.printf("%d 0 %d %d\n", nVertices, i, 1);
	for (i = 1; i < nChords; ++i)
		file.printf("%d %d %d %d\n", nVertices, i, i + 1, i + nChords);
	file.printf("%d %d %d %d\n", nVertices, i, 1, i + nChords);
	for (i = nChords+1; i < j; ++i)
		file.printf("%d %d %d %d\n", nVertices, i, i + 1, i + 1 - nChords);
	file.printf("%d %d %d %d\n", nVertices, i, nChords+1, 1);
	y = j + 1;
	for (i = nChords+1; i < j; ++i)
		file.printf("%d %d %d %d\n", nVertices, y, i, i + 1);
}

void	makeCylinder(float radius, float height)
{
	TriangleList	tList;
	TextOutput		file("../data-files/model/cylinder.off");
	int	nChords;
	float chordLength;
	int	nVertices;
	int	nFaces;
	int	nEdges;
	int precision = 4;

	nChords = radius * precision * 4;
	nVertices = nChords * 2;
	nFaces = nVertices * 2;
	nEdges = 0;
	file.printf("OFF\n%d %d %d\n", nVertices, nFaces, nEdges);
	file.printf("%f %f %f\n", 0.0, 0.0, 0.0);
	writeVertices(file, radius, 0.0, precision);
	writeVertices(file, radius, height, precision);
	file.printf("%f %f %f\n", 0.0, height, 0.0);
	writeIndex(file, nChords);
	file.commit();
}