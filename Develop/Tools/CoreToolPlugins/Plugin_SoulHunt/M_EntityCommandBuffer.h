#pragma once

#include "M_CommandBuffer.h"

class CEntityObject;
class CORETOOL_SOULHUNT_API CEntityNewCmdBuffer : public ICommandBuffer
{
public:
	CEntityNewCmdBuffer(CEntityObject* _pEntityObject);
	virtual ~CEntityNewCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

private:
	std::string m_strUndoMessage_;
	CEntityObject* m_pEntityObject_;
	bool m_bUnDoExecuted;
};

class CORETOOL_SOULHUNT_API CEntityDeleteCmdBuffer : public ICommandBuffer
{
public:
	CEntityDeleteCmdBuffer(CEntityObject* _pEntityObject);
	virtual ~CEntityDeleteCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

private:
	std::string m_strUndoMessage_;
	CEntityObject* m_pEntityObject_;
};

class CORETOOL_SOULHUNT_API CEntityMoveCmdBuffer : public ICommandBuffer
{
public:
	CEntityMoveCmdBuffer(CEntityObject* _pEntityObject, const rs3::RVector& _refVecOrgPos);
	virtual ~CEntityMoveCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();
	const rs3::RVector&			GetPosOrg()  { return m_vPos_; }
private:
	std::string m_strUndoMessage_;
	CEntityObject* m_pEntityObject_;
	rs3::RVector m_vPos_;
};

class CORETOOL_SOULHUNT_API CEntityScaleCmdBuffer : public ICommandBuffer
{
public:
	CEntityScaleCmdBuffer(CEntityObject* _pEntityObject, float _fOrgScale);
	virtual ~CEntityScaleCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

private:
	std::string m_strUndoMessage_;
	CEntityObject* m_pEntityObject_;
	float m_fOrgScale_;
};

class CORETOOL_SOULHUNT_API CEntityRotateCmdBuffer : public ICommandBuffer
{
public:
	CEntityRotateCmdBuffer(CEntityObject* _pEntityObject, const rs3::RVector& _refVecOrgDir);
	virtual ~CEntityRotateCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

	const rs3::RVector&			GetRotOrg() { return m_vDir_; }

private:
	std::string m_strUndoMessage_;
	CEntityObject* m_pEntityObject_;
	rs3::RVector m_vDir_;
};

struct SAttributeAtXML;
class CEntitySpawn;
class CORETOOL_SOULHUNT_API CEntityAttrGenVec1Buffer : public ICommandBuffer
{
public:
	CEntityAttrGenVec1Buffer(CEntitySpawn* _pEntitySpawn, SAttributeAtXML* _pEntityAttr, double _dGenVec1, const std::string& _strMessage);
	virtual ~CEntityAttrGenVec1Buffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage(){}

private:
	std::string m_strUndoMessage_;
	SAttributeAtXML* m_pEntityAttribute_;
	double m_dGenVec1_;
	CEntitySpawn* m_pEntitySpawn_;
};

class CORETOOL_SOULHUNT_API CEntityAttrGenStringBuffer : public ICommandBuffer
{
public:
	CEntityAttrGenStringBuffer(SAttributeAtXML* _pEntityAttr, const std::string& _strGenString, const std::string& _strMessage);
	virtual ~CEntityAttrGenStringBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage(){}

private:
	std::string m_strUndoMessage_;
	SAttributeAtXML* m_pEntityAttribute_;
	std::string m_strGenString_;
};
