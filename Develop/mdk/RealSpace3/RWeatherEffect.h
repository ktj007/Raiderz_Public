#pragma once

#include "RPCollection.h"
#include "RVType.h"
#include "RDevice.h"
#include "RSceneNode.h"

namespace rs3 {

class RCameraSceneNode;
class RDevice;
class RDirectionalLightSceneNode;
class RSceneSky;
class REffectSceneNode;
class RCollisionTree;

class RS_API RWeatherEffect : public RSceneNode
{
#define DEFAULT_RANGE_Z		3000
#define PICKINGRAY_DISTANCE 100000

//	DECLARE_USING_PASS(RRP_WEATHEREFFECT);
private:
	REffectSceneNode* m_pWaterEffect;
	RVector m_vCameraPosition;

	struct RWParticle
	{
		RVector3 vPos;
		RVector3 vDir;
		float fVelocity;
		float fRand;
		float fRangeZ;	// 없어질 지점
	};

	CParticleArray<RWParticle> m_poolParticles;
	RTexture* m_hTexture;
	
	float m_fRadius;						// fixed
	float m_fRangeX, m_fRangeY, m_fRangeZ;	// fixed
	float m_fBaseTime;						// inner
	float m_fMass;							// fixed;

	RVector m_vWind;						// input value

	void UpdateParticle(const RVector &vOffset, float fTime, RCollisionTree* pCollisionTree);

	typedef RVt_pos_clr_tex1 PVertexType;
	HVERTEXBUFFER m_hVertexBuffer;

	int m_nVertexNum;

	RPRIMITIVETYPE m_ePrimType;
	int m_nVertexNumPerPrim;

	RSceneSky *m_pScreenMeshNode;
	void LoadTexture(const char *filename);
	void LoadMesh(const char *filename);
	void DeleteMesh();

	// 비
	int AddVertexBufferRain(PVertexType *pVertex, int nIndex, const RWParticle *particle);
	// 눈
	int AddVertexBufferSnow(PVertexType *pVertex, int nIndex, const RWParticle *particle);

	// 그냥 Function Pointer 씁니다. 더 확장되면, 구조변경 할께요 -
	int (RWeatherEffect::*ptAddVertexBuffer)(PVertexType *pVertex, int nIndex, const RWParticle *particle);

	bool m_bFill;
	static const int BILLBOARD_VERTEX_NUM = 2;
public:
	enum WEATHER_TYPE
	{
		WEATHER_NONE = 0,
		WEATHER_RAIN = 1,
		WEATHER_SNOW = 2
	};

	struct WeatherParameter
	{
		WEATHER_TYPE eWeatherType;

		float fSize;
		float fVelocity;
		int nCreateNum;

		RVector4 v1stColor;
		RVector4 v2ndColor;
		float fValue;
		float f1stLuminosity;
		float f2ndLuminosity;
		float fWindForce;

		std::string strMeshFilename;
	};

private:
	WeatherParameter m_currentParameter;

	void Render( RCameraSceneNode* pCurrentCamera);
public:
	static const int VERTEX_SIZE = 20000;

	RWeatherEffect();
	virtual ~RWeatherEffect();

	void SetScale(float s);
	void SetVelocity(float v);
	void SetParticleCount(int n);

	void SetFirstColor(const RVector4 & vColor4);
	void SetSecondColor(const RVector4 & vColor4);
	void SetWeatherType(WEATHER_TYPE eType);
	bool Create();

	void SetWindForce(float f);
	void SetValue(float f);
	void SetFill(bool b);

	void Update(float fTime, RCollisionTree* pCollisionTree);
	float PickToCollisionTree(RVector vPos, RVector vDir, RCollisionTree* pCollisionTree);

	virtual void OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod) override;

	virtual bool CreateFromXML(MXmlElement &element, bool bBackgroundCreation);
	virtual void SaveToXML(MXmlElement &element);

	virtual void OnAdd();
	virtual void OnRemove();

	const WeatherParameter& GetParameter()
	{
		return m_currentParameter;
	}
};

};