#include "stdafx.h"
#include "RClothCollision.h"


namespace rs3
{
	bool RClothCollision::PlaneConstraint( rs3::RVector &v,const rs3::RVector &n,float d )
	{
		float singed_distance_to_plane = v.x * n.x + v.y * n.y + v.z * n.z - d; //원래소스 float singed_distance_to_plane = DotProduct(v,n) - d;
		if(singed_distance_to_plane <0.0f)
		{
			v-= singed_distance_to_plane*n;
			return true;
		}
		return false;
	}

	bool RClothCollision::SphereConstraint( const rs3::RVector &v0,rs3::RVector& v1,rs3::RVector& v2,const rs3::RVector& center,float r )
	{
		RVector n = v0 - center;	n.Normalize();
		float d= center.x * n.x + center.y * n.y + center.z * n.z + r; //원래소스 float d = DotProduct(center,n) + r;

		return PlaneConstraint(v1,n,d) || PlaneConstraint(v2,n,d);
	}

	inline void CopyArr(float *dest, const float *src)
	{
		dest[0] = src[0];
		dest[1] = src[1];
		dest[2] = src[2];
	}

	/*__forceinline*/ void RClothCollision::CapsuleConstraint( const rs3::RVector &v0,rs3::RVector& v1,rs3::RVector& v2,const rs3::RVector &endPoint, const rs3::RVector &n,float len,float r )
	{
		
		float fv0[4] = {0, };
		float fv1[4] = {0, };
		float fv2[4] = {0, };
		float fendPoint[4] = {0, };
		float fn[4] = {0, };

		CopyArr(fv0, v0.v);
		CopyArr(fv1, v1.v);
		CopyArr(fv2, v2.v);
		CopyArr(fendPoint, endPoint.v);
		CopyArr(fn, n.v);
		
		__asm
		{
			movups xmm0, fv0;
			xorps xmm4, xmm4;
			movups xmm5, xmm0;
			movups xmm1, fendPoint;
			movups xmm2, fn;

			//offset = v0 - endPoint
			subps xmm5, xmm1;

			//height = offset . n;
			mulps xmm5, xmm2;
			movaps xmm7, xmm5;
			movaps xmm6, xmm5;
			shufps xmm7, xmm7, 0xD2;
			shufps xmm6, xmm6, 0xC9;
			addps xmm6, xmm7;
			addps xmm5, xmm6;

			// Clamp(height,0,length);
			movss xmm7, len;
			shufps xmm7, xmm7, 0x00;
			maxps xmm5, xmm4;
			minps xmm5, xmm7;

			// center = endPoint + height * n
			// center2v0 = v0 - center
			mulps xmm5, xmm2;
			addps xmm5, xmm1;
			subps xmm0, xmm5;

			// center2v0.normalize()
			movaps xmm3, xmm0;
			mulps xmm3, xmm3;
			movaps xmm7, xmm3;
			movaps xmm6, xmm3;
			shufps xmm7, xmm7, 0xD2;
			shufps xmm6, xmm6, 0xC9;
			addps xmm3, xmm7;
			addps xmm3, xmm6;
			cmpneqps xmm4, xmm3;
			rsqrtps xmm3, xmm3;
			andps xmm3, xmm4;
			mulps xmm0, xmm3;

			// float d = center . center2v0 + r;
			mulps xmm5, xmm0;
			movss xmm2, r;
			shufps xmm2, xmm2, 0x00;
			movaps xmm7, xmm5;
			movaps xmm6, xmm5;
			shufps xmm7, xmm7, 0xD2;
			shufps xmm6, xmm6, 0xC9;
			addps xmm5, xmm7;
			addps xmm5, xmm6;
			addps xmm2, xmm5;

			xorps xmm4, xmm4;

			// float signed_distance_to_plane1 = v1 . center2v0 - d;
			// v1 -= signed_distance_to_plane1 < 0 ? signed_distance_to_plane1 * center2v0 : ZERO
			movups xmm3, fv1;
			movaps xmm1, xmm3;
			mulps xmm1, xmm0;
			movaps xmm7, xmm1;
			movaps xmm6, xmm1;
			shufps xmm7, xmm7, 0xD2;
			shufps xmm6, xmm6, 0xC9;
			addps xmm1, xmm7;
			addps xmm1, xmm6;
			subps xmm1, xmm2;
			movaps xmm6, xmm1;
			cmpltps xmm6, xmm4;
			andps xmm6, xmm0;
			mulps xmm6, xmm1;
			subps xmm3, xmm6;
			movups fv1, xmm3;

			movups xmm3, fv2;
			movaps xmm1, xmm3;
			mulps xmm1, xmm0;
			movaps xmm7, xmm1;
			movaps xmm6, xmm1;
			shufps xmm7, xmm7, 0xD2;
			shufps xmm6, xmm6, 0xC9;
			addps xmm1, xmm7;
			addps xmm1, xmm6;
			subps xmm1, xmm2;
			movaps xmm6, xmm1;
			cmpltps xmm6, xmm4;
			andps xmm6, xmm0;
			mulps xmm6, xmm1;
			subps xmm3, xmm6;
			movups fv2, xmm3;
		}

		CopyArr(v1.v, fv1);
		CopyArr(v2.v, fv2);
		
/*

		//--[ capsule ]
		float offsetx = v0.x - endPoint.x; 
		float offsety = v0.y - endPoint.y; 
		float offsetz = v0.z - endPoint.z; 

		//원래소스 float height= offset.x * n.x + offset.y * n.y + offset.z * n.z;									
		float height = offsetx * n.x;
		height+= offsety * n.y;
		height+= offsetz * n.z;

		Clamp(height,0,len);

		//원래소스 
		float centerx = endPoint.x + height*n.x;
		float centery = endPoint.y + height*n.y;
		float centerz = endPoint.z + height*n.z;

		//--[ sphere ]
		RVector center2v0;
		center2v0.x = v0.x - centerx;
		center2v0.y = v0.y - centery;
		center2v0.z = v0.z - centerz;

		center2v0.UnsafeFastNormalize();
//		center2v0.Normalize();

		float d= centerx * center2v0.x + centery * center2v0.y + centerz * center2v0.z + r;

		//--[ plane_v1 ]
		float singed_distance_to_plane1 = v1.x * center2v0.x + v1.y * center2v0.y + v1.z * center2v0.z - d;
		if(singed_distance_to_plane1 <0.0f)
		{
			//원래소스 v1-= singed_distance_to_plane1*center2v0;
			v1.x-= singed_distance_to_plane1 * center2v0.x;
			v1.y-= singed_distance_to_plane1 * center2v0.y;
			v1.z-= singed_distance_to_plane1 * center2v0.z;
		}

		//--[ plane_v2 ]
		float singed_distance_to_plane2 = v2.x * center2v0.x + v2.y * center2v0.y + v2.z * center2v0.z - d;
		if(singed_distance_to_plane2 <0.0f)
		{
			//원래소스 v2-= singed_distance_to_plane2*center2v0;
			v2.x-= singed_distance_to_plane2*center2v0.x;
			v2.y-= singed_distance_to_plane2*center2v0.y;
			v2.z-= singed_distance_to_plane2*center2v0.z;
		}*/
	}

	bool RClothCollision::SphereConstraint( const rs3::RVector &v0,rs3::RVector& v1,const rs3::RVector& center,float r )
	{
		RVector n = v0 - center;	n.Normalize();
		float d= center.x * n.x + center.y * n.y + center.z * n.z + r; //원래소스 float d = DotProduct(center,n) + r;
		return PlaneConstraint(v1,n,d);
	}


	void RClothCollision::CapsuleConstraint( const rs3::RVector &v0,rs3::RVector& v1,const rs3::RVector &endPoint, const rs3::RVector &n,float length,float r )
	{
		RVector offset(v0.x - endPoint.x , v0.y - endPoint.y , v0.z - endPoint.z );						//원래소스 RVector offset = v0 - endPoint;
		float height= offset.x * n.x + offset.y * n.y + offset.z * n.z;									//원래소스 float d = DotProduct(offset,n);
		Clamp(height,0,length);
		RVector center( endPoint.x + height*n.x , endPoint.y + height*n.y , endPoint.z + height*n.z);	//원래소스 : RVector center = endPoint + d*n;

		SphereConstraint(v0,v1,center,r);
	}
	
}