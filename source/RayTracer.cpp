#include "RayTracer.h"

RayTracer::RayTracer()
{
	m_surfaces = std::make_unique<G3D::TriTree>();
	m_multithreading = true;
	m_fixed_primitives = true;
	m_rays_per_pixel = 0;
	m_pinhole_camera = std::make_unique<PinholeCamera>(-3.0f, 0.785398f); //45° FOV
	m_sphere1 = Sphere(Point3(0, 0, -5), 1);
	m_sphere2 = Sphere(Point3(3, 0, -5), 1);
	m_epsilon = 0.0001f;
	m_bump = 0.001f;
}

RayTracer::~RayTracer()
{

}

//Return distance to intersection with plane
float RayTracer::intersectRayPlane(Point3 const& C, Vector3 const& n, Point3 const& P, Vector3 const& w)
{
	float d = n.dot(w);
	if (abs(d) <= m_epsilon) { return INFINITY; } //if nearly parallel
	float t = (C - P).dot(n) / d;
	return (t >= 0) ? t : INFINITY;
}

//Returns distance to first intersection with sphere
float RayTracer::intersectRaySphere(Sphere const& s, Point3 const& P, Vector3 const& w)
{
	Vector3 v = P - s.center;
	float b = 2.0f * w.dot(v);
	float c = v.dot(v) - s.radius * s.radius;
	float t = b * b - 4.0f * c; 	//Discriminant
	if (t < 0.0f) return INFINITY; //No intersection
	float sq = sqrt(t);
	float t_0 = (-b - sq) * 0.5f;
	float t_1 = (-b + sq) * 0.5f;
	//Choose closest
	if (t_0 > 0.0f) return t_0;
	if (t_1 > 0.0f) return t_1;
	return INFINITY; //Ray began past sphere
}

//Return distance to triangle V and store barycentric coordinates to b
float RayTracer::intersectRayTriangle(Point3 const V[3], float b[3], Point3 const& P, Vector3 const& w)
{
	Vector3 e1 = V[1] - V[0];
	Vector3 e2 = V[2] - V[0];
	//Normal vector
	Vector3 n = e1.cross(e2).direction();
	Vector3 q = w.cross(e2);
	float a = e1.dot(q);
	//faces back of triangle or almost parallel
	if (n.dot(w) >= 0 || abs(a) <= m_epsilon)
		return INFINITY;
	Vector3 s = (P - V[0]) / a;
	Vector3 r = s.cross(e1);
	b[0] = s.dot(q);
	b[1] = r.dot(w);
	b[2] = 1.0f - b[0] - b[1];
	//Intersection outside triangle
	if (b[0] < 0 || b[1] < 0 || b[2] < 0)
		return INFINITY;
	float t = e2.dot(r);
	return (t >= 0) ? t : INFINITY;
}

//Compute outgoing radiance
Radiance3 RayTracer::getL_i(Point3 const& P, Vector3 const& w)
{
	//const shared_ptr<Surfel>& s = findFirstIntersection(P, w);
	float s1 = intersectRaySphere(m_sphere1, P, w);
	float s2 = intersectRaySphere(m_sphere2, P, w);
	float s = (s1 < s2) ? s1 : s2;
	/*
	float p = intersectRayPlane(Point3(0, 0, -5), Vector3(0, 0, 1), P, w);
	Point3 V[3] = { Point3(-5,0,0), Point3(0,-5,0), Point3(0,0,-5) };
	float b[3];
	float r = intersectRayTriangle(V, b, P, w);
	*/

	if (s != INFINITY)
		return Radiance3::one();
	return Radiance3::zero();
}

shared_ptr<Surfel>	RayTracer::findFirstIntersection(Ray const& r)
{
	return m_surfaces->intersectRay(r);
}

bool RayTracer::isUnobstructed(Point3 const& P1, Point3 const& P2)
{
	Vector3 dist = P2 - P1;
	Ray ray = Ray::fromOriginAndDirection(P1, dist / dist.length(), 0.0f, dist.length() - 0.001f);
	TriTree::Hit ignore;
	return ! m_surfaces->intersectRay(ray, ignore, TriTree::OCCLUSION_TEST_ONLY | TriTree::DO_NOT_CULL_BACKFACES);
}

Radiance3 RayTracer::getL_o(shared_ptr<Surfel> const& s, Vector3 const& wo, Random& rng)
{
	Radiance3 L_o = s->emittedRadiance(wo);
	Point3 P = s->position;
	Vector3 v = s->shadingNormal;

	//Sum illumination from all lights
	for (shared_ptr<Light> const& light : m_lights)
	{
		Point3 L_pos = light->position().xyz();
		if (light->producesDirectIllumination() && isUnobstructed(L_pos, s->position + s->geometricNormal * m_bump))
		{
			Vector3 wi;
			wi = (L_pos - P).direction();
			Biradiance3 B = light->biradiance(P);
			Color3 f = s->finiteScatteringDensity(wi, wo);
			L_o += B * f * abs(wi.dot(v));
		}
	}
	L_o += s->reflectivity(rng) * (Radiance3::fromARGB(0x305050) * 0.3f); //RGB format: (xFF0000) is red
	return L_o;
}

Radiance3 RayTracer::getL_i(Ray const& r, Random& rng)
{
	shared_ptr<Surfel> s = findFirstIntersection(r);
	Radiance3 L_o = Radiance3(r.direction()) / 2 + Color3(0.5f);

	if (s)
		L_o = getL_o(s, -r.direction(), rng);
	return L_o;
}

void RayTracer::trace(Point2int32 const& pixel, Random& rng)
{
	if (m_fixed_primitives)
	{
		Point3	P;
		Vector3	w;

		m_pinhole_camera->getPrimaryRay(pixel.x + 0.5f, pixel.y + 0.5f, m_image->width(), m_image->height(), P, w);
		m_image->set(pixel.x, pixel.y, getL_i(P, w));
	}
	else
	{
		Radiance3 L_i = Radiance3::zero();
		Ray r = m_camera->worldRay(pixel.x + 0.5f, pixel.y + 0.5f, m_image->bounds());
		for (int i = 0; i < m_rays_per_pixel; ++i)
			L_i += getL_i(r, rng);
		m_image->set(pixel.x, pixel.y, L_i / (float)m_rays_per_pixel);
	}
}

bool RayTracer::rayTrace(std::shared_ptr<Image>& image)
{
	if (!image)
		return false;
	m_image = image;
	Point2int32	start = Point2int32(0, 0);
	Point2int32	end = Point2int32(image->width(), image->height());


	if (m_multithreading)
		G3D::Thread::runConcurrently(start, end, [this](Point2int32 pixel) { trace(pixel, Random::threadCommon()); });
	else
	{
		for (Point2int32 pixel; pixel.y < image->height(); ++pixel.y)
			for (pixel.x = 0; pixel.x < image->width(); ++pixel.x)
				trace(pixel, Random::threadCommon());
	}
	return true;
}