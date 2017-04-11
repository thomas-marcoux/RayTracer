#include "RayTracer.h"

RayTracer::RayTracer()
{
	m_surfaces = std::make_unique<G3D::TriTree>();
	m_multithreading = true;
	m_fixed_primitives = true;
	m_rays_per_pixel = 0;
	m_pinhole_camera = std::make_unique<PinholeCamera>(-3.0f, 0.785398f); //45° FOV
	m_sphere1 = Sphere(Point3(0, 0, -5), 1);
	m_epsilon = 0.05f;
}

RayTracer::~RayTracer()
{

}

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

//Compute outgoing radiance
Radiance3 RayTracer::getL_i(Point3 const& P, Vector3 const& w)
{
	//const shared_ptr<Surfel>& s = findFirstIntersection(P, w);
	float p = intersectRayPlane(Point3(0, 0, -5), Vector3(0, 0, 1), P, w);
	float s = intersectRaySphere(m_sphere1, P, w);

	if (s != INFINITY) //if (p != INFINITY)
		return Radiance3::one();
	return Radiance3::zero();
}

void RayTracer::trace(Point2int32 const& pixel)
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
		Ray r = m_camera->worldRay(pixel.x + 0.5f, pixel.y + 0.5f, m_image->bounds());
		Radiance3 radiance;
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
		G3D::Thread::runConcurrently(start, end, [this](Point2int32 pixel) { trace(pixel); });
	else
	{
		for (Point2int32 pixel; pixel.y < image->height(); ++pixel.y)
			for (pixel.x = 0; pixel.x < image->width(); ++pixel.x)
				trace(pixel);
	}
	return true;
}