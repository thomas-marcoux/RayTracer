#ifndef VECTOR3D_H
#define VECTOR3D_H

class Vector3D
{
public:
	float _x;
	float _y;
	float _z;
	float _magnitude;

public:
	Vector3D() : _x(0.0f), _y(0.0f), _z(0.0f) {}
	Vector3D(float x, float y, float z) : _x(x), _y(y), _z(z) {}
	Vector3D operator+(const Vector3D&);
	Vector3D operator*(float);
	Vector3D operator/(float);
	float magnitude();
	Vector3D normalize();
};


#endif // !VECTOR3D_H
