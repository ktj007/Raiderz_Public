///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT Class
//
//	(c) 2003 IDV, Inc.
//
//	*** INTERACTIVE DATA VISUALIZATION (IDV) PROPRIETARY INFORMATION ***
//
//	This software is supplied under the terms of a license agreement or
//	nondisclosure agreement with Interactive Data Visualization and may
//	not be copied or disclosed except in accordance with the terms of
//	that agreement.
//
//      Copyright (c) 2001-2003 IDV, Inc.
//      All Rights Reserved.
//
//		IDV, Inc.
//		1233 Washington St. Suite 610
//		Columbia, SC 29201
//		Voice: (803) 799-1699
//		Fax:   (803) 931-0320
//		Web:   http://www.idvinc.com

#pragma once


#include <vector>
#include <list>
#include "SpeedGrassConstants.h"

struct SGrassVertex
{
	float Position[3];
	float Normal[3];
	float Color[4];
	float Tex0[2];
	float Tex1[4];
};

class RS_API CSpeedGrassRT
{
public:
					CSpeedGrassRT(void);
		virtual 	~CSpeedGrassRT(void);

		/////////////////////////////////////////////////////////////////////////
		// constant table
		static float NOISE;
		static float THROW;

        ///////////////////////////////////////////////////////////////////////  
        //  struct SBlade

		struct RS_API SBlade
		{
			SBlade();

			// geometry
			float				m_afPos[3];
			float				m_fSize;

			float				m_fWSize;
			float				m_fHSize;

			int					m_nWhichTexture;	// 풀 그릴 시 하나의 텍스쳐만 셋팅 하고, 그 텍스쳐의 일부 좌표를 사용 한다. - COMMENT OZ

			// wind
			float				m_fNoise;
			float				m_fThrow;

			// color
			DWORD				m_dwBottomColor;
			DWORD				m_dwTopColor;

			float				m_afNorm[3];
		};

#pragma pack(push, PACKED_BLADE, 1)
		// version 에 따른 구조체 변경
		struct SPackedBlade3
		{
			float				m_afPos[3];
			float				m_fWSize;
			float				m_fHSize;
			int					m_nWhichTexture;

			float				m_fNoise;
			float				m_fThrow;

			void				Pack(const SBlade& b);
			void				Unpack(SBlade& b);
		};

		struct SPackedBlade4
		{
			SPackedBlade3		m_packedBlade3;
			
			float				m_fSize;
			float				m_afBottomColor[3];
			float				m_afTopColor[3];

			void				Pack(const SBlade& b);
			void				Unpack(SBlade& b);
		};

		/**
		 Version 5 -

		 Normal을 찾아봤는데, 어디서 사용되는지 알 수 없다.
		 WSize, HSize는 RSpeedGrass.cpp 의 GetGrassVertex 에서 vertex-buffer 로 할당할 때 계산되므로 Load/Save 에 필요 없어서 삭제
		 Throw는 랜덤 값이지만, 툴에서 찍을 때는 1.0f의 상수를 할당하고
		 Noise는 항상 15.f 의 값이 들어가도록 되어 있다.
		 따라서, 두개는 CSpeedGrassRT 클래스의 static 값으로 두었고,
		 bottom/topcolor는 DWORD 로 변경

		 풀 하나 당 32byte
		*/
		struct SPackedBlade5
		{
			//static DWORD NormalToPackedDword(float x, float y, float z);
			//static MVector3 PackedDwordToNormal(DWORD dwNormal);

			float	m_afPos[3];
			float	m_fSize;
			int		m_nWhichTexture;
			DWORD	m_dwBottomColor;
			DWORD	m_dwTopColor;

			void	Pack(const SBlade& b);
			void	Unpack(SBlade &b);
		};

		/**
		 Version 6 - Ver5에서 normal을 추가
		 풀 하나 당 44byte
		*/
		struct SPackedBlade6
		{
			SPackedBlade5		m_packedBlade5;
			float	m_afNorm[3];

			void	Pack(const SBlade& b);
			void	Unpack(SBlade &b);
		};

#pragma pack(pop, PACKED_BLADE)

        ///////////////////////////////////////////////////////////////////////  
        //  struct SRegion

		struct SRegion
		{
			SRegion(void);
			SRegion(const SRegion& rhs);
			~SRegion(void);

			// dimensions
			float						m_afCenter[3];
			float						m_afMin[3];
			float						m_afMax[3];	

			// culling
			bool						m_bCulled;
			float						m_fCullingRadius;

			// grass/brush blades
			std::vector<SBlade>			m_vBlades;
		};

		SRegion*			GetRegions(unsigned int& uiNumRegions);
		void				DeleteRegions(void);
		//bool				CustomPlacement(unsigned int uiRows, unsigned int uiCols);

        ///////////////////////////////////////////////////////////////////////  
        //  Geometry

static	const float*		GetUnitBillboard(void)											{ return m_afUnitBillboard; }


        ///////////////////////////////////////////////////////////////////////  
        //  LOD

static  void		        GetLodParams(float& fFarDistance, float& fTransitionLength);
static  void		        SetLodParams(float fFarDistance, float fTransitionLength);


        ///////////////////////////////////////////////////////////////////////  
        //  Culling

		bool				AllRegionsAreCulled(void) const									{ return m_bAllRegionsCulled; }
		void				Cull(void);


        ///////////////////////////////////////////////////////////////////////  
        //  Wind

static	void				SetWindDirection(const float* pWindDir);
static	const float*		GetWindDirection(void);


        ///////////////////////////////////////////////////////////////////////  
        //  Camera

static  const float*		GetCameraPos(void);
static  void                SetCamera(const float* pPosition, const float* pModelviewMatrix);
static	void				SetPerspective(float fAspectRatio, float fFieldOfView);


        ///////////////////////////////////////////////////////////////////////  
        //  Terrain hugging

virtual float				Color(float /*fX*/, float /*fY*/, const float* /*pNormal*/, float* /*pTopColor*/, float* /*pBottomColor*/) { return 0.0f; }	
virtual float				Height(float /*fX*/, float /*fY*/, float* /*pNormal*/) const { return 0.0f; }	

protected:

		//void				CreateRegions(const std::vector<SBlade>& vSceneBlades, float fCollisionDistance = 0.0f);
static	void				ComputeFrustum(void);
static	void				ComputeUnitBillboard(void);
		void				ConvertCoordsToCell(const float* pCoords, int* pGridCoords) const;
		unsigned int		GetRegionIndex(unsigned int uiRow, unsigned int uiCol) const	{ return uiRow * m_nNumRegionCols + uiCol; }
static	bool				OutsideFrustum(SRegion* pRegion);

protected:	

		// general
static	float					m_fLodFarDistance;
static	float					m_fLodTransitionLength;
static	float					m_afUnitBillboard[16];
static	float					m_afWindDir[4];

		// regions
		int						m_nNumRegionCols;
		int						m_nNumRegionRows;
		std::vector<SRegion>	m_vRegions;

		// camera
static	float					m_afCameraOut[3];
static	float					m_afCameraRight[3];
static	float					m_afCameraUp[3];
static	float					m_afCameraPos[3];
static	float					m_fFieldOfView;
static	float					m_fAspectRatio;

		// culling
static	float					m_afFrustumBox[6];
static	float					m_afFrustumMin[2];
static	float					m_afFrustumMax[2];
static	float					m_afFrustumPlanes[5][4];
		float					m_afBoundingBox[6];		// Cell Bounding box
		bool					m_bAllRegionsCulled;
};
