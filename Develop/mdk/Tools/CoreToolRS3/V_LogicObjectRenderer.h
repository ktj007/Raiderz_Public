#pragma once

#include "V_LogicObjectRenderer.h"

class CLogicObject_AABBPicking;


/** @brief CLogicObjectRenderer LogicObject 에 custom view 를 구현하기 위한 interface class */
class CLogicObjectRenderer
{
public:
	virtual ~CLogicObjectRenderer() {}

	virtual void	OnAdd(CControlMediator*	pControlMediator_) {}			// manager 에 등록될때 
	virtual void	OnRemove(CControlMediator*	pControlMediator_) {}		// manager 에 빠질때
	virtual void	OnChanged() {}			// logicobject 속성이 바뀌었을때
	virtual void	OnRender() {}			// render helper 등으로 직접 그릴경우
	virtual void	OnRenderSelection() {}	// 선택된경우
	virtual void	OnRenderTextInfo() {}
	virtual void	OnShow() {}				// 보여질때
	virtual void	OnHide() {}				// 숨겨질때
};


//////////////////////////////////////////////////////////////////////////
/** @brief CLogicObjectRenderer_SceneNode sceneNode 를 renderer 로 사용하기 위한 CLogicObjectRenderer */

class CORETOOL_API CLogicObjectRenderer_SceneNode : public CLogicObjectRenderer
{
public:
	CLogicObjectRenderer_SceneNode(CLogicObject_AABBPicking* pLogicObject, rs3::RSceneNode* pViewSceneNode)
		: m_pViewNode(pViewSceneNode) { m_pLogicObject = pLogicObject; }
	virtual ~CLogicObjectRenderer_SceneNode();

	void			SetViewSceneNode(rs3::RSceneNode* pSceneNode) { m_pViewNode = pSceneNode; }
	rs3::RSceneNode* GetViewSceneNode() { return m_pViewNode; }

	virtual void	OnAdd(CControlMediator*	pControlMediator_) override;
	virtual void	OnRemove(CControlMediator*	pControlMediator_) override;
	virtual void	OnChanged() override;
	virtual void	OnShow() override;
	virtual void	OnHide() override;

protected:
	virtual bool	GetCollisionEnabled() { return false; }

	rs3::RSceneNode*			m_pViewNode;
	CLogicObject_AABBPicking*	m_pLogicObject;	///< 참조하는 document
};

//////////////////////////////////////////////////////////////////////////
/** @brief CLogicObjectRenderer_SceneNodeInstance sceneNodeInstance 를 renderer 로 사용하기 위한 CLogicObjectRenderer */

class CORETOOL_API CLogicObjectRenderer_SceneNodeInstance : public CLogicObjectRenderer_SceneNode
{
public:
	CLogicObjectRenderer_SceneNodeInstance(CLogicObject_AABBPicking* pLogicObject, rs3::RSceneNode* pSceneNode)
		: CLogicObjectRenderer_SceneNode( pLogicObject, pSceneNode ) {}

	void	LoadSceneNodeInstance(CControlMediator* pControlMediator_, const char* szSceneFilename);
	void	ReloadSceneNodeInstance(CControlMediator* pControlMediator_, const char* szSceneFilename);
};


//////////////////////////////////////////////////////////////////////////
/** @brief CLogicObjectRenderer_SceneNodeItself scenenode model 자기 자신을 renderer 로 사용하기 위한 CLogicObjectRenderer */

class CORETOOL_API CLogicObjectRenderer_SceneNodeItself : public CLogicObjectRenderer
{
public:
	CLogicObjectRenderer_SceneNodeItself(CLogicObject_AABBPicking* pLogicObject, rs3::RSceneNode* pSceneNode)
		: m_pSceneNode(pSceneNode) { m_pLogicObject = pLogicObject; }

protected:
	rs3::RSceneNode*			m_pSceneNode;	///< model 이자 view 노드
	CLogicObject_AABBPicking*	m_pLogicObject;	///< 참조하는 document

	virtual void	OnChanged() override;
	virtual void	OnShow() override;
	virtual void	OnHide() override;
};
