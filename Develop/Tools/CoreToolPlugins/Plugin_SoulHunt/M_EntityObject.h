#pragma once

#include "M_LogicObject_AABBPicking.h"
#include "RTypes.h"

#include "M_EntityCommandBuffer.h"

#include "V_LogicObjectRenderer.h"

#include "../../ToolsCommon/toolPick.h"

#include "LogicObject_ForIconSelect.h"

namespace rs3
{
	class RSceneNode;
	class RToolSceneManager;
	class REffectSceneNode;
};

class CToolLogicObjectManager;
class CControlMediator;
class CControlMediatorHelperRender;

class CORETOOL_SOULHUNT_API CEntityObject : public CLogicObject_AABBPicking
{
public:

	CEntityObject(void);
	virtual ~CEntityObject(void);

	//////////////////////////////////////////////////////////////////////////
	//
	// Entity Const Value
	//
	//////////////////////////////////////////////////////////////////////////

	enum ENTITY_CONST
	{
		SPAWN_HEIGHT = 80,
		MARKER_HEIGHT = 100,
		DIRECTION_DISTANCE = 300
	};


	//////////////////////////////////////////////////////////////////////////
	//
	// Entity Type
	//
	//////////////////////////////////////////////////////////////////////////

	enum TYPE
	{
		ENTITY_ROOM = 0, // SPAWN보다 위에 있어야함
		ENTITY_SPAWN,
		ENTITY_MARKER,
		ENTITY_SENSOR,

		ENTITY_SPAWN_MAP_OBJ,
		ENTITY_SOUND,
		ENTITY_SPAWN_GATHER,

		ENTITY_AREA,
		
		ENTITY_OBJ_NUM
	};

	static TYPE				IdentifyTypeFromChar(const char* _pSzType);
	inline TYPE				GetType(){ return m_eEntityObjectType_; }


	//////////////////////////////////////////////////////////////////////////
	//
	// create / clone
	//
	//////////////////////////////////////////////////////////////////////////

	// TODO: factory 를 위해 추가했다, 숨기자
	static CControlMediator* m_pControlMediator;
	static CEntityObject*	CreateEntityObject(TYPE _type);
	CEntityObject*			CloneAtWB(const rs3::RVector& _refPos);

	virtual CLogicObject*	Clone() override;
	virtual ICommandBuffer*	CreateCloneNewCmdBuffer() override;

	// 특정 Attribute가 Double Clicked 되었을때
	virtual bool			NotifyPosAttributeDClickedJustOneEntity(SAttributeAtXML* _pDClickedEntityAttribute){ return true; }
	virtual bool			NotifyDirAttributeDClickedJustOneEntity(SAttributeAtXML* _pDClickedEntityAttribute){ return true; }
	virtual bool			NotifyUpAttributeDClickedJustOneEntity(SAttributeAtXML* _pDClickedEntityAttribute){ return true; }

	// 한 Entity의 Attribute 변경 수신
	virtual bool			NotifyAttributeChangedJustOneEntity(SAttributeAtXML* _pChangedEntityAttribute){ return true; }

	// Entity Render Helper Scale
	virtual void			GetRenderHelperScale(rs3::RVector3& _rHelperScale){}

	void					SetValid(bool bFlag = true)	{ m_bVaildPosition = bFlag; }
	bool					GetValid()					{ return m_bVaildPosition; }

	void					SetZoneLoad(bool bValue)	{ m_bZoneLoad = bValue; }
	bool					GetZoneLoad()				{ return m_bZoneLoad; }

	virtual void			SaveToXML(MXmlElement& _refElement, bool bCommon);
	
	void SettingEditMode();

	void					UpdatePickingForSelection(float& fSize, float& fOffSetHeight);

protected:

	TYPE					m_eEntityObjectType_;
	bool					m_bVaildPosition;
	bool					m_bZoneLoad;

	CLogicObject_ForIconSelect	m_ObjectSelect;

	void					OnUpdateAABB_();
	virtual void			UpdateEntityAABB_() = 0;


	// Entity는 조금 특수하게 아래의 상황들을 처리한다.
	virtual void				OnRender(){}
	virtual CLogicObject*		OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to) override;
	virtual void				OnSelected_(bool bSelected){}

	bool					IntersectTriangle(const vec3& orig, const vec3& dir, vec3& v0, vec3& v1, vec3& v2, float *t, float *u, float *v);

};

//////////////////////////////////////////////////////////////////////////
// Entity Model
class CORETOOL_SOULHUNT_API RaiderzLogicObject_Renderer : public CLogicObjectRenderer_SceneNodeInstance
{
protected:
	string				m_strSceneFileName;

public:
	RaiderzLogicObject_Renderer( CLogicObject_AABBPicking* pLogicObject, CControlMediator* pControlMediator_, string& strFileName);

	void CreateView();

	void RendererChanged(vec3 vScale, vec3 vPos, vec3 vDir, vec3 vUp);

	void SetColor(DWORD dwColor);

	CControlMediator*		m_pControlMediator;
};	

class CORETOOL_SOULHUNT_API RaiderzLogicObject_ArrowRenderer : public RaiderzLogicObject_Renderer
{
public:
	RaiderzLogicObject_ArrowRenderer( CEntityObject* pLogicObject, CControlMediator* pControlMediator_);

	void OnChanged(vec3 vScale, vec3 vPos, vec3 vDir, vec3 vUp, DWORD dwColor);

	CEntityObject*			m_pDocument;
};

//////////////////////////////////////////////////////////////////////////

class CORETOOL_SOULHUNT_API CEntitySpawn : public CEntityObject
{
public:

	inline bool			IsRandomDir();
	inline bool			GetRoamRadius(float& _refRoamRadius);
	inline bool			GetRoamDepth(float& _refRoamDepth);
	inline bool			GetCombatRadius(float& _refCombatRadius);
	inline float		GetHeightWithNpc(){ return m_fHeight; }

	inline DWORD		GetColor(){ return m_dwColor_; }
	void				SetColor(COLORREF _color);
	void				SetAutoColor();
	void				UpdateColor();

	//NPC 관련
	void				AddNpc(rs3::RToolSceneManager* _pSceneManager, CToolLogicObjectManager* _pLogicObjectManager, int _id, bool _bArrange = true);
	void				RemoveNpc(int _id, bool _bArrange = false);
	bool				IsMapObject(){ return m_bMapObject_; }

	rs3::RSceneNode*	GetNpcToRender();
	void				RenderNpcForSelection(CControlMediator* pControlMediator_);
	virtual CLogicObjectRenderer*		CreateRenderer(CControlMediator* pControlMediator_ );

	std::map<int, rs3::RSceneNode*>*				GetMapNpcIdNode() { return &m_mapNpcIdNode_; };

private:

	// 부모의 Factory를 통해서만 생성
	friend class CEntityObject;
	CEntitySpawn();
	~CEntitySpawn();

	//스택에 생성을 할것이 아니므로 임의 복사생성 금지
	CEntitySpawn(const CEntitySpawn&);
	CEntitySpawn& operator= (const CEntitySpawn&);

	virtual void		UpdateSpecialBOOL_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase);
	virtual void		UpdateSpecialSTRING_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase);
	virtual void		UpdateSpecialUINT_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase);
	virtual void		OnLoadCompleted_();
	
	virtual void		OnSetPos_(const rs3::RVector &_refUpdatePos);
	virtual void		OnSetDirUp_(const rs3::RVector &_refUpdateDir, const rs3::RVector &_refUpdateUp);

	virtual void		UpdateEntityAABB_();

	static DWORD		GetAutoColorValue_( int _nID );
	void				UpdateNpcDir_();

	static void			SetNpcPos_(std::map<int, rs3::RSceneNode*>& _refNpcMap, const rs3::RVector& _refPos);
	static void			SetNpcDir_(std::map<int, rs3::RSceneNode*>& _refNpcMap, const rs3::RVector& _refDir);

	virtual void		LoadPropertyFromXML(SElementAtXML* _pEntityProperty, MXmlElement* _pElement = NULL) override;

	//random_dir
	double*				m_pdBoolRandomDir_;
	double*				m_pdRoamRadius_;
	double*				m_pdRoamDepth_;

	double*				m_pdCombatRadius_;

	float				m_fHeight;

	std::string*		m_pStrColorName_;
	DWORD				m_dwColor_;
	bool				m_bMapObject_;

	typedef				std::map<int, rs3::RSceneNode*> MAP_NPC;
	MAP_NPC				m_mapNpcIdNode_;
};

inline bool CEntitySpawn::IsRandomDir()
{
	if(m_pdBoolRandomDir_ == NULL)
		return false;
	else
	{
		if(	*m_pdBoolRandomDir_ == 0.f)
			return false;
		else
			return true;
	}
}

inline bool CEntitySpawn::GetRoamRadius(float& _refRoamRadius)
{
	if(m_pdRoamRadius_ == NULL)
		return false;
	else
	{
		_refRoamRadius = (float)*m_pdRoamRadius_;
		return true;
	}
}

inline bool CEntitySpawn::GetRoamDepth(float &_refRoamDepth)
{
	if(m_pdRoamDepth_ == NULL)
		return false;
	else
	{
		_refRoamDepth = (float)*m_pdRoamDepth_;
		return true;
	}
}

inline bool CEntitySpawn::GetCombatRadius(float& _refCombatRadius)
{
	if(m_pdCombatRadius_ == NULL)
		return false;
	else
	{
		_refCombatRadius = (float)*m_pdCombatRadius_;
		return true;
	}
}

//////////////////////////////////////////////////////////////////////////

class CORETOOL_SOULHUNT_API CEntityMarker : public CEntityObject
{
friend class CEntityObject;
public:
	virtual CLogicObjectRenderer*		CreateRenderer(CControlMediator* pControlMediator_ );

private:
	CEntityMarker();
	~CEntityMarker();

	//스택에 생성을 할것이 아니므로 임의 복사생성 금지
	CEntityMarker(const CEntityMarker&);
	CEntityMarker& operator= (const CEntityMarker&);

protected:
	virtual void UpdateEntityAABB_();
};

//////////////////////////////////////////////////////////////////////////

class CORETOOL_SOULHUNT_API CEntitySensor : public CEntityObject
{
friend class CEntityObject;

private:
	CEntitySensor();
	~CEntitySensor();

	//스택에 생성을 할것이 아니므로 임의 복사생성 금지
	CEntitySensor(const CEntitySensor&);
	CEntitySensor& operator= (const CEntitySensor&);

	virtual void			UpdateSpecialSTRING_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase);
	virtual void			UpdateSpecialFLOAT_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase);
	virtual void			UpdateSpecialPos_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase);
	virtual void			UpdateSpecialDir_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase);
	virtual void			UpdateSpecialUp_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase);

	virtual void			UpdateEntityAABB_();
	virtual void			OnLoadCompleted_();

	virtual bool			OnGetPos_(rs3::RVector& _refVecPos) const;
	virtual void			OnSetPos_(const rs3::RVector &_refUpdatePos);
	virtual SAttributeAtXML* OnGetPosAttribute_();

	virtual bool			OnGetDir_(rs3::RVector& _refVecDir) const;
	virtual void			OnSetDirUp_(const rs3::RVector &_refUpdateDir, const rs3::RVector &_refUpdateUp);
	virtual SAttributeAtXML* OnGetDirAttribute_();

	virtual bool			OnGetUp_(rs3::RVector& _refVecUp) const;
	virtual SAttributeAtXML* OnGetUpAttribute_();

	virtual void			OnSelected_(bool bSelected);

	rs3::REffectSceneNode*	m_pSensorEffect_;
	double*					m_dSensorEffectPos[3];
	double*					m_dSensorEffectDir[3];
	double*					m_dSensorEffectUp[3];
	rs3::RVector3			m_vSensorEffectScale_;
	bool					m_bSensorEffectEditMode_;
	SAttributeAtXML*		m_pSensorEffectPosAttribute_;
	SAttributeAtXML*		m_pSensorEffectDirAttribute_;
	SAttributeAtXML*		m_pSensorEffectUpAttribute_;


	void					CreateSensorEffect_(std::string& _strSensroName);
	void					UpdateSensorEffectTM_();
	void					DestroySensorEffect_();
	void					SetEnableSensorEffect_(bool bEnable);

public:
	// xml 일부 속성 제거
	virtual void			LoadPropertyFromXML(SElementAtXML* _pEntityProperty, MXmlElement* _pElement = NULL) override;
	void					RemoveUnusedWarpAttribute(SElementAtXML* pWarpElement);

	virtual bool			NotifyPosAttributeDClickedJustOneEntity(SAttributeAtXML* _pDClickedEntityAttribute);
	virtual bool			NotifyDirAttributeDClickedJustOneEntity(SAttributeAtXML* _pDClickedEntityAttribute);
	virtual bool			NotifyUpAttributeDClickedJustOneEntity(SAttributeAtXML* _pDClickedEntityAttribute);
	virtual bool			NotifyAttributeChangedJustOneEntity(SAttributeAtXML* _pChangedEntityAttribute);
	virtual void			GetRenderHelperScale(rs3::RVector3& _rHelperScale){ _rHelperScale = m_vSensorEffectScale_; }

	void					GetSensorRenderPos(rs3::RMatrix& _refTrans);

	virtual CLogicObject*	OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to);
	virtual void			OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum);
};

//////////////////////////////////////////////////////////////////////////

class CORETOOL_SOULHUNT_API CEntitySound : public CEntityObject
{
friend class CEntityObject;

public:

	bool					GetDistanceMin(float &_refDistanceMin);
	bool					GetDistanceMax(float &_refDistanceMax);
	bool					GetVolume(float &_refVolume);

	inline SAttributeAtXML*	GetDistanceMinAttribute(){ return m_pDistanceMinAttribute_; }

	virtual CLogicObject*	OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to);
	virtual void			OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum);

protected:

	virtual void			OnUpdate() { UpdateEntityAABB_(); }
	virtual void			OnSetScale_(const float fUpdateRadius);

	virtual void			UpdateSpecialSTRING_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase);
	virtual void			UpdateSpecialFLOAT_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase);

	virtual void			UpdateEntityAABB_();

private:

	double*					m_pdDistanceMin_;
	SAttributeAtXML*		m_pDistanceMinAttribute_;

	double*					m_pdDistanceMax_;
	double*					m_pdVolume_;
	
	CEntitySound();
	~CEntitySound();

	CEntitySound(const CEntitySound&);
	CEntitySound& operator= (const CEntitySound&);
};

//////////////////////////////////////////////////////////////////////////

#define AREA_CYLINDER_SEGMENT	40
#define TEST_CYLINDER_COUNT		8

class CORETOOL_SOULHUNT_API CEntityArea : public CEntityObject
{
public:
	bool		m_bCreateShapeMode;
	MVector3	m_CylinderPos;
	int			m_nSubCylinderSelectedIdx;

	void SetTempPos(double* _pX, double* _pY, double* _pZ)
	{
		m_pdX_ = _pX;
		m_pdY_ = _pY;
		m_pdZ_ = _pZ;
	}

	void ClearTempPos()
	{
		m_pdX_ = NULL;
		m_pdY_ = NULL;
		m_pdZ_ = NULL;
	}

	void SetMoveBufferPos( const rs3::RVector3& _rPos)
	{
		delete m_pMoveBuffer_;
		m_pMoveBuffer_ = new CEntityMoveCmdBuffer(this, _rPos);
	}

	CEntityMoveCmdBuffer* GetMoveBuffer()
	{
		return m_pMoveBuffer_;
	}

	virtual bool NotifyAttributeChangedJustOneEntity(SAttributeAtXML* _pChangedEntityAttribute);
	virtual CLogicObject*		OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to);
	virtual void				OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum);

	void GetCylinderAABBs(std::vector< std::pair<SAttributeAtXML*,MBox> >& _vMBox);
	void UpdateAABB(){ UpdateEntityAABB_(); }

protected:
	virtual void UpdateEntityAABB_();
	virtual bool OnGetPos_( rs3::RVector& _refVecPos ) const;

private:
	friend class CEntityObject;
	CEntityArea()
	{
		m_bCreateShapeMode = false;
		m_dScaleTemp_ = 0;
		m_nSubCylinderSelectedIdx = -1;

		m_pdRadius_ = &m_dScaleTemp_;

		m_pMoveBuffer_ = new CEntityMoveCmdBuffer(this, rs3::RVector(0.f, 0.f, 0.f));
	}

	~CEntityArea()
	{
		delete m_pMoveBuffer_;
	}

	double m_dScaleTemp_;
	CEntityMoveCmdBuffer* m_pMoveBuffer_;

	struct stCylinder
	{
		vec3	m_vCylinderVertices[AREA_CYLINDER_SEGMENT * 2];
	};

	vector<stCylinder>	m_vecCylinderForPicking;

	void	CylinderPolygonUpdate();
	void	CalCylinderPolygon(stCylinder& ClyinderData, const MMatrix &matWorld, float fRadius, float fHeight);
};


//////////////////////////////////////////////////////////////////////////

class CORETOOL_SOULHUNT_API CEntityRoom : public CEntityObject
{
	friend class CEntityObject;
private:
	CEntityRoom();
	~CEntityRoom();

	//스택에 생성을 할것이 아니므로 임의 복사생성 금지
	CEntityRoom(const CEntityRoom&);
	CEntityRoom& operator= (const CEntityRoom&);

protected:
	virtual void UpdateEntityAABB_();

private:
	virtual void SaveToXML(MXmlElement& _refElement, bool bCommon) override;

public:
	rs3::ROBB GetOBB();
};
