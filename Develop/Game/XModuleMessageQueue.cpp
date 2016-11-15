#include "stdafx.h"
#include "XModuleMessageQueue.h"
#include "XObject.h"
#include "XEventID.h"

XModuleMessageQueue::XModuleMessageQueue(XObject* pOwner) : XModule(pOwner)
{

}

XModuleMessageQueue::~XModuleMessageQueue(void)
{
	// [2007/9/6 isnara] 메세지 큐가 남았다면...
	for (list<XEventQueueNode*>::iterator itor = m_Queue.begin(); itor != m_Queue.end(); ++itor)
	{
		DelMsgData(*itor);
		SAFE_DELETE(*itor);
	}

	m_Queue.clear();
}

void XModuleMessageQueue::OnUpdate(float fDelta)
{
	PFC_THISFUNC;

	if (m_Queue.empty()) return;

	unsigned int nNowTime = XGetNowTime();

	list<XEventQueueNode*> use_list;

	for (list<XEventQueueNode*>::iterator itor = m_Queue.begin(); itor != m_Queue.end(); )
	{
		XEventQueueNode* pNode = (*itor);
		if (nNowTime - pNode->nTimeStamp >= pNode->nDelayTime)
		{
			use_list.push_back(pNode);
			itor = m_Queue.erase(itor);
		}
		else
		{
			++itor;
		}
	}

	if (!use_list.empty())
	{
		for each(XEventQueueNode* pNode in use_list)
		{
			m_pOwner->Event(pNode->Msg);
			delete pNode;
		}
	}
}

void XModuleMessageQueue::Push(XEvent& msg, unsigned int nDelayTime)
{
	XEventQueueNode* pNewNode = new XEventQueueNode();
	pNewNode->Msg = msg;
	pNewNode->nTimeStamp = XGetNowTime();
	pNewNode->nDelayTime = nDelayTime;

	m_Queue.push_back(pNewNode);
}

void XModuleMessageQueue::DelMsgData( XEventQueueNode* pQueue )
{
	// 이 함수는 클래스가 삭제될때 실행되는 함수이다.
	// 메모리 릭이 생길 수 있는 원인이 되므로
	// 각 메세지마다 포인터를 가진 데이터라면 여기서 삭제한다.
	if(pQueue)
	{
		switch(pQueue->Msg.m_nID)
		{
		case XEVTD_TALENT_HIT:
			SAFE_DELETE(pQueue->Msg.m_pData);
			break;
		}
	}
}

void XModuleMessageQueue::Remove( XEventID nEventID )
{
	for (list<XEventQueueNode*>::iterator itor = m_Queue.begin(); itor != m_Queue.end(); )
	{
		XEventQueueNode* pNode = (*itor);
		if (pNode->Msg.m_nID == nEventID)
		{
			delete pNode;
			itor = m_Queue.erase(itor);
		}
		else
		{
			++itor;
		}
	}
}

void XModuleMessageQueue::GetEventQueueNode( XEventID nEventID, vector<XEvent>& vecList )
{
	for (list<XEventQueueNode*>::iterator itor = m_Queue.begin(); itor != m_Queue.end(); ++itor)
	{
		XEventQueueNode* pNode = (*itor);
		if(pNode->Msg.m_nID == nEventID && pNode->Msg.m_pData != NULL)
			vecList.push_back(pNode->Msg);
	}
}
