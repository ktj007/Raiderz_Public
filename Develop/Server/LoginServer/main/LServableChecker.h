#pragma once

#include "SServableChecker.h"
#include "STransData.h"

typedef void (LServerStartCallBack) (void);

class LServableChecker : public SServableChecker
{
public:
	enum SERVABLE_CHECK_PHASE
	{
		SCP_WAIT,
		SCP_STARTING,
		SCP_RUNNING,
	};

private:
	struct SERVER_STATUS_NODE
	{
		SERVER_STATUS_NODE() : nServerID(0), bIsRunning(false), bServable(false) {}

		int nServerID;
		bool bIsRunning;
		bool bServable;
	};
	typedef std::vector<SERVER_STATUS_NODE> LServerStatusNodeVector;

public:
	LServableChecker() : m_ePhase(SCP_WAIT), m_fnServerStartCallback(NULL) {}
	void					Reset();

	void					SetServerStartCallback(LServerStartCallBack fnCallback) { m_fnServerStartCallback = fnCallback; }

	bool					Start(std::vector<TD_SERVER_LIST_NODE>& vecServerListFromMasterServer);
	void					UpdateServerStatusInfoByDB(int nServerID, bool bIsRunning, bool bServable);

	SERVABLE_CHECK_PHASE	GetPhase() { return m_ePhase; }

private:
	SERVABLE_CHECK_PHASE	m_ePhase;
	LServerStatusNodeVector	m_vecServerStatusList;

	LServerStartCallBack*		m_fnServerStartCallback;

	bool					Check(const float fDelta) override;
	bool					CheckPhase_Starting();
	bool					Check_IsAllServerStart();

	bool					CheckPhase_Running();
	bool					Check_IsConnectedMasterServer();
};
