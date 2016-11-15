#ifndef _XMODULE_MESSAGE_QUEUE_H
#define _XMODULE_MESSAGE_QUEUE_H

#include "XModule.h"


class XModuleMessageQueue : public XModule, public MMemPool<XModuleMessageQueue>
{
private:
	DECLARE_ID(XMID_MESSAGE_QUEUE);

	struct XEventQueueNode
	{
		XEvent		Msg;
		unsigned int	nTimeStamp;
		unsigned int	nDelayTime;
	};
	void						DelMsgData(XEventQueueNode* pQueue);
protected:
	list<XEventQueueNode*>	m_Queue;
	virtual void				OnInitialized() {}
	virtual void				OnFinalized()	{}
	virtual void				OnUpdate(float fDelta);
public:
	XModuleMessageQueue(XObject* pOwner=NULL);
	virtual ~XModuleMessageQueue(void);
	void Push(XEvent& msg, unsigned int nDelayTime);
	void Remove(XEventID nEventID);
	void GetEventQueueNode(XEventID nEventID, vector<XEvent>& vecList);
};






#endif	// _XMODULE_PLAYER_INFO_H