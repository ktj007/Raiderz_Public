#pragma once

#include "M_CommandBuffer.h"

class CORETOOL_API CLogicNewCmdBuffer : public ICommandBuffer
{
public:
	CLogicNewCmdBuffer(CLogicObject* _pObject);
	virtual ~CLogicNewCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

private:
	std::string m_strUndoMessage_;
	CLogicObject* m_pObject_;
};

class CORETOOL_API CLogicDeleteCmdBuffer : public ICommandBuffer
{
public:
	CLogicDeleteCmdBuffer(CLogicObject* _pObject);
	virtual ~CLogicDeleteCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

private:
	std::string m_strUndoMessage_;
	CLogicObject* m_pObject_;
};

class CORETOOL_API CLogicMoveCmdBuffer : public ICommandBuffer
{
public:
	CLogicMoveCmdBuffer(CLogicObject* _pLogicObject, const rs3::RVector& _refVecOrgPos);
	virtual ~CLogicMoveCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

	const rs3::RVector&			GetPosOrg() { return m_vPos_; }

private:
	std::string m_strUndoMessage_;
	CLogicObject* m_pLogicObject_;
	rs3::RVector m_vPos_;
};

class CORETOOL_API CLogicRotateCmdBuffer : public ICommandBuffer
{
public:
	CLogicRotateCmdBuffer(CLogicObject* _pLogicObject, const rs3::RVector& _refVecOrgDir, const rs3::RVector& _refVecOrgUp);
	CLogicRotateCmdBuffer(CLogicObject* _pLogicObject, const rs3::RVector& _refVecOrgDir, const rs3::RVector& _refVecOrgUp,
		const rs3::RVector& _refVecOrgPos, const rs3::RVector& _refVecPivot);
	virtual ~CLogicRotateCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

	const rs3::RVector&			GetRotOrgDir() { return m_vDir_; }
	const rs3::RVector&			GetRotOrgUp() { return m_vUp_; }
	const rs3::RVector&			GetRotOrgPos() { return m_vPos_; }

	const rs3::RVector&			GetRotPivot() { return m_vPivot; }

private:
	std::string m_strUndoMessage_;
	CLogicObject* m_pLogicObject_;
	rs3::RVector m_vDir_;
	rs3::RVector m_vUp_;

	bool m_bPosMoved;
	rs3::RVector m_vPos_;	// 특정피벗을 중심으로 회전할 경우가 있어서 위치도 필요하다
	rs3::RVector m_vPivot;	// 회전의 중심 , 실제 언두할때는 필요치않으나 돌리는 중간에 필요해서 넣었다
};

class CORETOOL_API CLogicScaleCmdBuffer : public ICommandBuffer
{
public:
	CLogicScaleCmdBuffer(CLogicObject* _pLogicObject, float _fOrgScale);
	virtual ~CLogicScaleCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

private:
	std::string m_strUndoMessage_;
	CLogicObject* m_pLogicObject_;
	float m_fOrgScale_;
};

//////////////////////////////////////////////////////////////////////////
// logicobject_scenenode
class CLogicObject_SceneNode;

class CORETOOL_API CLogicObject_SceneNodeCloneNewCmdBuffer : public ICommandBuffer
{
public:
	CLogicObject_SceneNodeCloneNewCmdBuffer(CLogicObject_SceneNode* _pObject);
	virtual ~CLogicObject_SceneNodeCloneNewCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

private:
	std::string m_strUndoMessage_;
	CLogicObject_SceneNode* m_pObject_;
	rs3::RSceneNode*	m_pModelNode;
	rs3::RSceneNode*	m_pParentNode;
};

class CORETOOL_API CLogicObject_SceneNodeDeleteCmdBuffer : public ICommandBuffer
{
public:
	CLogicObject_SceneNodeDeleteCmdBuffer(CLogicObject* _pObject);
	virtual ~CLogicObject_SceneNodeDeleteCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

private:
	std::string m_strUndoMessage_;
	CLogicObject* m_pObject_;
	rs3::RSceneNode*	m_pModelNode;
	rs3::RSceneNode*	m_pParentNode;
};
