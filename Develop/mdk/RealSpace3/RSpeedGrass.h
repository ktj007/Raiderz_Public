#pragma once


#include <string>
#include "Dependencies/speedgrass/SpeedGrassRT.h"
#include "RDevice.h"
#include "RSceneManager.h"


#include "RTreeMeshBuffer.h"


#include "MXml.h"


namespace rs3
{

class RShaderFX;
class RSceneManager;
class RDevice;
class RTerrain;
enum RRENDERINGMETHOD;

typedef CSpeedGrassRT::SBlade RGrassBlade;

class RGrassSystem;

class RSpeedGrass : public CSpeedGrassRT
{
	friend RGrassSystem;
protected:
	// Region Data
	struct SRegionData
	{
		HVERTEXBUFFER hVertexBuffer;
		HINDEXBUFFER hIndexBuffer;

		UINT nVertexNum;
		UINT nPrimitiveNum;

		UINT nVertexBufferSize;
		UINT nIndexBufferSize;

		SRegionData()
			: hVertexBuffer(R_NONE)
			, hIndexBuffer(R_NONE)
			, nVertexBufferSize(0)
			, nIndexBufferSize(0)
			, nVertexNum(0)
			, nPrimitiveNum(0)
		{}
	};

	vector<SRegionData>		m_vRegionDatas;
	
	// Grass Composite-map Info
	int						m_nCompTexWidth;
	int						m_nCompTexHeight;

	struct SCompositeMapInfo
	{
		string strFilename;
		float fUOffset;
		float fVOffset;
		float fWidth;
		float fHeight;

		SCompositeMapInfo()
			: strFilename("")
			, fWidth(0.0f)
			, fHeight(0.0f)
			, fUOffset(0.f)
			, fVOffset(0.f)
		{
		}
	};

public:
	const static UINT GRASS_MAX_NUM = 12;
protected:
	SCompositeMapInfo*	m_pGrassTable[GRASS_MAX_NUM];

	RDevice*				m_pDevice;
	HVERTEXFORMAT			m_hGrassVertex;

	//바람
	RVector					m_vWindDirection;
	float					m_fWindStrength;

	//풀 렌더링 옵션
	float		m_fGrassJigglingPeriod;		// 떨림주기
	float		m_fGrassJigglingStrength;	// 떨림쎄기
	float		m_fGrassIncliningStrength;	// 기울여짐쎄기

	void		SetGrassJigglingPeriod(float f)		{	m_fGrassJigglingPeriod		= f;	}
	void		SetGrassJigglingStrength(float f)	{	m_fGrassJigglingStrength	= f;	}
	void		SetGrassIncliningStrength(float f)	{	m_fGrassIncliningStrength	= f;	}

	void					GetGrassVertex(SBlade& b, vector<SGrassVertex>& vert, vector<WORD>& idx, int idxoff, RTerrain* pTerrain);
	float					GetDistSq(float x1, float y1, float x2, float y2);

	RVector4				m_vAmbientColor;
	RVector4				m_vDiffuseColor;
	
	int						m_nGrassCountLimitPerRegion;	// 리전 당 풀의 갯수 제한 - 090610, OZ
public:
	RSpeedGrass(RDevice* dev);
	virtual ~RSpeedGrass();

	virtual bool Create() = 0;
	virtual void Destroy();

	virtual void SetBasicRenderState( RGrassSystem *pGrassSystem, RCameraSceneNode* pCamera) = 0;
	virtual int Render( RShaderFX* pShader, RTechnique* pTechnique)  = 0;
	virtual int RenderTillingEffect() = 0;
	virtual int RenderGStage( RRENDERINGMETHOD methd) = 0;
	virtual int RenderMStage() = 0;
	virtual void RenderRaw()  = 0;
	virtual void ResetRenderState() = 0;

	virtual void UpdateDXBuffers( RTerrain* pTerrain) = 0;

	void CreateRegions(int row, int col, float fWidth, float fHeight);
	bool AddGrass(const SBlade& b);						// 결과를 리턴하게 변경 - 090610, OZ
	void DeleteGrass(float x, float y, float fRadius);

	void SetWindDirection(const RVector& windDir);
	void SetWindStrength(float windStr);				// 0(바람없음)~1(최대)

public:
	int GetGrassID(const char *szTextureFilename);
	int GetGrassWidth(int nIndex);
	int GetGrassHeight(int nIndex);

	int GrassTypeCount();
	
	void UpdateGrass();
	void LoadGrass(MXmlElement &xmlElement, const char *szNameA, const char *szNameB);

	void		SetAmbientColor(RVector4 &vColor);
	RVector4	GetAmbientColor();

	void		SetDiffuseColor(RVector4 &vColor);
	RVector4	GetDiffuseColor();

	static void SetLODParams(float fFarDistance, float fTransitionLength)
	{
		m_fLodFarDistance = fFarDistance;
		m_fLodTransitionLength = fTransitionLength;
	}
};

};