#pragma once
#ifndef __CVECTOR3
#define __CVECTOR3

#include <cmath>
using namespace std;

class CVector3
{
// Concrete class - public access
public:
	/*---------------------------------------------------------------------------------------------
		Data
	---------------------------------------------------------------------------------------------*/
    
    // Vector components
    float x;
	float y;
	float z;

	/*-----------------------------------------------------------------------------------------
		Constructors/Destructors
	-----------------------------------------------------------------------------------------*/
	// Default constructor - leaves values uninitialised (for performance)
	CVector3() {}

	// Construct by value
	CVector3( const float xIn, const float yIn, const float zIn)
	{
		x = xIn;
		y = yIn;
		z = zIn;
	}
	
	// Set the vector through a pointer to three floats
    void Set( const float* pfElts )
	{
		x = pfElts[0];
		y = pfElts[1];
		z = pfElts[2];
	}

	// Dot product of this with another vector
    float Dot( const CVector3& v ) const
	{
	    return x*v.x + y*v.y + z*v.z;
	}

	//Operators
	CVector3 operator + (const CVector3 &vectorToAdd)
	{
		return CVector3{ x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z };
	}
	CVector3 operator + (const float &value)
	{
		return CVector3{ x + value, y + value, z + value };
	}
	void operator += (const CVector3 &vectorToAdd)
	{
		x += vectorToAdd.x;
		y += vectorToAdd.y;
		z += vectorToAdd.z;
	}
	void operator += (const float &value)
	{
		x += value;
		y += value;
		z += value;
	}
	CVector3 operator - (const CVector3 &vectorToSub)
	{
		return CVector3{ x - vectorToSub.x, y - vectorToSub.y, z - vectorToSub.z };
	}
	CVector3 operator - (const float &value)
	{
		return CVector3{ x - value, y - value, z - value };
	}
	void operator -= (const CVector3 &vectorToAdd)
	{
		x -= vectorToAdd.x;
		y -= vectorToAdd.y;
		z -= vectorToAdd.z;
	}
	void operator -= (const float &value)
	{
		x -= value;
		y -= value;
		z -= value;
	}
	CVector3 operator*(float scalar)
	{
		return CVector3{ x * scalar, y * scalar, z * scalar };
	}
	CVector3 operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
	}
	CVector3 operator/(float scalar)
	{
		return CVector3{ x / scalar, y / scalar, z / scalar };
	}
	CVector3 operator/=(float scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
	}
};

// Subtracting of two given vectors (order is important - non-member version
inline CVector3 Subtract( const CVector3& v,  const CVector3& w  )
{
    return CVector3( v.x - w.x, v.y - w.y, v.z - w.z );
}

// Dot product of two given vectors (order not important) - non-member version
inline float DotProduct( const CVector3& v1, const CVector3& v2 )
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

// Cross product of two given vectors (order is important) - non-member version
inline CVector3 CrossProduct( const CVector3& v1, const CVector3& v2 )
{
	return CVector3(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

const float kfEpsilon = 0.5e-6f;    // For 32-bit floats

// Test if a float value is approximately 0. Epsilon value is the range around zero that
// is considered equal to zero. Default value requires zero to 6 decimal places
inline bool IsZero( const float x )
{
	return fabsf( x ) < kfEpsilon;
}


// 1 / Sqrt
inline float InvSqrt( const float x )
{
	return 1.0f / sqrtf( x );
}

// Return unit length vector in the same direction as given one
inline CVector3 Normalise( const CVector3& v )
{
	float lengthSq = v.x*v.x + v.y*v.y + v.z*v.z;

	// Ensure vector is not zero length (use BaseMath.h float approx. fn with default epsilon)
	if ( IsZero( lengthSq ) )
	{
		return CVector3(0.0f, 0.0f, 0.0f);
	}
	else
	{
		float invLength = InvSqrt( lengthSq );
		return CVector3(v.x * invLength, v.y * invLength, v.z * invLength);
	}
}

inline float VectorLength(const CVector3& v)
{
	float lengthSq = v.x*v.x + v.y*v.y + v.z*v.z;

	// Ensure vector is not zero length (use BaseMath.h float approx. fn with default epsilon)
	if (IsZero(lengthSq))
	{
		return 0.0f;
	}
	else
	{
		return sqrt(lengthSq);
	}
}

const CVector3 kXAxis(1.0f, 0.0f, 0.0f);
const CVector3 kYAxis(0.0f, 1.0f, 0.0f);
const CVector3 kZAxis(0.0f, 0.0f, 1.0f);

#endif