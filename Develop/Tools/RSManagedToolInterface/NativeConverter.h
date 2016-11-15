#pragma once

#pragma unmanaged
#include "MVector3.h"
#include "MMatrix.h"
#include "RTypes.h"
#pragma managed

#include "Vector3.h"
#include "Matrix4.h"
#include "MMath.h"

namespace RSManaged
{
	class NativeConverter
	{
	public:
		inline static RSManaged::Vector3 getNETVector(MVector3 v)
		{
			return RSManaged::Vector3(v.x, v.y, v.z);
		}
		inline static MVector3 getNativeVector(RSManaged::Vector3 v)
		{
			return MVector3(v.x, v.y, v.z);
		}

		inline static RSManaged::Matrix4 getNETMatrix(MMatrix mat)
		{
			Matrix4 nmat;
			for (int i=0; i<16; ++i)
			{
				nmat.Members[i] = mat.m[i];
			}

			return nmat;
			
		}
		inline static MMatrix getNativeMatrix(RSManaged::Matrix4 mat)
		{
			MMatrix nmat;
			for (int i=0; i<16; ++i)
				nmat.m[i] = mat.Members[i];

			return nmat;
			
		}

		inline static RSManaged::RSMPolarCoord getNETPolarCoord(RPolarCoord pc)
		{
			RSManaged::RSMPolarCoord _pc;
			_pc.fPhi = pc.fPhi;
			_pc.fTheta = pc.fTheta;
			_pc.fRadius = pc.fRadius;
			return _pc;
		}
		inline static RPolarCoord getNativePolarCoord(RSManaged::RSMPolarCoord pc)
		{
			RPolarCoord _pc;
			_pc.fPhi = pc.fPhi;
			_pc.fTheta = pc.fTheta;
			_pc.fRadius = pc.fRadius;
			return _pc;
		}

	};

}