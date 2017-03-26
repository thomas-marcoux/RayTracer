#include <cmath>
#include "Vector3D.h"

Vector3D Vector3D::operator+(const Vector3D &o)
{
	Vector3D r(*this);
	r._x += o._x;
	r._y += o._y;
	r._z += o._z;
	return r;
}

Vector3D Vector3D::operator*(float f)
{
	Vector3D r(*this);
	r._x *= f;
	r._y *= f;
	r._z *= f;
	return r;
}

Vector3D Vector3D::operator/(float f)
{
	Vector3D r(*this);
	r._x /= f;
	r._y /= f;
	r._z /= f;
	return r;
}

float Vector3D::magnitude()
{
	return sqrt(_x * _x + _y * _y + _z * _z);
}

Vector3D Vector3D::normalize()
{
	return (*this / magnitude());
}