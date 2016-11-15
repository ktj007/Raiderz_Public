#pragma once

#include "M_LogicObject_AABBPicking.h"

#include "DirectionalLightPropertyReflection.h"
#include "PointLightPropertyReflection.h"
#include "SpotLightPropertyReflection.h"
#include "EffectInstanceReflection.h"
#include "ActorPropertyReflection.h"
#include "OccluderPropertyReflection.h"

//////////////////////////////////////////////////////////////////////////
/** @brief CLogicObject_SceneNode scenenode 를 편집하기 위한 base logic object */

class CORETOOL_API CLogicObject_SceneNode: public CLogicObject_AABBPicking
{
public:
	CLogicObject_SceneNode();
	~CLogicObject_SceneNode();

	rs3::RSceneNode* GetModelSceneNode() { return m_pModelNode; }

	void			LinkModelSceneNode( rs3::RSceneNode* pSceneNode );
	void			UnlinkModelSceneNode();

	void			SetController( CControlMediator* );

	virtual void	OnUpdate() override;

	CLogicObject*		Clone() override;

	ICommandBuffer*		CreateCloneNewCmdBuffer() override;
	ICommandBuffer*		CreateDeleteCmdBuffer() override;

	void				GatherLogicObjectsIncluding( std::list< CLogicObject* >& listObjects ) override;

	const char*			GetTypeName() override;

	CBaseObject*		GetReflectProperty() override
	{
		if(m_pPropertyReflection==NULL)
			return NULL;

		m_pPropertyReflection->SetSceneNode(m_pModelNode);
		return m_pPropertyReflection; 
	}

protected:
	virtual void		OnSetObjectName(const char* szName) override;
	virtual void		OnSetLayerName(const char* szName) override;
	virtual void		OnSetVisible(bool bVisible) override;
	virtual void		OnSetEditable(bool bEditable) override;

	virtual bool		OnGetPos_(rs3::RVector& _refVecPos) const override;
	virtual void		OnSetPos_(const rs3::RVector &_refUpdatePos) override;

	virtual bool		OnGetDir_(rs3::RVector& _refVecDir) const override;
	virtual bool		OnGetUp_(rs3::RVector& _refVecUp) const override;
	virtual void		OnSetDirUp_(const rs3::RVector &_refUpdateDir,const rs3::RVector &_refUpdateUp) override;

	virtual bool		OnGetScale_(float& _refSize) const override;
	virtual void		OnSetScale_(const float fUpdateRadius) override;

	void				NotifyChildrenLogicObjects();

	virtual int			GetIconIndex() override;

	rs3::RSceneNode*	m_pModelNode;
	CControlMediator*	m_pController;	// 하위 신노드에게 통보하기 위해 필요했는데,, 이게 맞는걸까

	CSceneNodeCommonProperty*		m_pPropertyReflection;

private:
	void				SetProperty(const char* szPropertyName, const char* szValue);
	void				RemoveProperty(const char* szPropertyName);
	int					FindUserPropertyIndex( const char* szPropertyName );
	const string&		FindUserProperty( const char* szPropertyName );
};

inline void CLogicObject_SceneNode::SetController( CControlMediator* pController) { m_pController = pController; }

//////////////////////////////////////////////////////////////////////////
// 외부 view 가 필요한것들

class CLogicObject_PointLight : public CLogicObject_SceneNode
{
public:
	CLogicObject_PointLight() { m_pPropertyReflection = new CPointLightPropertyRef; }
	
	virtual CLogicObjectRenderer* CreateRenderer( CControlMediator* pControlMediator_ ) override;
};

class CLogicObject_Occluder : public CLogicObject_SceneNode
{
public:
	CLogicObject_Occluder() 
	{ 
		m_pPropertyReflection = new COccluderPropertyRef;
	}
	virtual CLogicObjectRenderer* CreateRenderer( CControlMediator* pControlMediator_ ) override;
	virtual CLogicObject*	OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to) override;
};

/// scene.xml 을 뷰로 쓰는 LogicObject_SceneNode
class CLogicObject_SceneNodeWithSceneInstanceView: public CLogicObject_SceneNode
{
protected:
	CLogicObject*	OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to) override;
	void			OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum) override;

	rs3::RSceneNode*	CreateSceneInstanceView(const char* szSceneFileName);

	rs3::RSceneNode*	m_pViewSceneNode;
};

class CLogicObject_SpotLight : public CLogicObject_SceneNodeWithSceneInstanceView
{
public:
	CLogicObject_SpotLight() { m_pPropertyReflection = new CSpotLightPropertyRef; }
	virtual CLogicObjectRenderer* CreateRenderer( CControlMediator* pControlMediator_ ) override;
};

class CLogicObject_DirectionalLight : public CLogicObject_SceneNodeWithSceneInstanceView
{
public:
	CLogicObject_DirectionalLight() { m_pPropertyReflection = new CDirectionalLightPropertyRef; }
	virtual CLogicObjectRenderer* CreateRenderer( CControlMediator* pControlMediator_ ) override;
};

class CLogicObject_SubLight : public CLogicObject_SceneNodeWithSceneInstanceView
{
public:
	CLogicObject_SubLight() { m_pPropertyReflection = new CSubLightPropertyRef; }
	virtual CLogicObjectRenderer* CreateRenderer( CControlMediator* pControlMediator_ ) override;
};

class CLogicObject_EffectInstance : public CLogicObject_SceneNodeWithSceneInstanceView
{
public:
	CLogicObject_EffectInstance() { m_pPropertyReflection = new CEffectInstancePropertyRef; }
	CLogicObjectRenderer* CreateRenderer( CControlMediator* pControlMediator_ ) override;
};

//////////////////////////////////////////////////////////////////////////
// ( model scenenode 가 보여서 ) view 가 필요없는 인노드들

class CLogicObject_VisibleSceneNode : public CLogicObject_SceneNode
{
public:
	virtual CLogicObjectRenderer* CreateRenderer( CControlMediator* pControlMediator_ ) override;

protected:
	CLogicObject*	OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to) override;
	void			OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum) override;
};

class CLogicObject_Tree : public CLogicObject_VisibleSceneNode
{
public:
	virtual CControlSceneProperty*	GetProperty() override;

	CLogicObject*	OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to) override;
	void			OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum) override;
};

class CLogicObject_SceneInstance : public CLogicObject_VisibleSceneNode
{
public:
	virtual CControlSceneProperty*	GetProperty() override;
};

class CLogicObject_StaticMesh : public CLogicObject_VisibleSceneNode
{
public:
	virtual CControlSceneProperty*	GetProperty() override;

	CLogicObject*	OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to) override;
	void			OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum) override;
};

class CLogicObject_Terrain : public CLogicObject_VisibleSceneNode
{
public:
	virtual CControlSceneProperty*	GetProperty() override;

	// 피킹 생략
	CLogicObject*	OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to) override { return NULL; }
	void			OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum) override {}
};

class CLogicObject_Actor : public CLogicObject_VisibleSceneNode
{
public:
	CLogicObject_Actor() { m_pPropertyReflection = new CActorPropertyRef; }
};


class CLogicObject_WaterPlane : public CLogicObject_VisibleSceneNode
{
public:
	virtual CControlSceneProperty*	GetProperty() override;

	// 피킹 생략
	CLogicObject*	OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to) override { return NULL; }
	void			OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum) override {}
};
