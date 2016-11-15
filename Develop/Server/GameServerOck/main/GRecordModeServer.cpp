#include "StdAfx.h"
#include "GRecordModeServer.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GConfig.h"

#define COMMAND_RECORD_FILE L"record.txt"

//////////////////////////////////////////////////////////////////////////
//
//	GCommandRecroder
//
//////////////////////////////////////////////////////////////////////////

GCommandRecroder::GCommandRecroder()
: m_pOstream(NULL)
, m_fElapsedTime(0.0f)
{
	m_pOstream = new ofstream(COMMAND_RECORD_FILE, ios::out|ios::binary);

	mlog("[[ Command Record Mode ]]\n");
}

GCommandRecroder::~GCommandRecroder()
{
	SAFE_DELETE(m_pOstream);
}

void GCommandRecroder::SerializeCommand( MCommand* pCommand )
{
	VALID(pCommand);

	static char buff[10240];
	int nCommandSize = pCommand->GetSize();
	pCommand->GetData(buff, nCommandSize);
	MUID uidSender = pCommand->m_Sender;
	MUID uidReceiver = pCommand->m_Receiver;

	m_pOstream->write((char*)&m_fElapsedTime, sizeof m_fElapsedTime);
	m_pOstream->write((char*)&nCommandSize, sizeof nCommandSize);
	m_pOstream->write((char*)&uidSender.Value, sizeof uidSender.Value);
	m_pOstream->write((char*)&uidReceiver.Value, sizeof uidReceiver.Value);
	m_pOstream->write(buff, nCommandSize);
}

void GCommandRecroder::OnUpdate( float fDelta )
{
	m_fElapsedTime += fDelta;
}

//////////////////////////////////////////////////////////////////////////
//
//	GCommandReplayer
//
//////////////////////////////////////////////////////////////////////////

GCommandReplayer::GCommandReplayer(GRecordModeServer* pOwner)
: m_pOwner(pOwner)
, m_fElapsedTime(0.0f)
, m_nMaxReplayData(0)
{
	Load();
}

GCommandReplayer::~GCommandReplayer()
{
}

void GCommandReplayer::OnUpdate( float fDelta )
{
	m_fElapsedTime += fDelta;

	if (m_qReplayDatas.empty())
		return;

	ReplayData& data = m_qReplayDatas.front();
	if (data.fElapsedTime <= m_fElapsedTime)
	{
		MCommand* pCommand = data.pCommand;
		float fLastTime = m_qReplayDatas.back().fElapsedTime;
		m_pOwner->GetCommandProfiler().OnRecv(pCommand);
		m_pOwner->GetCommandProfiler().OnCommandBegin(pCommand);
		m_pOwner->OnCommand(pCommand);
		m_pOwner->GetCommandProfiler().OnCommandEnd(pCommand);
		SAFE_DELETE(data.pCommand);
		m_qReplayDatas.pop_front();
		
		mlog("running replay... %3.1f%% remain_cmd: %d, remain_time: %.1f sec\n", 
			float(m_qReplayDatas.size()/m_nMaxReplayData)*100.0f, m_qReplayDatas.size(), 
			fLastTime - m_fElapsedTime);
	}
}

void GCommandReplayer::Load()
{
	ifstream s(COMMAND_RECORD_FILE, ios::in|ios::binary);
	while( true )
	{
		ReplayData data;
		if (!DeserializeCommand(s, data))
			break;

		m_qReplayDatas.push_back(data);
	}

	m_nMaxReplayData = m_qReplayDatas.size();
	mlog("[[ Command Replay Mode (%d cmd) ]]\n", m_nMaxReplayData);
}

bool GCommandReplayer::DeserializeCommand(ifstream& s, ReplayData& data)
{
	static char buff[10240];
	int nCommandSize;
	MUID uidSender;
	MUID uidReceiver;
	if (!s.read((char*)&data.fElapsedTime, sizeof data.fElapsedTime))
		return false;
	s.read((char*)&nCommandSize, sizeof nCommandSize);
	s.read((char*)&uidSender.Value, sizeof uidSender.Value);
	s.read((char*)&uidReceiver.Value, sizeof uidReceiver.Value);
	s.read(buff, nCommandSize);

	data.pCommand = new MCommand();
	data.pCommand->SetData(buff, nCommandSize);
	data.pCommand->SetSenderUID(uidSender);
	data.pCommand->SetReceiverUID(uidReceiver);

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//	GRecordModeServer
//
//////////////////////////////////////////////////////////////////////////

GRecordModeServer::GRecordModeServer(const MNetServerDesc& desc)
: GNetServer(desc)
, m_nLastTime(0)
, m_pRecorder(NULL)
, m_pReplayer(NULL)
{
	AttachCommandProfiler(new MCommandProfiler());

	if (GConfig::m_nCommandRecordingType == COMMANDREC_RECORD)
	{
		m_pRecorder = new GCommandRecroder();
	}
	else if (GConfig::m_nCommandRecordingType == COMMANDREC_REPLAY)
	{
		m_pReplayer = new GCommandReplayer(this);
	}
	else
	{
		DCHECK(false);
	}
}

GRecordModeServer::~GRecordModeServer(void)
{
	SAFE_DELETE(m_pRecorder);
	SAFE_DELETE(m_pReplayer);
}

void GRecordModeServer::OnRun( void )
{
	uint32 nNowTime = gsys.pSystem->GetNowTime();
	
	if (m_nLastTime == 0)
	{
		m_nLastTime = nNowTime;
		return;
	}

	float fDelta = .001f*float(nNowTime-m_nLastTime);
	m_nLastTime = nNowTime;
	OnUpdate(fDelta);

	__super::OnRun();
}

minet::MCommandResult GRecordModeServer::OnCommand( MCommand* pCommand )
{
	if (m_pRecorder)
	{
		m_pRecorder->SerializeCommand(pCommand);
	}
	return __super::OnCommand(pCommand);
}

void GRecordModeServer::OnUpdate( float fDelta )
{
	if (m_pRecorder)
		m_pRecorder->OnUpdate(fDelta);

	if (m_pReplayer)
		m_pReplayer->OnUpdate(fDelta);
}

