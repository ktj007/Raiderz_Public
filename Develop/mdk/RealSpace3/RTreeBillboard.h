#pragma once

#include "RTypes.h"
#include "d3d9types.h"

namespace rs3 {

struct SFVFTreeBillboardVertex_Simple
{
		rs3::RVector	m_vPosition;            // Always Used                          
		float           m_fTexCoords[2];        // Always Used
};
static const DWORD SPEEDTREE_BILLBOARD_SIMPLE = D3DFVF_XYZ | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEX1;

struct SSpeedTreeBillboard
{
	SSpeedTreeBillboard( ) { }
	~SSpeedTreeBillboard( ) { }

	struct SVertex
	{
		SVertex()
		{
			for( int i = 0 ; i < 4 ; ++i )
			{
				m_afPos[i]			= 0.f;
				m_afTexCoord0[i]	= 0.f;
				m_afMiscParams[i]	= 0.f;
			}

			for( int i = 0 ; i < 3 ; ++i )
				m_afLightAdjusts[i] = 0.f;
		}

		~SVertex()
		{

		}

		float           m_afPos[4];             // xyz = tree pos, w = cluster corner
		float           m_afTexCoord0[4];       // x = width, y = height, z = azimuth, w = fade value 
		float           m_afMiscParams[4];      // vert: x = tree scalar, y = texcoord offset, z = num images, z = transition %
		float           m_afLightAdjusts[3];    // x = bright side adjustment, y = dark side adjustment, z = ambient scalar
	};

	static  int Size(void)
	{
		return 6 * sizeof(SVertex);
	}

	inline void UpdateTree(const float* pPos, float fAzimuth, float fFade, float fScale)
	{
		const float c_fScale = 0.0f;

		m_sVert0.m_afPos[0] = m_sVert1.m_afPos[0] = m_sVert2.m_afPos[0] = m_sVert3.m_afPos[0] = m_sVert4.m_afPos[0] = m_sVert5.m_afPos[0] = pPos[0];
		m_sVert0.m_afPos[1] = m_sVert1.m_afPos[1] = m_sVert2.m_afPos[1] = m_sVert3.m_afPos[1] = m_sVert4.m_afPos[1] = m_sVert5.m_afPos[1] = pPos[1];
		m_sVert0.m_afPos[2] = m_sVert1.m_afPos[2] = m_sVert2.m_afPos[2] = m_sVert3.m_afPos[2] = m_sVert4.m_afPos[2] = m_sVert5.m_afPos[2] = pPos[2];

		m_sVert0.m_afTexCoord0[2] = m_sVert1.m_afTexCoord0[2] = m_sVert2.m_afTexCoord0[2] = 
		m_sVert3.m_afTexCoord0[2] = m_sVert4.m_afTexCoord0[2] = m_sVert5.m_afTexCoord0[2] = fAzimuth;

		m_sVert0.m_afTexCoord0[3] = m_sVert1.m_afTexCoord0[3] = m_sVert2.m_afTexCoord0[3] = 
		m_sVert3.m_afTexCoord0[3] = m_sVert4.m_afTexCoord0[3] = m_sVert5.m_afTexCoord0[3] = fFade;

		m_sVert0.m_afMiscParams[0] = m_sVert1.m_afMiscParams[0] = m_sVert2.m_afMiscParams[0] = 
		m_sVert3.m_afMiscParams[0] = m_sVert4.m_afMiscParams[0] = m_sVert5.m_afMiscParams[0] = fScale;
	}

	SVertex         m_sVert0;
	SVertex         m_sVert1;
	SVertex         m_sVert2;
	SVertex         m_sVert3;
	SVertex         m_sVert4;
	SVertex         m_sVert5;
};

}
