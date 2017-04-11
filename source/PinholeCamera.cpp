#include <cmath>
#include "PinholeCamera.h"

PinholeCamera::PinholeCamera(float z, float theta)
{
	z_near = z;
	verticalFieldOfView = theta;
}

// C++ A Camera Model [_rn_camera] from http://graphicscodex.com 
/* x, y, width, and height in pixels; P in meters */
void PinholeCamera::getPrimaryRay(float x, float y, int width, int height, Point3& P, Vector3& w) const {
	// Compute the side of a square at $z = -1$ based on our vertical top-to-bottom field of view
	const float side = -2.0f * tan(verticalFieldOfView / 2.0f);

	// Note that I invert the y-axis because we're moving from the 2D y=down to the 3D y=up coordinate system
	P = Point3(z_near * (x / width - 0.5f) * side * width / height,
		z_near * -(y / height - 0.5f) * side,
		z_near);

	// The incoming direction is simply that from the origin to $P$
	w = P.direction();
}
