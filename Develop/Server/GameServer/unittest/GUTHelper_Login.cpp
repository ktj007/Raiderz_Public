#include "stdafx.h"
#include "GUTHelper_Login.h"
#include "GPlayerGameData.h"
#include "GCommandTable.h"
#include "GEntityPlayer.h"

MCommand* GUTHelper_Login::NewCommand_MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER( GEntityPlayer* pEntityPlayer, MUID uidConnectionKey, MUID uidGameServer, MUID uidNetClient )
{
	class GPlayerGameDataForTrans : public GPlayerGameData
	{
	public:
		GPlayerGameDataForTrans() : GPlayerGameData() {}
		virtual ~GPlayerGameDataForTrans() {}

		MCommand* MakeReadyEnterMovePlayerReqCommand(GEntityPlayer* pPlayer, MUID uidConnectionKey, MUID uidGameServer, MUID uidNetClient)
		{
			// CheckEntityPlayer
			if (pPlayer == NULL)
			{
				return NULL;
			}

			// PackTransData	
			PackFromEntityPlayer(pPlayer);

			MUID uidPlayer = pPlayer->GetUID();

			// MakeCommand
			return MakeNewCommand(MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER,
				uidNetClient,
				18,
				NEW_UID(uidConnectionKey),
				NEW_UID(uidGameServer),
				NEW_UID(uidPlayer),
				NEW_BLOB(&m_GateInfo, sizeof(m_GateInfo), 1),
				NEW_BLOB(&m_DynamicFieldEnterPosInfo, sizeof(m_DynamicFieldEnterPosInfo), 1),
				NEW_BLOB(&m_PlayerInfoData, sizeof(m_PlayerInfoData), 1),
				NEW_BLOB(&m_CharaInfoData, sizeof(m_CharaInfoData), 1),
				NEW_BLOB(m_vecEquipData), 
				NEW_BLOB(m_vecItemInstanceData),
				NEW_BLOB(m_vecTalentData),
				NEW_BLOB(m_vecPaletteData),
				NEW_BLOB(m_vecTalentCollTimeSec),
				NEW_BLOB(m_vecBuffRemaindSec),
				NEW_BLOB(m_vecQuestInstanceData),
				NEW_BLOB(m_vecQuestDoneCount),
				NEW_BLOB(m_vecFactionData),
				NEW_BLOB(m_vecSawnCutscene),
				NEW_BLOB(m_vecRecipe)
				);
		}
	};

	GPlayerGameDataForTrans playerGameData;
	MCommand* pNewCommand = playerGameData.MakeReadyEnterMovePlayerReqCommand(pEntityPlayer, uidConnectionKey, uidGameServer, uidNetClient);

	return pNewCommand;
}