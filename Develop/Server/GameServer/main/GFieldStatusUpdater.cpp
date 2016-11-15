#include "stdafx.h"
#include "GFieldStatusUpdater.h"
#include "GGlobal.h"
#include "GFieldMgr.h"
#include "GSharedFieldMaster.h"
#include "GDBManager.h"



GFieldStatusUpdater::GFieldStatusUpdater() : m_Rglt(3600)
{

}

void GFieldStatusUpdater::Update( const float fDelta )
{
	if (!m_Rglt.IsReady(fDelta))
		return;

	map<int, int> mFieldPlayerQty;

	GFieldMgr::SHARED_FIELD_MASTER_MAP& fmm = gmgr.pFieldMgr->GetSharedFieldMap();
	for (GFieldMgr::SHARED_FIELD_MASTER_MAP::iterator out_it = fmm.begin(); out_it != fmm.end(); ++out_it)
	{
		GSharedFieldMaster* pfm = out_it->second;

		SHAREDFIELD_MAP& sm = pfm->GetChannels();
		for (SHAREDFIELD_MAP::iterator in_it = sm.begin(); in_it != sm.end(); ++in_it)
		{
			GSharedField* psf = in_it->second;

			if ((-1 < psf->GetID()) && (0 < psf->GetPlayerQty()))
			{
				map<int, int>::iterator fpq = mFieldPlayerQty.find(psf->GetID());
				if (fpq != mFieldPlayerQty.end())
					fpq->second += psf->GetPlayerQty();
				else
					mFieldPlayerQty.insert(map<int, int>::value_type(psf->GetID(), psf->GetPlayerQty()));
			}			
		}
	}

	vector<pair<int, int>> vec;
	for each (const pair<int, int>& fpq in mFieldPlayerQty)
	{
		vec.push_back(pair<int, int>(fpq.first, fpq.second));
		if (10 == vec.size())
		{
			gsys.pDBManager->FieldPlayerLog(vec);
			vec.clear();
		}
	}

	if (!vec.empty())
		gsys.pDBManager->FieldPlayerLog(vec);
}
