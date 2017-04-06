#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <G3D/G3DAll.h>

class RayTracer
{
public:
	RayTracer();
	~RayTracer();
	void rayTrace(G3D::Image*, G3D::Scene*, G3D::Camera*);
};

#endif // !RAYTRACER_H
