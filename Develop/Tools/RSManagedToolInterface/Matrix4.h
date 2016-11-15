#pragma once

#include "MMath.h"
#include "Vector3.h"

using namespace System;

namespace RSManaged
{
	public __value struct Matrix4
	{
	public:
		float Members __nogc [16];

		Matrix4()
		{
			MakeIdentity();
		}

		void MakeIdentity()
		{
			for (int i=0; i<16; ++i)
				Members[i] = 0.0f;

			Members[0] = Members[5] = Members[10] = Members[15] = 1;
		}

		__property inline float get_M(int row, int col) 
		{
			if (row < 0 || row >= 4 ||
				col < 0 || col >= 4)
				throw new System::Exception(new System::String("Invalid index for accessing matrix members"));

			return Members[col * 4 + row];
		}

		inline void set_M(int row, int col, float v)
		{
			if (row < 0 || row >= 4 ||
				col < 0 || col >= 4)
				throw new System::Exception(new System::String("Invalid index for accessing matrix members"));

			Members[col * 4 + row] = v;
		}

		static Matrix4 op_Multiply(Matrix4 a, Matrix4 b)
		{
			Matrix4 tmtrx;

			#define m1 a.Members
			#define m2 b.Members
			#define m3 tmtrx.Members

			m3[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
			m3[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
			m3[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
			m3[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];
			
			m3[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
			m3[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
			m3[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
			m3[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];
			
			m3[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
			m3[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
			m3[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
			m3[11] = m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];
			
			m3[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
			m3[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
			m3[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
			m3[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];

			#undef m1
			#undef m2
			#undef m3

			return tmtrx;
		}

		inline bool IsIdentity()
		{
			for (int i=0; i<4; ++i)
				for (int j=0; j<4; ++j)
					if (j != i)
					{
						if (get_M(i,j) < -0.0000001f ||
							get_M(i,j) >  0.0000001f)
							return false;
					}
					else
					{
						if (get_M(i,j) < 0.9999999f ||
							get_M(i,j) > 1.0000001f)
							return false;
					}

			return true;
		}

		void SetTranslation( const Vector3 translation )
		{
			Members[12] = translation.x;
			Members[13] = translation.y;
			Members[14] = translation.z;
		}

		Vector3 GetTranslation()
		{
			return Vector3(Members[12], Members[13], Members[14]);
		}

		void SetInverseTranslation( const Vector3 translation )
		{
			Members[12] = -translation.x;
			Members[13] = -translation.y;
			Members[14] = -translation.z;
		}

		void SetRotationRadians( const Vector3 rotation )
		{
			double cr = Math::Cos( rotation.x );
			double sr = Math::Sin( rotation.x );
			double cp = Math::Cos( rotation.y );
			double sp = Math::Sin( rotation.y );
			double cy = Math::Cos( rotation.z );
			double sy = Math::Sin( rotation.z );

			Members[0] = (float)( cp*cy );
			Members[1] = (float)( cp*sy );
			Members[2] = (float)( -sp );

			double srsp = sr*sp;
			double crsp = cr*sp;

			Members[4] = (float)( srsp*cy-cr*sy );
			Members[5] = (float)( srsp*sy+cr*cy );
			Members[6] = (float)( sr*cp );

			Members[8] = (float)( crsp*cy+sr*sy );
			Members[9] = (float)( crsp*sy-sr*cy );
			Members[10] = (float)( cr*cp );
		}

		void SetRotationDegrees( const Vector3 rotation )
		{
			SetRotationRadians( rotation * (float)(3.1415926535897932384626433832795 / 180.0) );
		}

		void SetScale( Vector3 scale )
		{
			Members[0] = scale.x;
			Members[5] = scale.y;
			Members[10] = scale.z;
		}
		
		void TransformVect( Vector3& vect)
		{
			float vector[3];

			vector[0] = vect.x*Members[0] + vect.y*Members[4] + vect.z*Members[8] + Members[12];
			vector[1] = vect.x*Members[1] + vect.y*Members[5] + vect.z*Members[9] + Members[13];
			vector[2] = vect.x*Members[2] + vect.y*Members[6] + vect.z*Members[10] + Members[14];

			vect.x = vector[0];
			vect.y = vector[1];
			vect.z = vector[2];
		}

		void TransformVect( const Vector3& in, Vector3& out)
		{
			out.x = in.x*Members[0] + in.y*Members[4] + in.z*Members[8] + Members[12];
			out.y = in.x*Members[1] + in.y*Members[5] + in.z*Members[9] + Members[13];
			out.z = in.x*Members[2] + in.y*Members[6] + in.z*Members[10] + Members[14];
		}

		void TranslateVect( Vector3& vect ) 
		{
			vect.x = vect.x+Members[12];
			vect.y = vect.y+Members[13];
			vect.z = vect.z+Members[14];
		}

		bool MakeInverse()
		{
			Matrix4 temp;

			if (GetInverse(temp))
			{
				*this = temp;
				return true;
			}

			return false;
		}

		bool GetInverse(Matrix4& out)
		{
			#define _m			getMInsecure
			#define _out		out.setMInsecure

			float d = (_m(0, 0) * _m(1, 1) - _m(1, 0) * _m(0, 1)) * (_m(2, 2) * _m(3, 3) - _m(3, 2) * _m(2, 3))	- (_m(0, 0) * _m(2, 1) - _m(2, 0) * _m(0, 1)) * (_m(1, 2) * _m(3, 3) - _m(3, 2) * _m(1, 3))
					+ (_m(0, 0) * _m(3, 1) - _m(3, 0) * _m(0, 1)) * (_m(1, 2) * _m(2, 3) - _m(2, 2) * _m(1, 3))	+ (_m(1, 0) * _m(2, 1) - _m(2, 0) * _m(1, 1)) * (_m(0, 2) * _m(3, 3) - _m(3, 2) * _m(0, 3))
					- (_m(1, 0) * _m(3, 1) - _m(3, 0) * _m(1, 1)) * (_m(0, 2) * _m(2, 3) - _m(2, 2) * _m(0, 3))	+ (_m(2, 0) * _m(3, 1) - _m(3, 0) * _m(2, 1)) * (_m(0, 2) * _m(1, 3) - _m(1, 2) * _m(0, 3));
			
			if (d == 0.f)
				return false;

			d = 1.f / d;

			_out(0, 0, d * (_m(1, 1) * (_m(2, 2) * _m(3, 3) - _m(3, 2) * _m(2, 3)) + _m(2, 1) * (_m(3, 2) * _m(1, 3) - _m(1, 2) * _m(3, 3)) + _m(3, 1) * (_m(1, 2) * _m(2, 3) - _m(2, 2) * _m(1, 3))));
			_out(1, 0, d * (_m(1, 2) * (_m(2, 0) * _m(3, 3) - _m(3, 0) * _m(2, 3)) + _m(2, 2) * (_m(3, 0) * _m(1, 3) - _m(1, 0) * _m(3, 3)) + _m(3, 2) * (_m(1, 0) * _m(2, 3) - _m(2, 0) * _m(1, 3))));
			_out(2, 0, d * (_m(1, 3) * (_m(2, 0) * _m(3, 1) - _m(3, 0) * _m(2, 1)) + _m(2, 3) * (_m(3, 0) * _m(1, 1) - _m(1, 0) * _m(3, 1)) + _m(3, 3) * (_m(1, 0) * _m(2, 1) - _m(2, 0) * _m(1, 1))));
			_out(3, 0, d * (_m(1, 0) * (_m(3, 1) * _m(2, 2) - _m(2, 1) * _m(3, 2)) + _m(2, 0) * (_m(1, 1) * _m(3, 2) - _m(3, 1) * _m(1, 2)) + _m(3, 0) * (_m(2, 1) * _m(1, 2) - _m(1, 1) * _m(2, 2))));
			_out(0, 1, d * (_m(2, 1) * (_m(0, 2) * _m(3, 3) - _m(3, 2) * _m(0, 3)) + _m(3, 1) * (_m(2, 2) * _m(0, 3) - _m(0, 2) * _m(2, 3)) + _m(0, 1) * (_m(3, 2) * _m(2, 3) - _m(2, 2) * _m(3, 3))));
			_out(1, 1, d * (_m(2, 2) * (_m(0, 0) * _m(3, 3) - _m(3, 0) * _m(0, 3)) + _m(3, 2) * (_m(2, 0) * _m(0, 3) - _m(0, 0) * _m(2, 3)) + _m(0, 2) * (_m(3, 0) * _m(2, 3) - _m(2, 0) * _m(3, 3))));
			_out(2, 1, d * (_m(2, 3) * (_m(0, 0) * _m(3, 1) - _m(3, 0) * _m(0, 1)) + _m(3, 3) * (_m(2, 0) * _m(0, 1) - _m(0, 0) * _m(2, 1)) + _m(0, 3) * (_m(3, 0) * _m(2, 1) - _m(2, 0) * _m(3, 1))));
			_out(3, 1, d * (_m(2, 0) * (_m(3, 1) * _m(0, 2) - _m(0, 1) * _m(3, 2)) + _m(3, 0) * (_m(0, 1) * _m(2, 2) - _m(2, 1) * _m(0, 2)) + _m(0, 0) * (_m(2, 1) * _m(3, 2) - _m(3, 1) * _m(2, 2))));
			_out(0, 2, d * (_m(3, 1) * (_m(0, 2) * _m(1, 3) - _m(1, 2) * _m(0, 3)) + _m(0, 1) * (_m(1, 2) * _m(3, 3) - _m(3, 2) * _m(1, 3)) + _m(1, 1) * (_m(3, 2) * _m(0, 3) - _m(0, 2) * _m(3, 3))));
			_out(1, 2, d * (_m(3, 2) * (_m(0, 0) * _m(1, 3) - _m(1, 0) * _m(0, 3)) + _m(0, 2) * (_m(1, 0) * _m(3, 3) - _m(3, 0) * _m(1, 3)) + _m(1, 2) * (_m(3, 0) * _m(0, 3) - _m(0, 0) * _m(3, 3))));
			_out(2, 2, d * (_m(3, 3) * (_m(0, 0) * _m(1, 1) - _m(1, 0) * _m(0, 1)) + _m(0, 3) * (_m(1, 0) * _m(3, 1) - _m(3, 0) * _m(1, 1)) + _m(1, 3) * (_m(3, 0) * _m(0, 1) - _m(0, 0) * _m(3, 1))));
			_out(3, 2, d * (_m(3, 0) * (_m(1, 1) * _m(0, 2) - _m(0, 1) * _m(1, 2)) + _m(0, 0) * (_m(3, 1) * _m(1, 2) - _m(1, 1) * _m(3, 2)) + _m(1, 0) * (_m(0, 1) * _m(3, 2) - _m(3, 1) * _m(0, 2))));
			_out(0, 3, d * (_m(0, 1) * (_m(2, 2) * _m(1, 3) - _m(1, 2) * _m(2, 3)) + _m(1, 1) * (_m(0, 2) * _m(2, 3) - _m(2, 2) * _m(0, 3)) + _m(2, 1) * (_m(1, 2) * _m(0, 3) - _m(0, 2) * _m(1, 3))));
			_out(1, 3, d * (_m(0, 2) * (_m(2, 0) * _m(1, 3) - _m(1, 0) * _m(2, 3)) + _m(1, 2) * (_m(0, 0) * _m(2, 3) - _m(2, 0) * _m(0, 3)) + _m(2, 2) * (_m(1, 0) * _m(0, 3) - _m(0, 0) * _m(1, 3))));
			_out(2, 3, d * (_m(0, 3) * (_m(2, 0) * _m(1, 1) - _m(1, 0) * _m(2, 1)) + _m(1, 3) * (_m(0, 0) * _m(2, 1) - _m(2, 0) * _m(0, 1)) + _m(2, 3) * (_m(1, 0) * _m(0, 1) - _m(0, 0) * _m(1, 1))));
			_out(3, 3, d * (_m(0, 0) * (_m(1, 1) * _m(2, 2) - _m(2, 1) * _m(1, 2)) + _m(1, 0) * (_m(2, 1) * _m(0, 2) - _m(0, 1) * _m(2, 2)) + _m(2, 0) * (_m(0, 1) * _m(1, 2) - _m(1, 1) * _m(0, 2))));

			return true;
		}

		void SetProjectionMatrixPerspectiveFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
		{
			float h = (float)(Math::Cos(fieldOfViewRadians/2) / Math::Sin(fieldOfViewRadians/2));
			float w = h / aspectRatio;

			setMInsecure(0,0,2*zNear/w);
			setMInsecure(1,0,0);
			setMInsecure(2,0,0);
			setMInsecure(3,0,0);

			setMInsecure(0,1,0);
			setMInsecure(1,1,2*zNear/h);
			setMInsecure(2,1,0);
			setMInsecure(3,1,0);

			setMInsecure(0,2,0);
			setMInsecure(1,2,0);
			setMInsecure(2,2,zFar/(zFar-zNear));
			setMInsecure(3,2,-1);

			setMInsecure(0,3,0);
			setMInsecure(1,3,0);
			setMInsecure(2,3,zNear*zFar/(zNear-zFar));
			setMInsecure(3,3,0);
		}

		void SetProjectionMatrixPerspectiveFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
		{
			float h = (float)(Math::Cos(fieldOfViewRadians/2) / Math::Sin(fieldOfViewRadians/2));
			float w = h / aspectRatio;

			setMInsecure(0,0,2*zNear/w);
			setMInsecure(1,0,0);
			setMInsecure(2,0,0);
			setMInsecure(3,0,0);

			setMInsecure(0,1,0);
			setMInsecure(1,1,2*zNear/h);
			setMInsecure(2,1,0);
			setMInsecure(3,1,0);

			setMInsecure(0,2,0);
			setMInsecure(1,2,0);
			setMInsecure(2,2,zFar/(zFar-zNear));
			setMInsecure(3,2,1);

			setMInsecure(0,3,0);
			setMInsecure(1,3,0);
			setMInsecure(2,3,zNear*zFar/(zNear-zFar));
			setMInsecure(3,3,0);
		}

		void SetProjectionMatrixPerspectiveRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
		{
			setMInsecure(0,0,2*zNear/widthOfViewVolume);
			setMInsecure(1,0,0);
			setMInsecure(2,0,0);
			setMInsecure(3,0,0);

			setMInsecure(0,1,0);
			setMInsecure(1,1,2*zNear/heightOfViewVolume);
			setMInsecure(2,1,0);
			setMInsecure(3,1,0);

			setMInsecure(0,2,0);
			setMInsecure(1,2,0);
			setMInsecure(2,2,zFar/(zNear-zFar));
			setMInsecure(3,2,-1);

			setMInsecure(0,3,0);
			setMInsecure(1,3,0);
			setMInsecure(2,3,zNear*zFar/(zNear-zFar));
			setMInsecure(3,3,0);
		}

		void SetProjectionMatrixPerspectiveLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
		{
			setMInsecure(0,0,2*zNear/widthOfViewVolume);
			setMInsecure(1,0,0);
			setMInsecure(2,0,0);
			setMInsecure(3,0,0);

			setMInsecure(0,1,0);
			setMInsecure(1,1,2*zNear/heightOfViewVolume);
			setMInsecure(2,1,0);
			setMInsecure(3,1,0);

			setMInsecure(0,2,0);
			setMInsecure(1,2,0);
			setMInsecure(2,2,zFar/(zNear-zFar));
			setMInsecure(3,2,1);

			setMInsecure(0,3,0);
			setMInsecure(1,3,0);
			setMInsecure(2,3,zNear*zFar/(zNear-zFar));
			setMInsecure(3,3,0);
		}

		void SetProjectionMatrixOrthoLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
		{
			setMInsecure(0,0,2/widthOfViewVolume);
			setMInsecure(1,0,0);
			setMInsecure(2,0,0);
			setMInsecure(3,0,0);

			setMInsecure(0,1,0);
			setMInsecure(1,1,2/heightOfViewVolume);
			setMInsecure(2,1,0);
			setMInsecure(3,1,0);

			setMInsecure(0,2,0);
			setMInsecure(1,2,0);
			setMInsecure(2,2,1/(zNear-zFar));
			setMInsecure(3,2,0);

			setMInsecure(0,3,0);
			setMInsecure(1,3,0);
			setMInsecure(2,3,zNear/(zNear-zFar));
			setMInsecure(3,3,1);
		}

		void SetProjectionMatrixOrthoRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
		{
			setMInsecure(0,0,2/widthOfViewVolume);
			setMInsecure(1,0,0);
			setMInsecure(2,0,0);
			setMInsecure(3,0,0);

			setMInsecure(0,1,0);
			setMInsecure(1,1,2/heightOfViewVolume);
			setMInsecure(2,1,0);
			setMInsecure(3,1,0);

			setMInsecure(0,2,0);
			setMInsecure(1,2,0);
			setMInsecure(2,2,1/(zNear-zFar));
			setMInsecure(3,2,0);

			setMInsecure(0,3,0);
			setMInsecure(1,3,0);
			setMInsecure(2,3,zNear/(zNear-zFar));
			setMInsecure(3,3,-1);
		}

		void SetLookAtMatrixLH(Vector3 position, Vector3 target, Vector3 upVector)
		{
			Vector3 zaxis = target - position;
			zaxis.Normalize();

			Vector3 xaxis = upVector.CrossProduct(zaxis);
			xaxis.Normalize();

			Vector3 yaxis = zaxis.CrossProduct(xaxis);

			setMInsecure(0,0,xaxis.x);
			setMInsecure(1,0,yaxis.x);
			setMInsecure(2,0,zaxis.x);
			setMInsecure(3,0,0);

			setMInsecure(0,1,xaxis.y);
			setMInsecure(1,1,yaxis.y);
			setMInsecure(2,1,zaxis.y);
			setMInsecure(3,1,0);

			setMInsecure(0,2,xaxis.z);
			setMInsecure(1,2,yaxis.z);
			setMInsecure(2,2,zaxis.z);
			setMInsecure(3,2,0);

			setMInsecure(0,3,-xaxis.DotProduct(position));
			setMInsecure(1,3,-yaxis.DotProduct(position));
			setMInsecure(2,3,-zaxis.DotProduct(position));
			setMInsecure(3,3,1.0f);
		}

		void SetLookAtMatrixRH(Vector3 position, Vector3 target, Vector3 upVector)
		{
			Vector3 zaxis = position - target;
			zaxis.Normalize();

			Vector3 xaxis = upVector.CrossProduct(zaxis);
			xaxis.Normalize();

			Vector3 yaxis = zaxis.CrossProduct(xaxis);

			setMInsecure(0,0,xaxis.x);
			setMInsecure(1,0,yaxis.x);
			setMInsecure(2,0,zaxis.x);
			setMInsecure(3,0,0);

			setMInsecure(0,1,xaxis.y);
			setMInsecure(1,1,yaxis.y);
			setMInsecure(2,1,zaxis.y);
			setMInsecure(3,1,0);

			setMInsecure(0,2,xaxis.z);
			setMInsecure(1,2,yaxis.z);
			setMInsecure(2,2,zaxis.z);
			setMInsecure(3,2,0);

			setMInsecure(0,3,-xaxis.DotProduct(position));
			setMInsecure(1,3,-yaxis.DotProduct(position));
			setMInsecure(2,3,-zaxis.DotProduct(position));
			setMInsecure(3,3,1.0f);			
		}

		void SetLocalMatrix(Vector3 position, Vector3 dir, Vector3 up)
		{
			Vector3 right = dir.CrossProduct(up);

			set_M(0,0, right.x);

			set_M(1,0, right.y);
			set_M(2,0, right.z);
			set_M(3,0, 0);

			set_M(0,1, dir.x);
			set_M(1,1, dir.y);
			set_M(2,1, dir.z);
			set_M(3,1, 0);

			set_M(0,2, up.x);
			set_M(1,2, up.y);
			set_M(2,2, up.z);
			set_M(3,2, 0);

			set_M(0,3, position.x);
			set_M(1,3, position.y);
			set_M(2,3, position.z);
			set_M(3,3, 1.0f);

		}
	private:

		inline float getMInsecure(int row, int col) 
		{
			return Members[col * 4 + row];
		}

		inline void setMInsecure(int row, int col, float v) 
		{
			Members[col * 4 + row] = v;
		}
	};
}
