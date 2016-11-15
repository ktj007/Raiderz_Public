#pragma once

#include "GActorObserver.h"
#include "GMonitorPortBase.h"

class GEntityNPC;
class GJob;

class GAIMonitorPort : private GActorObserver, public GMonitorPortBase
{
public:
	GAIMonitorPort(void);
	virtual ~GAIMonitorPort(void);

	static GAIMonitorPort& Inst();
	
	void SetNPC(MUID uidTargetNPC);
	bool IsSetedNPC(MUID uidTargetNPC) const;

	void Update(float fDelta);

	void SendCallbackMessageToViewer(const char* pFormat, ...);
	void SendGameLogToViewer( int nLevel, const char* temp );
	void SendEventMessageToViewer( const char* pFormat, ... );

	void OnJobEnqueue( bool bMissionMode, const GJob* pJob );
	void OnJobEvent( bool bMissionMode,const char* szEvent );

private:
	virtual bool IsEnable() override;
	virtual const wchar_t* GetName() override { return L"AI Monitor"; }
	virtual const char* GetFilePath() override { return "..\\AIMonitor\\AIMonitor.exe"; }
private:
	void RefreshNPCInfo();
	string ExtractNPCInfoForXML(GEntityNPC* pNPC);
	string ExtractJobQueueForXML( const char* pszJobType, const deque<GJob*>& vecJobs );
	string ExtractJobFinishLogForXML( const GJob* pJob );
	string ExtractLogForXML( const char* szType, const char* szText );
	string ExtractGLogForXML( int nLevel, const char* temp );
	string ExtractJobEnqueueLogForXML( bool bMissionMode, const GJob* pJob );
	string ExtractJobEventLogForXML( bool bMissionMode, const char* szEvent );
	string MakeJobCountString();
private:
	// 잡이 완료 되었을 떄 (NPC 전용)
	virtual void OnJobFinished(const GJob* pJob) override;

private:
	MUID m_uidTargetNPC;
	MRegulator m_rgrTick;
};
