// SBAJONES 2066906 Samuel Jones
#include "Vector3D.h"

Vector3D::Vector3D()
{

}

Vector3D::Vector3D(float X, float Y, float Z)
{
	x = X;
	y = Y;
	z = Z;
}

Vector3D::~Vector3D()
{
}

void Vector3D::SetX(float X)
{
	x = X;
}

void Vector3D::SetY(float Y)
{
	y = Y;
}

void Vector3D::SetZ(float Z)
{
	z = Z;
}

Vector3D Vector3D::Normalise()
{
	float X = (x * x);
	float Y = (y * y);
	float Z = (z * z);

	float length = sqrtf(X + Y + Z);
	X /= length;
	Y /= length;
	Z /= length;

	return Vector3D(X, Y, Z);
}

// Gets the distance between two vectors and returns the value as a float
float Vector3D::GetDistance(Vector3D finalVector)
{
	float X = x - finalVector.x;
	float Y = y - finalVector.y;
	float Z = z - finalVector.z;
	X *= X;
	Y *= Y;
	Z *= Z;
	float distance = sqrtf(X + Y + Z);

	return distance;
}
