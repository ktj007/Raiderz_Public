#include "stdafx.h"
#include "XInteractionInfo.h"

SUITE(InteractionInfo)
{
	class FInteractionInfo
	{
	public:
		FInteractionInfo()
		{

		}
		~FInteractionInfo()
		{

		}

		XInteractionInfo		m_InteractionInfo;
	};


	TEST_FIXTURE(FInteractionInfo, TestInteractionInfo_InitOnEnterField)
	{
		m_InteractionInfo.Init_ForChangeField();

		CHECK_EQUAL(MUID::ZERO, m_InteractionInfo.InteractionTargetUID);
		CHECK_EQUAL(MUID::ZERO, m_InteractionInfo.GetInteractionSearchUID());
		CHECK_EQUAL(MUID::ZERO, m_InteractionInfo.GetRequestPlayerUID());
		CHECK_EQUAL(MUID::ZERO, m_InteractionInfo.VSPlayerUID);
		

		CHECK_EQUAL(UIID_INVALID, m_InteractionInfo.stTargetActorInfo.nMouseSearchUIID);
		CHECK_EQUAL(UIID_INVALID, m_InteractionInfo.stTargetActorInfo.nSearchUIID);
		CHECK_EQUAL(UIID_INVALID, m_InteractionInfo.stTargetActorInfo.nTargetUIID);
		CHECK_EQUAL(false, m_InteractionInfo.stTargetActorInfo.bLockOnTarget);
		CHECK_EQUAL(0, m_InteractionInfo.stTargetActorInfo.nLockOnTime);
		CHECK_EQUAL(0, m_InteractionInfo.stTargetActorInfo.nPostReqTime);
	}

	TEST_FIXTURE(FInteractionInfo, TestInteractionInfo_GetSetSearchUIID)
	{
		const UIID nUIID = 9191;

		CHECK_EQUAL(UIID_INVALID, m_InteractionInfo.GetSearchUIID());

		m_InteractionInfo.SetSearchUIID(nUIID);
		CHECK_EQUAL(nUIID, m_InteractionInfo.GetSearchUIID());
	}

	TEST_FIXTURE(FInteractionInfo, TestInteractionInfo_GetSetTargetUIID)
	{
		const UIID nUIID = 234;

		CHECK_EQUAL(UIID_INVALID, m_InteractionInfo.GetTargetUIID());

		m_InteractionInfo.SetTargetUIID(nUIID);
		CHECK_EQUAL(nUIID, m_InteractionInfo.GetTargetUIID());
	}

	TEST_FIXTURE(FInteractionInfo, TestInteractionInfo_GetSetRequestPlayerUID)
	{
		const MUID uidPlayer = MUID(34,4);

		CHECK_EQUAL(MUID::ZERO, m_InteractionInfo.GetRequestPlayerUID());

		m_InteractionInfo.SetRequestPlayerUID(uidPlayer);
		CHECK_EQUAL(uidPlayer, m_InteractionInfo.GetRequestPlayerUID());
	}

}