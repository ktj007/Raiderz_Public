#pragma once

#include <vector>
#include <string>

#include "RSceneNode.h"
#include "RTypes.h"
#include "CoreTypes.h"

class CControlMediator;
class CLogicObject;

namespace rs3
{
	class RToolTerrain;
	class RToolSceneManager;
}

class CORETOOL_API ICommandBuffer
{
public:
	ICommandBuffer();
	virtual ~ICommandBuffer();

	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController) = 0;
	virtual const std::string&	GetUndoMessage() = 0;
	virtual void				LogUndoMessage() = 0;

	size_t m_tCommandBufferSize;
};

class CORETOOL_API CGroupedCommandBuffer : public ICommandBuffer
{
public:

	CGroupedCommandBuffer();
	virtual ~CGroupedCommandBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage();
	virtual void				LogUndoMessage(){}

	void AddBuffer(ICommandBuffer* _pCommandBuffer);
	bool IsEmpty(){ return m_vecCmdBuffer_.empty(); }

private:
	std::string* m_pStrUndoMessage_;
	std::string m_strEmptyUndoMessage_;
	std::vector<ICommandBuffer*> m_vecCmdBuffer_;
};

class CORETOOL_API CObjectNewCmdBuffer : public ICommandBuffer
{
public:
	CObjectNewCmdBuffer(rs3::RToolSceneManager* pSceneManager, rs3::RSceneNode* _pSceneNode);
	virtual ~CObjectNewCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

private:
	std::string m_strUndoMessage_;
	rs3::RSceneNode* m_pSceneNode_;
	rs3::RToolSceneManager* m_pSceneManager;
};

class CORETOOL_API CObjectDeleteCmdBuffer : public ICommandBuffer
{
public:
	CObjectDeleteCmdBuffer(rs3::RSceneNode* _pSceneNode);
	virtual ~CObjectDeleteCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

private:
	std::string m_strUndoMessage_;
	rs3::RSceneNode* m_pSceneNode_;
};

class CORETOOL_API CObjectMoveCmdBuffer : public ICommandBuffer
{
public:
	CObjectMoveCmdBuffer(rs3::RSceneNode* _pSceneNode, const rs3::RVector& _refRVecOrg);
	virtual ~CObjectMoveCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

	const rs3::RVector&			GetPosOrg() { return m_vPos_; }

private:
	std::string m_strUndoMessage_;
	rs3::RSceneNode* m_pSceneNode_;
	rs3::RVector m_vPos_;
};

class CORETOOL_API CObjectScaleCmdBuffer : public ICommandBuffer
{
public:
	CObjectScaleCmdBuffer(rs3::RSceneNode* _pSceneNode, const rs3::RMatrix& _refRMatrixOrg);
	virtual ~CObjectScaleCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

private:
	std::string m_strUndoMessage_;
	rs3::RSceneNode* m_pSceneNode_;
	rs3::RMatrix m_matOrg_;
};

class CORETOOL_API CObjectRotateCmdBuffer : public ICommandBuffer
{
public:
	CObjectRotateCmdBuffer(rs3::RSceneNode* _pSceneNode, const rs3::RMatrix& _refMatOrg);
	virtual ~CObjectRotateCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

	const rs3::RMatrix&			GetRotOrg() { return m_matOrg_; }

private:
	std::string m_strUndoMessage_;
	rs3::RSceneNode* m_pSceneNode_;
	rs3::RMatrix m_matOrg_;
};

class CORETOOL_API CTerrainHeightCmdBuffer : public ICommandBuffer
{
public:
	CTerrainHeightCmdBuffer(rs3::RToolTerrain* _pTerrain);
	virtual ~CTerrainHeightCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

private:
	std::string m_strUndoMessage_;
	float* m_pFloatHeightMapBuffer_;
	rs3::RToolTerrain* m_pTerrain_;
};

class CORETOOL_API CTerrainSplatCmdBuffer : public ICommandBuffer
{
public:
	CTerrainSplatCmdBuffer(int _layer, int _width, int _height, BYTE* _pByteSrc);
	virtual ~CTerrainSplatCmdBuffer();

	//interface implement
	virtual VIEW_MESSAGE::ID	Undo(CControlMediator* _pController);
	virtual const std::string&	GetUndoMessage(){ return m_strUndoMessage_; }
	virtual void				LogUndoMessage();

private:
	std::string m_strUndoMessage_;
	int m_nLayer_;
	BYTE* m_pByteSplattingBuffer_;
	BYTE* m_pByteOriginalByteAddress_;
};