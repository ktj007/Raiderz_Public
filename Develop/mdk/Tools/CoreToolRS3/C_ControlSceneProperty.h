#pragma once

#include <string>
#include "RSceneNode.h"
#include "MXml.h"
#include "afxpropertygridctrl.h"
#include "CoreTypes.h"

class CControlMediator;
class CObjectPropertyGridCtrl;

// CControlSceneProperty 는 ui 의 프로퍼티와 신노드의 프로퍼티를 연결해주는 역할을 하며 
// 하나의 타입당 하나의 인스턴스를 싱글턴으로 가지고 있다

// Declare ID
#define DEC_NUM_CONTROLSCENE	0
#define DEC_NUM_TREESCENENODE	1	 
#define DEC_NUM_ACTORSCENENODE	2
#define DEC_NUM_TERRAIN			3
#define DEC_NUM_STATICMESH		4
#define DEC_NUM_SCENE			5

// id는 각각 달라야 하며, Tree Control의 image icon ID 로도 쓰입니다
#define DECLARE_PROPERTY_ID(x)	virtual unsigned int GetPropertyID() { return x; }
#define DECLARE_PROPERTY_NAME(x)	virtual const char* GetPropertyName() { static const char* id = x; return id; } // map< const char* , class > 형태


namespace rs3 {
	class RToolSceneManager;
}

class CControlSceneProperty : public CCoreObject
{
	friend rs3::RToolSceneManager;

protected:
	CControlSceneProperty();
public:
	~CControlSceneProperty();

	static CControlSceneProperty* GetInstance();
	DECLARE_PROPERTY_ID(DEC_NUM_CONTROLSCENE);
	DECLARE_PROPERTY_NAME("unknown");

	virtual void SetNewInstance(rs3::RSceneNode* pNewSceneNode, rs3::RSceneNode* pOldSceneNode, bool _bCopyMode) {}

	virtual void Init(CObjectPropertyGridCtrl* pPropertyUI, rs3::RSceneNode* pSceneNode, bool bEditable = true);
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* propertyItem, rs3::RSceneNode* pSceneNode);

protected:
	// 종속성을 끊기위해 넣었으나 싱글턴을 끊는것이 정석인듯
	static CControlMediator*	m_pControlMediator;

private:
	variant_t		m_nodeFileName;

	variant_t		m_posX;
	variant_t		m_posY;
	variant_t		m_posZ;

	variant_t		m_scale;

	variant_t		m_dirX;
	variant_t		m_dirY;
	variant_t		m_dirZ;

	variant_t		m_upX;
	variant_t		m_upY;
	variant_t		m_upZ;

	CMFCPropertyGridProperty*	m_pPosGroup;
	CMFCPropertyGridProperty*	m_pDirGroup;
	CMFCPropertyGridProperty*	m_pUpGroup;
	CMFCPropertyGridProperty*	m_pScaleProperty;

	inline bool IsPositionProperty(CMFCPropertyGridProperty* pProperty) { return pProperty->GetParent() == m_pPosGroup; }
	inline bool IsDirectionProperty(CMFCPropertyGridProperty* pProperty) { return (pProperty->GetParent() == m_pDirGroup) || (pProperty->GetParent() == m_pUpGroup); }
	inline bool IsScaleProperty(CMFCPropertyGridProperty* pProperty) { return m_pScaleProperty == pProperty; }
};

class CTerrainSceneNodeProperty : public CControlSceneProperty
{
public:
	static CTerrainSceneNodeProperty* GetInstance();
	DECLARE_PROPERTY_ID(DEC_NUM_TERRAIN);
	DECLARE_PROPERTY_NAME("Terrain");

	virtual void Init(CObjectPropertyGridCtrl* pPropertyUI, rs3::RSceneNode* pSceneNode, bool bEditable = true);
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* propertyItem, rs3::RSceneNode* pSceneNode);

private:
	variant_t m_bCreateNaviMesh;	//네비게이션 메쉬를 생성할 것인가, 아니면 말것인가 ? ( true : 네비메쉬생성 , false : 네비메쉬생성 안함 )
};

class CStaticMeshSceneNodeProperty : public CControlSceneProperty
{
public:
	static CStaticMeshSceneNodeProperty* GetInstance();
	DECLARE_PROPERTY_ID(DEC_NUM_STATICMESH);
	DECLARE_PROPERTY_NAME("Static Mesh");

	virtual void Init(CObjectPropertyGridCtrl* pPropertyUI, rs3::RSceneNode* pSceneNode, bool bEditable = true);
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* propertyItem, rs3::RSceneNode* pSceneNode);

private:
	variant_t m_bEnablePicking;	//코어툴에서 피킹이 될것인가, 안될것인가 ( false : 피킹안됨, true : 피킹됨 )
};

class CSceneNodeInstanceProperty : public CControlSceneProperty
{
public:
	static CSceneNodeInstanceProperty* GetInstance();
	DECLARE_PROPERTY_ID(DEC_NUM_SCENE);
	DECLARE_PROPERTY_NAME("Scene Instance");

	virtual void SetNewInstance(rs3::RSceneNode* pNewSceneNode, rs3::RSceneNode* pOldSceneNode, bool _bCopyMode);

	virtual void Init(CObjectPropertyGridCtrl* pPropertyUI, rs3::RSceneNode* pSceneNode, bool bEditable = true);
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* propertyItem, rs3::RSceneNode* pSceneNode);

private:
	variant_t m_bEditable;	///< 수정가능한가
	CMFCPropertyGridProperty* m_pPropertyEditable;
};

class CTreeSceneNodeProperty : public CControlSceneProperty
{
public:
	static CTreeSceneNodeProperty* GetInstance();
	DECLARE_PROPERTY_ID(DEC_NUM_TREESCENENODE);
	DECLARE_PROPERTY_NAME("Tree");

	virtual void SetNewInstance(rs3::RSceneNode* pNewSceneNode, rs3::RSceneNode* pOldSceneNode, bool _bCopyMode);

	virtual void Init(CObjectPropertyGridCtrl* pPropertyUI, rs3::RSceneNode* pSceneNode, bool bEditable = true);
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* propertyItem, rs3::RSceneNode* pSceneNode);
};

class CWaterPlaneSceneNodeProperty : public CControlSceneProperty
{
public:
	static CWaterPlaneSceneNodeProperty* GetInstance();
	virtual void Init(CObjectPropertyGridCtrl* pPropertyUI, rs3::RSceneNode* pSceneNode, bool bEditable = true);
	virtual void OnPropertyChanged( CMFCPropertyGridProperty* propertyItem, rs3::RSceneNode* pSceneNode );

protected:
	variant_t	m_fScale;
	variant_t	m_fAmplitude1;
	variant_t	m_fPhase1;
	variant_t	m_fUnderWaterDepth;
	variant_t	m_fBlending;
	
	CString		m_strShoreTexture;
	variant_t	m_fShoreIntensity;
	variant_t	m_fShoreUScale, m_fShoreVScale;
	CMFCPropertyGridProperty* m_pShoreTexProperty;

	variant_t	m_fRefractAttenuationDist;
	variant_t	m_fRefractAttenuationFactor;
};
