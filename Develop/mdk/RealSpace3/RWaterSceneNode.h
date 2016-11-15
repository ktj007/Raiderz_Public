#pragma once

#include "RSceneNode.h"
#include "RTreeMeshBuffer.h"

namespace rs3
{

RS_API extern const char *RSID_WATERPLANE;

/**
 반사가 되는 물 
*/
class RS_API RWaterPlane : public RSceneNode
{
public:
	struct SWaterPlaneProperty
	{
		SWaterPlaneProperty() : 
			m_fDepthLimit(1500.0f),
			m_fBumpUVScale1(2000.0f),
			m_fBumpUVScale2(3500.0f),
			m_fBumpUVScale3(5000.0f),
			m_fBumpUVScale4(7000.0f),
			m_vBumpMove1(RVector2(0.012f, -0.010f)),
			m_vBumpMove2(RVector2(-0.007f, 0.008f)),
			m_vBumpMove3(RVector2(0.004f, -0.004f)),
			m_vBumpMove4(RVector2(-0.004f, 0.003f)),
			m_fBumpAdjustFactor(0.0f),
			m_fBumpWeight1(0.55f),
			m_fBumpWeight2(0.35f),
			m_fBumpWeight3(0.25f),
			m_fBumpWeight4(0.15f),
			m_vWaterRGBHalflife(RVector3(2400, 4800, 12000)),
			m_fFresnelConstant(0.97f),
			m_fReflectAdjust(1.0f),
			m_fBumpRotate(0.25),
			m_strBumpTex1("WaterBumps1.tga"),
			m_strBumpTex2("WaterBumps2.tga"),
			m_bUseCustomFog(false),
			m_fFogNear(0),
			m_fFogFar(0),
			m_vFogColor(RVector(0, 0, 0)),
			m_vDiffuseColor(RVector(1, 1, 1)),
			m_fBlending(0),
			m_fRefractAttenuationDist(1),
			m_fRefractAttenuationFactor(1),
			m_strShoreTexture(""),
			m_fShoreIntensity(0),
			m_vShoreUVScale(RVector2(1,1)),

			
			// vertex wave Property
			m_fScale(250.0f),
			m_fAmplitude1(0.0f),
			m_fPhase1(1.0f),
			
			//underwater property
			m_fUnderWaterFogNear(400),
			m_fUnderWaterFogFar(400),
			m_fUnderWaterBumpUVScale(10),
			m_fUnderWaterDepth(10000.0f)
		{}

		float			m_fDepthLimit;
		float			m_fBumpUVScale1;
		float			m_fBumpUVScale2;
		float			m_fBumpUVScale3;
		float			m_fBumpUVScale4;

		RVector2		m_vBumpMove1;
		RVector2		m_vBumpMove2;
		RVector2		m_vBumpMove3;
		RVector2		m_vBumpMove4;

		float			m_fBumpAdjustFactor;
		float			m_fBumpWeight1;
		float			m_fBumpWeight2;
		float			m_fBumpWeight3;
		float			m_fBumpWeight4;

		RVector			m_vWaterRGBHalflife;
		float			m_fFresnelConstant;

		float			m_fReflectAdjust;

		float			m_fBumpRotate;

		std::string		m_strBumpTex1;
		std::string		m_strBumpTex2;
		
		bool			m_bUseCustomFog;
		float			m_fFogNear;
		float			m_fFogFar;
		RVector			m_vFogColor;
		RVector			m_vDiffuseColor;
		float			m_fBlending;
		
		float			m_fRefractAttenuationDist;
		float			m_fRefractAttenuationFactor;

		// vertex wave Property
		float			m_fScale;
		float			m_fAmplitude1;
		float			m_fPhase1;

		// Underwater Property
		float			m_fUnderWaterFogNear;
		float			m_fUnderWaterFogFar;
		float			m_fUnderWaterBumpUVScale;
		float			m_fUnderWaterDepth;

		// Shore Property
		std::string		m_strShoreTexture;
		float			m_fShoreIntensity;
		RVector2		m_vShoreUVScale;
	};
	SWaterPlaneProperty* GetWaterPlaneProperty(){ return m_pWaterPlaneProperty; }

public:
	RWaterPlane();
	virtual ~RWaterPlane();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// SceneNode declaration
	//
	MDeclareRTTI;
	DECLARE_NODE_ID_STRING(RSID_WATERPLANE);
//	DECLARE_USING_PASS(RRP_WATER)

	virtual bool				CreateFromXML(MXmlElement &element, bool bBackgroundCreation);
	virtual void				SaveToXML(MXmlElement &element);

	bool						Build( const char* _pSzFileName, MXmlElement* pElementForProperty = NULL, bool bBackgroundCreation = false);
	bool						SetWaterPlaneInfoFromXML(MXmlElement& element);
	bool						PickPlane(RPICKINFO& pickInfo);
	
	void						LoadBumpMap();
	void						DestroyBumpMap();
	
	void						LoadShoreTexture();
	void						DestroyShoreTexture();
	
	float						GetWaterLevel() const { return m_fWaterLevel; }
	RTexture*					GetBumpTexture1() { return m_pBumpTexture1; }
	RTexture*					GetBumpTexture2() { return m_pBumpTexture2; }
	RTexture*					GetShoreTexture() { return m_ShoreTexture; }

	bool						IsUnderWater(RVector3 vCameraPosition);
	bool						IsApplyUnderWaterEffect(RVector3 vCameraPosition);

	void						Render();

	// 충돌 메시 데이터 수집
	virtual void				CollectStaticCollisionPolygons(RSceneNodePolygonCollector* pCollector) const override;

protected:
	virtual int					CalcPriority();

private:
	void						SetUsableAndUpdate_();
	void						Clear();

	struct SWaterVertex
	{
		RVector     m_vPosition;
		float		m_fDepth;
	};
	float							m_fWaterLevel;
	RTreeMeshBuffer<SWaterVertex>*	m_pWaterMesh;

	SWaterPlaneProperty*			m_pWaterPlaneProperty;

	RTexture*						m_pBumpTexture1;
	RTexture*						m_pBumpTexture2;
	RTexture*						m_ShoreTexture;
};

}