#include "stdafx.h"
#include "XGameInput.h"

SUITE(GameInput)
{
	class XActionKeyDef_ForTest : public XActionKeyDef
	{
	public:
		using XActionKeyDef::DefSeqKey;
	};
	TEST(TestActionKeyDef_SeqKey)
	{
		XActionKeyDef_ForTest		actionKeyDef;

		actionKeyDef.DefSeqKey(SPECIAL_KEY_ADVANCE_ATTACK, 2, VKEY_FORWARD, VKEY_ACTION);

		CHECK_EQUAL(actionKeyDef.GetTable(0).sequenceKeys.size(), 2);
		CHECK_EQUAL(actionKeyDef.GetTable(0).sequenceKeys[0], VKEY_FORWARD);
		CHECK_EQUAL(actionKeyDef.GetTable(0).sequenceKeys[1], VKEY_ACTION);
	}

	TEST(TestSpecialActionTransition)
	{
//		XSpecialActionTable m_Table;
	}
}