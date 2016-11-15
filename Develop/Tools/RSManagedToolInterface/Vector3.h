#pragma once

#include "MMath.h"

using namespace System;

namespace RSManaged
{
	public __value class Vector3
	{
	public:
		float x, y, z;

		Vector3(): x(0), y(0), z(0) {};
		Vector3(float nx, float ny, float nz) : x(nx), y(ny), z(nz) {};

		static bool op_Equality(Vector3 o1, Vector3 o2)
		{
			return (o1.x == o2.x && o1.y == o2.y && o1.z == o2.z);	
		}

		static Vector3 op_Addition(Vector3 o1, Vector3 o2)
		{
			return Vector3(o1.x + o2.x, o1.y + o2.y, o1.z + o2.z);	
		}

		static Vector3 op_Subtraction(Vector3 o1, Vector3 o2)
		{
			return Vector3(o1.x - o2.x, o1.y - o2.y, o1.z - o2.z);	
		}

		static Vector3 op_Multiply(Vector3 o, float scal)
		{
			return Vector3(o.x * scal, o.y * scal, o.z * scal);
		}

		static Vector3 op_Multiply(float scal, const Vector3& o)
		{
			return Vector3(o.x * scal, o.y * scal, o.z * scal);
		}

		static Vector3 op_Division(Vector3 o, float scal)
		{
			return Vector3(o.x / scal, o.y / scal, o.z / scal);
		}

		void Set( float nx,  float ny,  float nz) {x=nx; y=ny; z=nz; }
		void Set( Vector3 p) { x=p.x; y=p.y; z=p.z;}

		double GetLength()  { return System::Math::Sqrt(x*x + y*y + z*z); }
		double GetLengthSQ()  { return x*x + y*y + z*z; }

		float DotProduct( Vector3 other) 
		{
			return x*other.y + y*other.y + z*other.z;
		}

		double GetDistanceFrom( Vector3 other) 
		{
			double vx = x - other.x; double vy = y - other.y; double vz = z - other.z;
			return Math::Sqrt(vx*vx + vy*vy + vz*vz);
		}

		float GetDistanceFromSQ( Vector3 other) 
		{
			float vx = x - other.x; float vy = y - other.y; float vz = z - other.z;
			return (vx*vx + vy*vy + vz*vz);
		}

		Vector3 CrossProduct( Vector3 p) 
		{
			return Vector3(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
		}

		bool IsBetweenPoints( Vector3 begin,  Vector3 end) 
		{
			float f = (float)(end - begin).GetLengthSQ();
			return (float)GetDistanceFromSQ(begin) < f && 
				(float)GetDistanceFromSQ(end) < f;
		}

		Vector3 Normalize()
		{
			float inv = (float)1.0 / (float)GetLength();
			x *= inv;
			y *= inv;
			z *= inv;
			return *this;
		}

		void SetLength(float newlength)
		{
			Normalize();
			(*this) = (*this)*newlength;
		}

		void Invert()
		{
			x *= -1.0f;
			y *= -1.0f;
			z *= -1.0f;
		}

		void RotateXZBy(double degrees, Vector3 center)
		{
			degrees *=GRAD_PI2;
			float cs = (float)Math::Cos(degrees);
			float sn = (float)Math::Sin(degrees);
			x -= center.x;
			z -= center.z;
			Set(x*cs - z*sn, y, x*sn + z*cs);
			x += center.x;
			z += center.z;
		}

		void RotateXYBy(double degrees,  Vector3 center)
		{
			degrees *=GRAD_PI2;
			float cs = (float)Math::Cos(degrees);
			float sn = (float)Math::Sin(degrees);
			x -= center.x;
			y -= center.y;
			Set(x*cs - y*sn, x*sn + y*cs, z);
			x += center.x;
			y += center.y;
		}

		void RotateYZBy(double degrees,  Vector3 center)
		{
			degrees *=GRAD_PI2;
			float cs = (float)Math::Cos(degrees);
			float sn = (float)Math::Sin(degrees);
			z -= center.z;
			y -= center.y;
			Set(x, y*cs - z*sn, y*sn + z*cs);
			z += center.z;
			y += center.y;
		}

		Vector3 GetInterpolated( Vector3 other, float d) 
		{
			float inv = 1.0f - d;
			return Vector3(other.x*inv + x*d,
								other.y*inv + y*d,
								other.z*inv + z*d);
		}

		bool Equals(Object* rhs) 
		{
			Vector3* c = dynamic_cast<Vector3 *>(rhs);

			if(!c) 
				return false;

			return c->x == x && c->y == y && c->z == z;
		}
	};
}
