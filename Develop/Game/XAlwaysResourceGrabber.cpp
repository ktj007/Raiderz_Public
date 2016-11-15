#include "stdafx.h"
#include "XAlwaysResourceGrabber.h"
#include "XResourceRefMgr.h"

void XAlwaysResourceGrabber::Grab()
{
	ReserveAddRef(FILENAME_MINIBAR_HP);
	ReserveAddRef(FILENAME_MINIBAR_BAR);
	ReserveAddRef(FILENAME_EFFECT_TRACE_PARTICLE);

	ReserveAddRef(L"data/model/player/hf/hf.elu");
	ReserveAddRef(L"data/model/player/hm/hm.elu");

	ReserveAddRef(L"data/model/NPC/hf_adult/hf_adult.elu");
	ReserveAddRef(L"data/model/NPC/hm_adult/hm_adult.elu");
}

