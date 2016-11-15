#include "StdAfx.h"
#include "M_ToolCmdMgr.h"
#include "M_CommandBuffer.h"
#include <algorithm>

#include "RSceneNode.h"
#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_ControlSceneProperty.h"
#include "C_StatusBarHandler.h"

#include "VL_SceneNodeList.h"

//CToolCmdMgr Method
CToolCmdMgr::CToolCmdMgr(void)
:	m_bPreMouseCmdL_(false),
	m_bPostMouseCmdL_(false),
	m_bPreMouseCmdM_(false),
	m_bPostMouseCmdM_(false),
	m_strNextUndoMsg_("다음 취소할 편집이 없습니다.")
{
	m_tTotalCmdSize_ = 0;
	m_bMapChanged_ = false;
}

CToolCmdMgr::~CToolCmdMgr(void)
{
}

void CToolCmdMgr::ResetCommandManager()
{
	//CommandBuffer 초기화
	for(std::deque<ICommandBuffer*>::iterator itr = m_dqCommandBuffer_.begin();
		itr != m_dqCommandBuffer_.end(); ++itr)
		delete *itr;

	m_dqCommandBuffer_.clear();
	m_tTotalCmdSize_ = 0;
	m_bMapChanged_ = false;
}

//null 커맨드 기록은 맵이 변경되었다는 사실만 알려주는 역할
void CToolCmdMgr::RecordCommand(ICommandBuffer* _pCommandBuffer)
{
	m_bMapChanged_ = true;

	if(_pCommandBuffer != NULL)
	{
		//최소 사이즈 : 5
		ASSERT( _pCommandBuffer->m_tCommandBufferSize > 4 );

		//check deque for buffer container capacity
		while(true)
		{
			if(m_tTotalCmdSize_ < 1000) // 50여개 정도의 커맨드만 보관
			{
				break;
			}
			else //넘치면 예전것을 앞에서 부터 뽑아 없애준다.
			{
				ICommandBuffer* pCommandBuffer = m_dqCommandBuffer_.front();
				m_tTotalCmdSize_ -= pCommandBuffer->m_tCommandBufferSize;
				delete pCommandBuffer;

				m_dqCommandBuffer_.pop_front();
			}
		}

		//Buffer 처리
		m_tTotalCmdSize_ += _pCommandBuffer->m_tCommandBufferSize;
		m_dqCommandBuffer_.push_back(_pCommandBuffer);
		SetNextUndoMsg_();

		#ifdef _DEBUG_RED_ALERT
		_pCommandBuffer->LogUndoMessage();
		#endif
	}
}

void CToolCmdMgr::UndoCommand()
{
	//Undo할 커맨드가 있는지 여부
	if(m_dqCommandBuffer_.empty())
	{
		CStatusBarHandler::GetInstance()->ShowStatusBarMessage("취소할 편집이 없습니다.");
		return;
	}

	ICommandBuffer* pCommandBuffer = m_dqCommandBuffer_.back();

	m_strCurrentUndoMsg_ = pCommandBuffer->GetUndoMessage() + "이 취소되었습니다. ";
	m_tTotalCmdSize_ -= pCommandBuffer->m_tCommandBufferSize;

	//Undo With Controller
	VIEW_MESSAGE::ID undo_message = pCommandBuffer->Undo(m_pControlMediator_);

	//컴포지트 패턴형식의 언두를 한후 완료시점 UpdateView
	m_pControlMediator_->BroadCastMessage(undo_message);

	delete pCommandBuffer;
	m_dqCommandBuffer_.pop_back();

	//StatusBar를 통해 알리기
	SetNextUndoMsg_();
	m_strFullMsg_ = m_strCurrentUndoMsg_ + m_strNextUndoMsg_;
	CStatusBarHandler::GetInstance()->ShowStatusBarMessage(m_strFullMsg_.c_str());
}

void CToolCmdMgr::SetNextUndoMsg_()
{
	if(m_dqCommandBuffer_.empty())
	{
		m_strNextUndoMsg_ = "다음 취소할 편집이 없습니다.";
		return;
	}

	m_strNextUndoMsg_ = "다음 취소할 편집은 " + m_dqCommandBuffer_.back()->GetUndoMessage() + "입니다.";
}
