#include "RayTracer.h"

RayTracer::RayTracer()
{
	m_surfaces = std::make_unique<G3D::TriTree>();
	m_multithreading = true;
	m_fixed_primitives = true;
	m_rays_per_pixel = 0;
}

RayTracer::~RayTracer()
{

}

void RayTracer::trace(Point2int32 const& pixel)
{
	Ray r = m_camera->worldRay(pixel.x + 0.5, pixel.y + 0.5, m_image->bounds());

	if (m_fixed_primitives)
	{

	}
	else
	{

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