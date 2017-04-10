#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <G3D/G3DAll.h>

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
	bool rayTrace(std::shared_ptr<Image>&);

private:
	void trace(Point2int32 const&);
	Radiance3 getRadiance();

private:
	std::unique_ptr<TriTree>	m_surfaces;
	std::shared_ptr<Camera>		m_camera;
	Array<shared_ptr<Light>>	m_lights;
	std::shared_ptr<Image>		m_image;
	bool						m_multithreading;
	bool						m_fixed_primitives;
	int							m_rays_per_pixel;
};

#endif // !RAYTRACER_H
