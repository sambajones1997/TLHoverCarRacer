// SBAJONES 2066906 Samuel Jones
#pragma once
#include <math.h>
class Vector3D
{
private:

	// Declaring the public variables
	float x;
	float y;
	float z;

public:
	Vector3D();
	Vector3D(float X, float Y, float Z);
	~Vector3D();

	// Declaring the procedures the class can handle

	// Setting individual values
	void SetX(float X);	// Set the x component of the 3D vector
	void SetY(float Y);	// Set the y component of the 3D vector
	void SetZ(float Z); // Set the z component of the 3D vector
	Vector3D Normalise();	// Normalising the vector to give a value between 0.0f and 1.0f for each value
	float GetDistance(Vector3D finalVector); // Gets the distance as a float between two vectors

	// DECLARING PROCEDURES TO GET THE INDIVIDUAL VALUES OF THE VECTOR

	float GetX() const
	{
		return x;
	}
	float GetY() const
	{
		return y;
	}
	float GetZ() const
	{
		return z;
	}

	// Declaring the procedures for mathematical uses (OPERATORS)

	// Returns the value when two vectors are added together
	Vector3D operator +(Vector3D v) const
	{
		return Vector3D{ x + v.x, y + v.y, z + v.z };
	}

	// Returns the value when two vectors are subtracted
	Vector3D operator -(Vector3D v) const
	{
		return Vector3D{ x - v.x, y - v.y, z - v.z };
	}

	// Returns the value when two vectors are divided
	Vector3D operator /(Vector3D v) const
	{
		return Vector3D{ x / v.x, y / v.y, z / v.z };
	}

	// Returns the value when two vectors are multiplied
	Vector3D operator *(Vector3D v) const
	{
		return Vector3D{ x * v.x, y * v.y, z * v.z };
	}

	// Returns the value when a vector is added by shorthand e.g "vector1 += vector 2;"
	Vector3D operator +=(Vector3D v) const
	{
		return Vector3D{ x + v.x, y + v.y, z + v.z };
	}

	// Returns the value of a shorthand subtraction between two vectors i.e "vector1 -= vector2;"
	Vector3D operator -=(Vector3D v) const
	{
		return Vector3D{ x - v.x, y - v.y, z - v.z };
	}

	// Returns the value of a shorthand multiplication between two vectors i.e "vector1 *= vector2;"
	Vector3D operator *=(Vector3D v) const
	{
		return Vector3D{ x * v.x, y * v.y, z * v.z };
	}

	// Returns the value of a shorthand division between two vectors i.e "vector1 /= vector2;"
	Vector3D operator /=(Vector3D v) const
	{
		return Vector3D{ x / v.x, y / v.y, z / v.z };
	}

	// MATHEMATICAL OPERATIONS WITH FLOATS

	// Long Hand Vector-float division
	Vector3D operator /(float f) const
	{
		return Vector3D{ x / f, y / f, z / f };
	}
	// Shorthand Vector3D-float division
	Vector3D operator /=(float f) const
	{
		return Vector3D{ x / f, y / f, z / f };
	}
	// Longhand Vector3D-float multiplication
	Vector3D operator *(float f) const
	{
		return Vector3D{ x * f, y * f, z *f };
	}
	// Shorthand Vector3D-float multiplication
	Vector3D operator *=(float f) const
	{
		return Vector3D{ GetX() * f, GetY() * f, GetZ() *f };
	}

	// MATHEMATICAL OPERATIONS WITH INTEGERS

	// Long Hand Vector-integer division
	Vector3D operator /(int i) const
	{
		return Vector3D{ x / i, y / i, z / i };
	}
	// Short Hand Vector-integer division
	Vector3D operator /=(int i) const
	{
		return Vector3D{ x / i, y / i, z / i };
	}
	// Long Hand Vector-integer multiplication
	Vector3D operator *(int i) const
	{
		return Vector3D{ x * i, y * i, z * i };
	}
	// Short Hand Vector-integer multiplication
	Vector3D operator *=(int i) const
	{
		return Vector3D{ x * i, y * i, z * i };
	}

};

