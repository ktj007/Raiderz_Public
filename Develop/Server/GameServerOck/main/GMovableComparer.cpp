#include "StdAfx.h"
#include "GMovableComparer.h"
#include "GEntityPlayer.h"
#include "GField.h"
#include "GConst.h"

float GMovableComparer::s_fWorstTime = 0.0f;

GMovableComparer::GMovableComparer(GEntityPlayer* pPlayer)
: m_pPlayer(pPlayer)
, m_fPrevElapsed(0.0f)
{
	m_vecSlices.reserve(100);
	Start();
}

GMovableComparer::~GMovableComparer(void)
{
	Stop();
}

void GMovableComparer::Start()
{
	if (!GConst::TEST_CMD_HANDLER_COMPARER)
		return;

	CreateRecord(m_recordOld);
	m_timerTotal.Start();
}

void GMovableComparer::Stop()
{
	if (!GConst::TEST_CMD_HANDLER_COMPARER)
		return;

	float fElapsedTime = (float)m_timerTotal.Check();
	Record recordNew;
	CreateRecord(recordNew);
	Report(fElapsedTime, m_recordOld, recordNew);
}

void GMovableComparer::SaveTimeSlice( const wchar_t* pszName )
{
	if (!GConst::TEST_CMD_HANDLER_COMPARER)
		return;

	float fElapsedTime = (float)m_timerTotal.Check();
	m_vecSlices.push_back(make_pair(fElapsedTime-m_fPrevElapsed, pszName));
	m_fPrevElapsed = fElapsedTime;
}

void GMovableComparer::CreateRecord(Record& record)
{
	if (!GConst::TEST_CMD_HANDLER_COMPARER)
		return;

	record.ptSector = m_pPlayer->GetSectorPoint();
	record.vPos = m_pPlayer->GetPos();

	GVectorMUID vecNPC;	
	m_pPlayer->GetNeighborSectorNPC(vecNPC.Vector());
	record.nNPCQty = vecNPC.Vector().size();

	GVectorMUID vecPlayer;
	m_pPlayer->GetNeighborSectorPlayer(vecPlayer.Vector());
	record.nPlayerQty = vecPlayer.Vector().size();
}

void GMovableComparer::Report( float fElapsedTime, const Record& recordOld, const Record& recordNew )
{
	if (!GConst::TEST_CMD_HANDLER_COMPARER)
		return;

	if (s_fWorstTime > fElapsedTime)
		return; // ÃÖ¾ÇÀÇ ½Ã°£ÀÌ ¾Æ´Ô

	s_fWorstTime = fElapsedTime;

	mlog("MovableComparer Report (elasped: %.1f ms)------------\n", fElapsedTime);
	if (recordOld.ptSector != recordNew.ptSector)
	{
		mlog("\tsector changed. (%d,%d) -> (%d,%d)\n", 
			recordOld.ptSector.x, recordOld.ptSector.y, recordNew.ptSector.x, recordNew.ptSector.y);
	}

	mlog("\tNPC: old(%d), new(%d)\n", recordOld.nNPCQty, recordNew.nNPCQty);
	mlog("\tPlayer: old(%d), new(%d)\n", recordOld.nPlayerQty, recordNew.nPlayerQty);
	mlog("\tPosition: old(%.1f %.1f %.1f), new(%.1f %.1f %.1f)\n", 
		recordOld.vPos.x, recordOld.vPos.y, recordOld.vPos.z,
		recordNew.vPos.x, recordNew.vPos.y, recordNew.vPos.z);
	mlog("\tTimeSlice List\n");
	for each (pair<float,wstring> each in m_vecSlices)
	{
		mlog("\t\t%s: %.1fms\n", MLocale::ConvUTF16ToAnsi(each.second.c_str()).c_str(), each.first);
	}
}

