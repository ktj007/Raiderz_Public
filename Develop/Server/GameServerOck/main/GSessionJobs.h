#pragma once

#include "GJob.h"

class GSessionJob : public GJob
{
public:
	GSessionJob(): GJob(NULL) {}

	virtual bool OnStart() override { return true; }

	virtual	 bool IsSessionJob() override	{ return true; }
};

class GJobS_Blocking: public GSessionJob, public MTestMemPool<GJobS_Blocking>
{
public:
	GJobS_Blocking(bool bEnable): m_bEnable(bEnable) {}

	DECLARE_JOB_ID(GJOB_SESSION_BLOCKING);

	bool IsEnable() { return m_bEnable; }
	virtual string GetParamString() const override 
	{ 
		return m_bEnable?"Enable = true":"Enable = false"; 
	}

private:
	bool m_bEnable;
};

class GJobS_Wait: public GSessionJob, public MTestMemPool<GJobS_Wait>
{
public:
	GJobS_Wait(float fDuration): m_fWaitTime(fDuration) {}

	DECLARE_JOB_ID(GJOB_SESSION_WAIT);

	float GetWaitTime() { return m_fWaitTime; }
	virtual string GetParamString() const override 
	{ 
		char buff[512];
		sprintf(buff, "WaitTime: %f", m_fWaitTime);
		return buff;
	}
private:
	float m_fWaitTime;
};

class GJobS_AddNPC: public GSessionJob, public MTestMemPool<GJobS_AddNPC>
{
public:
	GJobS_AddNPC(MUID uidNPC): m_uidNPC(uidNPC) {}

	DECLARE_JOB_ID(GJOB_SESSION_ADD_NPC);

	MUID GetNPCUID() { return m_uidNPC; }

	virtual string GetParamString() const override 
	{ 
		char buff[512];
		sprintf(buff, "uidNPC: %I64d", m_uidNPC.Value);
		return buff;
	}
private:
	MUID m_uidNPC;
};

class GJobS_RemoveNPC: public GSessionJob, public MTestMemPool<GJobS_RemoveNPC>
{
public:
	GJobS_RemoveNPC(MUID uidNPC): m_uidNPC(uidNPC) {}

	DECLARE_JOB_ID(GJOB_SESSION_REMOVE_NPC);

	MUID GetNPCUID() { return m_uidNPC; }

	virtual string GetParamString() const override 
	{ 
		char buff[512];
		sprintf(buff, "uidNPC: %I64d", m_uidNPC.Value);
		return buff;
	}
private:
	MUID m_uidNPC;
};

class GJobS_EndSession: public GSessionJob, public MTestMemPool<GJobS_EndSession>
{
public:
	GJobS_EndSession() {}

	DECLARE_JOB_ID(GJOB_SESSION_END);

	virtual string GetParamString() const override 
	{ 
		return "";
	}
};

class GJobS_ChangeState: public GSessionJob, public MTestMemPool<GJobS_ChangeState>
{
public:
	GJobS_ChangeState(string strState): m_strState(strState) {}

	DECLARE_JOB_ID(GJOB_SESSION_CHANGE_STATE);

	string GetStateName() { return m_strState; }

	virtual string GetParamString() const override 
	{ 
		return m_strState;
	}
private:
	string m_strState;
};

class GJobS_MakePeace: public GSessionJob, public MTestMemPool<GJobS_MakePeace>
{
public:
	GJobS_MakePeace() {}

	DECLARE_JOB_ID(GJOB_SESSION_MAKE_PEACE);

	virtual string GetParamString() const override 
	{ 
		return "";
	}
};

class GJobS_MakeCombat: public GSessionJob, public MTestMemPool<GJobS_MakeCombat>
{
public:
	GJobS_MakeCombat() {}

	DECLARE_JOB_ID(GJOB_SESSION_MAKE_COMBAT);

	virtual string GetParamString() const override 
	{ 
		return "";
	}
};
