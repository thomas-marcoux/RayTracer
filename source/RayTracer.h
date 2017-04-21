#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <G3D/G3DAll.h>
#include "PinholeCamera.h"

class RayTracer
{
public:
	RayTracer();
	~RayTracer();
	void useMultiThreading(bool b) { (b) ? m_multithreading = true : m_multithreading = false; }
	void useFixedPrimitives(bool b) { (b) ? m_fixed_primitives = true : m_fixed_primitives = false;}
	void loadSurfaces(Array<std::shared_ptr<Surface>>& surfaces) { m_surfaces->setContents(surfaces); }
	void loadLights(std::shared_ptr<G3D::Scene> const& scene) { scene->getTypedEntityArray<Light>(m_lights); }
	void loadCamera(std::shared_ptr<G3D::Camera> camera) { m_camera = camera; }
	void setRayPerPixel(int i) { m_rays_per_pixel = i; }
	void setZNear(int z_near) { m_pinhole_camera->setZNear((float)z_near); }
	bool rayTrace(std::shared_ptr<Image>&);

private:
	void trace(Point2int32 const&, Random& rng);
	Radiance3 getL_i(Ray const&, Random& rng);
	Radiance3 getL_i(Point3 const&, Vector3 const&);
	Radiance3 getL_o(shared_ptr<Surfel> const&, Vector3 const&, Random& rng);
	float intersectRaySphere(Sphere const& s, Point3 const& P, Vector3 const& w);
	float intersectRayPlane(Point3 const& C, Vector3 const& n, Point3 const& P, Vector3 const& w);
	float RayTracer::intersectRayTriangle(Point3 const V[3], float b[3], Point3 const& P, Vector3 const& w);
	shared_ptr<Surfel>	findFirstIntersection(Ray const&);
	bool isUnobstructed(Point3 const&, Point3 const&);

private:
	std::unique_ptr<TriTree>	m_surfaces;
	std::shared_ptr<Camera>		m_camera;
	Array<shared_ptr<Light>>	m_lights;
	std::shared_ptr<Image>		m_image;
	bool						m_multithreading;
	bool						m_fixed_primitives;
	int							m_rays_per_pixel;
	float						m_bump;
	//Fixed primitives attributes
	std::unique_ptr<PinholeCamera>	m_pinhole_camera;
	float						m_epsilon;
	Sphere						m_sphere1;
	Sphere						m_sphere2;
};

#endif // !RAYTRACER_H
