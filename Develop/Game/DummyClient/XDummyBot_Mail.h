#pragma once

#include "XDummyBot.h"
#include "XMailbox.h"

class XDummyBot_Mail : public XDummyBot
{
	enum PHASE
	{
		PHASE_READY,
		PHASE_MOVE_TO_MAILBOX,
		PHASE_OPEN_MAILBOX,
		PHASE_WAIT_TO_OPEN_MAILBOX,
		PHASE_CLEAR_MAILBOX,
		PHASE_WAIT_TO_CLEAR_MAILBOX,
		PHASE_START,

		PHASE_READ_MAIL,
		PHASE_GET_APPENDED_GOODS,
		PHASE_DELETING_MAIL,

		PHASE_WRITE_MAIL,
		PHASE_PUTUP_APPENDING_ITEM,
		PHASE_POST_MAIL,
		PHASE_WAIT_POST_MAIL_RES,
		PHASE_WAIT_RECV_MAIL,

		PHASE_NO_PARTNER,
	};

public:
	XDummyBot_Mail(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_Mail(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	virtual const wchar_t* _GetName() { return GetName(); }
	static const wchar_t* GetName() { return L"Mail"; }

	PHASE GetPhase() { return m_nPhase; }

protected:
	virtual void OnRun(float fDelta) override;

private:
	MRegulator			m_rgltTimeout;
	PHASE				m_nPhase;

	int					m_nTestItemID;
	XMyInventory		m_Inventory;
	int					m_nMyMoney;
	XDummyBot_Mail*		m_pTargetBot; 	// 편지받을 대상 봇

	static bool			m_bSpawnMailboxNPC;
	
	//////////////////////////////////////////////////////////////////////////
	int					m_MailboxID;
	MUID				m_MailboxUID;
	UIID				m_MailboxUIID;
	vec3				m_MailboxPos;

	XMailbox			m_Mailbox;
	XMail*				m_pTestMail;
	//////////////////////////////////////////////////////////////////////////

	int					m_nPostCount;
	

	void ChangePhase(PHASE nPhase) { m_nPhase = nPhase; }

	void Init();
	bool CheckInitialized();
	bool CheckGetAllAppendedGoods();
	void ReadFirstMail();
	bool IsEvenNumberAgent();
	XDummyBot_Mail* GetTraderBot();
};
