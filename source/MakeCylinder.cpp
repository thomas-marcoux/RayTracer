#include <G3D/G3DAll.h>

#define PI 3.14159265358979323846f
#define PI2 PI * 2


void	writeVertices(TextOutput& file, float radius, float height, int nChords)
{
	float rotation = (PI / nChords) * 2;
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
	int k = nChords / 2;
	int l = nChords + k;
	int y = 0;

	//Bottom Circle
	y = 0;
	for (i = 1; i < nChords; ++i)
		file.printf("%d %d %d %d\n", nVertices, i + 1, i, y);
	file.printf("%d %d %d %d\n", nVertices, 1, i, y);
	//Bottom Mesh
	for (i = 1; i < nChords; ++i)
		file.printf("%d %d %d %d\n", nVertices, i + nChords, i + 1, i);
	file.printf("%d %d %d %d\n", nVertices, i + nChords, 1, i);
	//Top Mesh
	for (i=nChords+1; i < j; ++i)
		file.printf("%d %d %d %d\n", nVertices, i, i + 1, i + 1 - nChords);
	file.printf("%d %d %d %d\n", nVertices, i, nChords+1, 1);
	//Top Circle
	y = j + 1;
	for (i = nChords+1; i < j; ++i)
		file.printf("%d %d %d %d\n", nVertices, i+1, i, y);
	file.printf("%d %d %d %d\n", nVertices, nChords + 1, i, y);
}

void	makeCylinder(float radius, float height)
{
	TextOutput		file("../data-files/model/cylinder.off");
	int	nChords;
	int	nVertices;
	int	nFaces;
	int	nEdges;
	int precision = 4;

	nChords = (int)(radius) * precision * 4;
	nVertices = nChords * 2 + 2;
	nFaces = (nVertices-2) * 2;
	nEdges = 0;	
	file.printf("OFF\n%d %d %d\n", nVertices, nFaces, nEdges);
	file.printf("%f %f %f\n", 0.0, 0.0, 0.0);
	writeVertices(file, radius, 0.0, nChords);
	writeVertices(file, radius, height, nChords);
	file.printf("%f %f %f\n", 0.0, height, 0.0);
	writeIndex(file, nChords);
	file.commit();
}