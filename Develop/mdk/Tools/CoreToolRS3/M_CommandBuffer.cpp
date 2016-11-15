#include "StdAfx.h"
#include "M_CommandBuffer.h"
#include "M_ToolCmdMgr.h"
#include "M_ToolLogicObjectManager.h"

#include "M_ToolTerrain.h"
#include "M_ToolSceneManager.h"
#include "C_ControlMediatorHelperAction.h"

#include "VL_SceneNodeList.h"

//Command Buffer Interface
ICommandBuffer::ICommandBuffer() : m_tCommandBufferSize(0){}
ICommandBuffer::~ICommandBuffer()
{
}

//////////////////////////////////////////////////////////////////////////////
///
/// Grouped Command Buffer
CGroupedCommandBuffer::CGroupedCommandBuffer() : m_strEmptyUndoMessage_("빈 그룹 언두 버퍼")
{
	m_pStrUndoMessage_ = NULL;
	m_tCommandBufferSize = 5;
}

CGroupedCommandBuffer::~CGroupedCommandBuffer()
{
	for(std::vector<ICommandBuffer*>::iterator itr = m_vecCmdBuffer_.begin();
		itr != m_vecCmdBuffer_.end(); ++itr)
	{
		delete *itr;
	}
}

VIEW_MESSAGE::ID CGroupedCommandBuffer::Undo(CControlMediator* _pController)
{
	VIEW_MESSAGE::ID message = VIEW_MESSAGE::UNDO_GENERAL;
	for(std::vector<ICommandBuffer*>::iterator itr = m_vecCmdBuffer_.begin();
		itr != m_vecCmdBuffer_.end(); ++itr)
		message = (*itr)->Undo(_pController);
	return message;
}

const std::string& CGroupedCommandBuffer::GetUndoMessage()
{
	if(m_pStrUndoMessage_ != NULL)
		return *m_pStrUndoMessage_;
	else
		return m_strEmptyUndoMessage_;
}

void CGroupedCommandBuffer::AddBuffer(ICommandBuffer* _pCommandBuffer)
{
	ASSERT(_pCommandBuffer->m_tCommandBufferSize > 0 );
	m_tCommandBufferSize += _pCommandBuffer->m_tCommandBufferSize;
	m_vecCmdBuffer_.push_back(_pCommandBuffer);
	m_pStrUndoMessage_ = const_cast<std::string*>(&m_vecCmdBuffer_[0]->GetUndoMessage());

	#ifdef _DEBUG_RED_ALERT
	_pCommandBuffer->LogUndoMessage();
	#endif
}

//////////////////////////////////////////////////////////////////////////////
///
/// Object New
CObjectNewCmdBuffer::CObjectNewCmdBuffer(rs3::RToolSceneManager* pSceneManager, rs3::RSceneNode* _pSceneNode)
:	m_strUndoMessage_("새로운 물체생성 편집")
,	m_pSceneManager( pSceneManager )
{
	m_pSceneNode_ = _pSceneNode;
	m_tCommandBufferSize = 20;
}
CObjectNewCmdBuffer::~CObjectNewCmdBuffer(){}

VIEW_MESSAGE::ID CObjectNewCmdBuffer::Undo(CControlMediator* _pController)
{
	// detach logic object
	m_pSceneManager->DetachLogicObject( m_pSceneNode_ );

	ASSERT(m_pSceneNode_);
	m_pSceneNode_->RemoveFromParent();
// 	CControlSceneProperty *pProperty = (CControlSceneProperty*)m_pSceneNode_->GetProperty();
// 	m_pSceneNode_->SetProperty(NULL);
// 	SAFE_DELETE(pProperty);
	SAFE_DELETE(m_pSceneNode_);

	return VIEW_MESSAGE::UNDO_OBJ_NEW_DEL;
}

void CObjectNewCmdBuffer::LogUndoMessage()
{
	if (m_pSceneNode_)
	{
		rs3::RVector vPos = m_pSceneNode_->GetPosition();
		mlog("RSceneNode(%s) New Operation did at %.3f, %.3f, %.3f\n", m_pSceneNode_->GetNodeName().c_str(), vPos.x, vPos.y, vPos.z);
	}
}

//////////////////////////////////////////////////////////////////////////////
///
/// Object Delete
CObjectDeleteCmdBuffer::CObjectDeleteCmdBuffer(rs3::RSceneNode* _pSceneNode)
:	m_strUndoMessage_("물체 삭제 편집")
{
	m_pSceneNode_ = _pSceneNode;
	m_tCommandBufferSize = 20;
}

CObjectDeleteCmdBuffer::~CObjectDeleteCmdBuffer()
{
	if(m_pSceneNode_ != NULL)
	{
// 		CControlSceneProperty *pProperty = (CControlSceneProperty*)m_pSceneNode_->GetProperty();
// 		m_pSceneNode_->SetProperty(NULL);
// 		SAFE_DELETE(pProperty);
		delete m_pSceneNode_;
	}
}

VIEW_MESSAGE::ID CObjectDeleteCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pSceneNode_);
	ASSERT(_pController);

	rs3::RToolSceneManager* pSceneManager = _pController->GetToolSceneManager();
	pSceneManager->AddSceneNode(m_pSceneNode_, true);
	m_pSceneNode_ = NULL;

	return VIEW_MESSAGE::UNDO_OBJ_NEW_DEL;
}

void CObjectDeleteCmdBuffer::LogUndoMessage()
{
	if (m_pSceneNode_)
	{
		rs3::RVector vPos = m_pSceneNode_->GetPosition();
		mlog("RSceneNode(%s) Delete Operation did at %.3f, %.3f, %.3f\n", m_pSceneNode_->GetNodeName().c_str(), vPos.x, vPos.y, vPos.z);
	}
}

//////////////////////////////////////////////////////////////////////////////
///
/// Object Move
CObjectMoveCmdBuffer::CObjectMoveCmdBuffer(rs3::RSceneNode* _pSceneNode, const rs3::RVector& _refRVecOrg)
:	m_strUndoMessage_("물체 이동 편집"),
	m_vPos_(_refRVecOrg)
{
	m_pSceneNode_ = _pSceneNode;
	m_tCommandBufferSize = 5;
}
CObjectMoveCmdBuffer::~CObjectMoveCmdBuffer(){}

VIEW_MESSAGE::ID CObjectMoveCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pSceneNode_);
	m_pSceneNode_->SetPosition(m_vPos_);
	rs3::RSceneNodeUpdateVisitor visitor;
	m_pSceneNode_->Traverse(&visitor);
	return VIEW_MESSAGE::UNDO_MOVE;
}

void CObjectMoveCmdBuffer::LogUndoMessage()
{
	if (m_pSceneNode_)
	{
		rs3::RVector vPos = m_pSceneNode_->GetPosition();
		mlog("RSceneNode(%s) Move Operation did at %.3f, %.3f, %.3f\n", m_pSceneNode_->GetNodeName().c_str(), vPos.x, vPos.y, vPos.z);
	}
}

//////////////////////////////////////////////////////////////////////////////
///
/// Object Scale
CObjectScaleCmdBuffer::CObjectScaleCmdBuffer(rs3::RSceneNode* _pSceneNode, const rs3::RMatrix& _refRMatrixOrg)
:	m_strUndoMessage_("물체 크기변경 편집"),
	m_matOrg_(_refRMatrixOrg)
{
	m_pSceneNode_ = _pSceneNode;
	m_tCommandBufferSize = 5;
}
CObjectScaleCmdBuffer::~CObjectScaleCmdBuffer(){}

VIEW_MESSAGE::ID CObjectScaleCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pSceneNode_);
	m_pSceneNode_->SetTransform(m_matOrg_);
	rs3::RSceneNodeUpdateVisitor visitor;
	m_pSceneNode_->Traverse(&visitor);
	return VIEW_MESSAGE::UNDO_GENERAL;
}

void CObjectScaleCmdBuffer::LogUndoMessage()
{
	if (m_pSceneNode_)
	{
		rs3::RVector vPos = m_pSceneNode_->GetPosition();
		mlog("RSceneNode(%s) Scale Operation did at %.3f, %.3f, %.3f\n", m_pSceneNode_->GetNodeName().c_str(), vPos.x, vPos.y, vPos.z);
	}
}

//////////////////////////////////////////////////////////////////////////////
///
/// Object Rotate
CObjectRotateCmdBuffer::CObjectRotateCmdBuffer(rs3::RSceneNode* _pSceneNode, const rs3::RMatrix& _refMatOrg)
:	m_strUndoMessage_("물체 회전 편집"),
	m_matOrg_(_refMatOrg)
{
	m_pSceneNode_ = _pSceneNode;
	m_tCommandBufferSize = 5;
}

CObjectRotateCmdBuffer::~CObjectRotateCmdBuffer(){}

VIEW_MESSAGE::ID CObjectRotateCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pSceneNode_);
	m_pSceneNode_->SetTransform(m_matOrg_);
	rs3::RSceneNodeUpdateVisitor visitor;
	m_pSceneNode_->Traverse(&visitor);
	return VIEW_MESSAGE::UNDO_GENERAL;
}

void CObjectRotateCmdBuffer::LogUndoMessage()
{
	if (m_pSceneNode_)
	{
		rs3::RVector vPos = m_pSceneNode_->GetPosition();
		mlog("RSceneNode(%s) Rotation Operation did at %.3f, %.3f, %.3f\n", m_pSceneNode_->GetNodeName().c_str(), vPos.x, vPos.y, vPos.z);
	}
}

//////////////////////////////////////////////////////////////////////////////
///
/// Terrain Height
CTerrainHeightCmdBuffer::CTerrainHeightCmdBuffer(rs3::RToolTerrain* _pTerrain)
:	m_strUndoMessage_("지형 높이변경 편집")
{
	m_pTerrain_ = _pTerrain;
	int nSize = m_pTerrain_->GetWidthMap() * m_pTerrain_->GetHeightMap();

	m_pFloatHeightMapBuffer_ = new float[ nSize ];
	memcpy(m_pFloatHeightMapBuffer_, m_pTerrain_->GetHeightMapPtr(), sizeof(float) * nSize);

	//set resource size
	m_tCommandBufferSize = 100;
}

CTerrainHeightCmdBuffer::~CTerrainHeightCmdBuffer()
{
	if(m_pFloatHeightMapBuffer_ != NULL)
		delete[] m_pFloatHeightMapBuffer_;
}

VIEW_MESSAGE::ID CTerrainHeightCmdBuffer::Undo(CControlMediator* _pController)
{
	int nSize = m_pTerrain_->GetWidthMap() * m_pTerrain_->GetHeightMap();
	memcpy(m_pTerrain_->GetHeightMapPtr(), m_pFloatHeightMapBuffer_, sizeof(float) * nSize);

	int widthCount = m_pTerrain_->GetWidthCount();
	int heightCount = m_pTerrain_->GetHeightCount();
	for ( int h = 0; h < heightCount; ++h )
		for ( int w = 0; w < widthCount; ++w )
			m_pTerrain_->UpdateHeightField( w, h);

	//리소스 정리
	delete[] m_pFloatHeightMapBuffer_;
	m_pFloatHeightMapBuffer_ = NULL;

	return VIEW_MESSAGE::UNDO_GENERAL;
}

void CTerrainHeightCmdBuffer::LogUndoMessage()
{
	mlog("Terrain Height Changed Operation did\n");
}

//////////////////////////////////////////////////////////////////////////////
///
/// Terrain Splatting
CTerrainSplatCmdBuffer::CTerrainSplatCmdBuffer(int _layer, int _width, int _height, BYTE* _pByteSrc)
:	m_strUndoMessage_("지형 스플래팅 편집")
{
	m_nLayer_ = _layer;
	m_pByteOriginalByteAddress_ = _pByteSrc;
	m_pByteSplattingBuffer_ = new BYTE[_width * _height * 4];

	for(int h = 0; h < _height; ++h)
	{
		int size = h * _width * 4;
		BYTE *pDst = &m_pByteSplattingBuffer_[size];
		BYTE *pSrc = &_pByteSrc[size];
		memcpy(pDst, pSrc, 4 * _width);
	}

	//set resource size
	m_tCommandBufferSize = 100;
}

CTerrainSplatCmdBuffer::~CTerrainSplatCmdBuffer()
{
	if(m_pByteSplattingBuffer_ != NULL)
		delete[] m_pByteSplattingBuffer_;
}

VIEW_MESSAGE::ID CTerrainSplatCmdBuffer::Undo(CControlMediator* _pController)
{
	rs3::RToolTerrain* pTerrain = (rs3::RToolTerrain*) _pController->GetToolSceneManager()->GetCurrentTerrain();
	ASSERT(pTerrain);

	BYTE *pByte = pTerrain->GetAlphaTexturePtr(m_nLayer_);
	if( pByte == m_pByteOriginalByteAddress_ )
	{
		DWORD dwWidth = pTerrain->GetAlphaTextureWidth(m_nLayer_);
		DWORD dwHeight = pTerrain->GetAlphaTextureHeight(m_nLayer_);

		for ( int h = 0; h < (int)dwHeight; ++h )
		{
			int size = h * dwWidth * 4;
			BYTE *pDst = &pByte[size];
			BYTE *pSrc = &m_pByteSplattingBuffer_[size];
			memcpy(pDst, pSrc, 4*dwWidth);
		}

		pTerrain->UpdateAlphaTexture(m_nLayer_);
	}

	//리소스 정리
	delete[] m_pByteSplattingBuffer_;
	m_pByteSplattingBuffer_ = NULL;

	return VIEW_MESSAGE::UNDO_GENERAL;
}

void CTerrainSplatCmdBuffer::LogUndoMessage()
{
	mlog("Terrain Splatting Operation did\n");
}