#ifndef _CCOMMAND_RESULT_TABLE_H
#define _CCOMMAND_RESULT_TABLE_H

inline bool IsCommandResult_Party(int nResult);
inline bool IsCommandResult_Guild(int nResult);
inline bool IsCommandResult_Msg(int nResult);

enum CCommandResultTable
{
	CR_FAIL										= 0,		// 실패하였습니다.
	CR_SUCCESS									= 1,		// 성공하였습니다.
	
	// SYSTEM
	CR_FAIL_SYSTEM_INVALID_PC_UID				= 11,		// 잘못된 플레이어 UID 입니다.
	CR_FAIL_SYSTEM_INVALID_NPC_UID				= 12,		// 잘못된 NPC UID 입니다.
	CR_FAIL_SYSTEM_INVALID_TALENT_ID			= 13,		// 잘못된 탤런트 ID 입니다.
	CR_FAIL_SYSTEM_INVALID_ITEM_ID				= 14,		// 잘못된 아이템 ID 입니다.
	CR_FAIL_SYSTEM_NOT_PLUS_QUANTITY			= 15,		// 양수 수량이 아닙니다.
	CR_FAIL_SYSTEM_INVALID_SOULBINDING_ID		= 17,		// 잘못된 소울 바인딩 ID 입니다.
	CR_FAIL_SYSTEM_INVALID_FIELD_ID				= 18,		// 잘못된 필드 ID 입니다.
	CR_FAIL_SYSTEM_INVALID_MARKER_ID			= 19,		// 잘못된 마커 ID 입니다.
	CR_FAIL_SYSTEM_INVALID_QUEST_ID				= 20,		// 잘못된 퀘스트 ID 입니다.
	CR_FAIL_SYSTEM_CANT_HAVE_TWO_UNIQUE_ITEM	= 21,		// 유니크 아이템은 한 개만 소유할 수 있습니다.
	CR_FAIL_SYSTEM_EMPTY_DURABILITY				= 22,		// 내구도가 0입니다.
	CR_FAIL_SYSTEM_NOT_LEARNED_TALENT			= 24,		// 배우지 않은 탤런트입니다.
	CR_FAIL_SYSTEM_TARGET_IS_INTERACTING		= 25,		// 상대방이 인터랙션중입니다.
	CR_FAIL_SYSTEM_TARGET_IS_TRADING			= 26,		// 상대방이 거래 중입니다.
	CR_FAIL_SYSTEM_TARGET_IS_LOOTING			= 27,		// 상대방이 루팅 중입니다.
	CR_FAIL_SYSTEM_TARGET_IS_CUTSCENING			= 28,		// 상대방이 컷신 중입니다.
	CR_FAIL_SYSTEM_TARGET_IS_DUELING			= 29,		// 상대방이 결투 중입니다.
	CR_FAIL_SYSTEM_TARGET_IS_NOWCOMBAT			= 30,		// 상대방이 전투 중입니다.
	CR_FAIL_SYSTEM_FEW_INVENTORY				= 31,		// 가방에 여유 공간이 부족합니다.
	CR_FAIL_SYSTEM_OVER_MONEY					= 32,		// 금액의 보유 한도를 초과한 상태입니다.
	CR_FAIL_SYSTEM_NOT_EXIST_ITEM				= 33,		// 존재하지 않는 아이템 입니다.
	CR_FAIL_SYSTEM_TOO_SHORT_NAME_LEN			= 34,		// 이름이 너무 짧습니다.
	CR_FAIL_SYSTEM_TOO_LONG_NAME_LEN			= 35,		// 이름이 너무 깁니다.
	CR_FAIL_SYSTEM_CONTAIN_NUMBER_OR_SPACE		= 36,		// 이름에 숫자나 공백이 들어갈 수 없습니다.
	CR_FAIL_SYSTEM_CONTAIN_INVALID_CHARACTER	= 37,		// 이름에 어려운 글자나 특수문자가 들어갈 수 없습니다.
	CR_FAIL_SYSTEM_MIXING_LANGUAGE				= 38,		// 이름을 영어와 한글을 혼용해서 사용할 수 없습니다.
	CR_FAIL_SYSTEM_NUMBER_ONLY					= 39,		// 이름은 숫자만으로 구성될 수 없습니다.
	CR_FAIL_SYSTEM_NAME_INCLUDE_INVALID_STRING	= 40,		// 이름에 사용할 수 없는 문자가 포함되어 있습니다.
	CR_FAIL_SYSTEM_CONTAIN_ALPHABETLESS			= 41,		// 이름에 특수기호를 사용할 수 없습니다.
	CR_FAIL_SYSTEM_NOT_ENOUGH_MONEY				= 42,		// 소지금이 부족합니다.
	CR_FAIL_SYSTEM_NOT_ENOUGH_STORAGEMONEY		= 43,		// 보관함에 있는 돈이 부족합니다.
	CR_FAIL_SYSTEM_OVER_DEPOSIT_MONEY			= 44,		// 입금 하기에 너무 큰 돈입니다.
	CR_FAIL_SYSTEM_OVER_WITHDRAW_MONEY			= 45,		// 출금 하기에 너무 큰 돈입니다.


	// COMMON
	CR_FAIL_CONNECT								= 999,		// 서버와 연결이 끊어졌습니다.


	// LOGIN
	CR_FAIL_LOGIN_COMMAND_INVALID_VERSION		= 1001,		// 서버와 클라이언트가 버전이 다릅니다.
	CR_FAIL_LOGIN								= 1002,		// 로그인에 실패했습니다.
	CR_FAIL_MOVE_GAME_SERVER					= 1003,		// 서버 이동에 실패했습니다.
	CR_SUCCESS_LOGIN_GAME_SERVER_RETRY			= 1004,		// (재시도 처리)
	CR_FAIL_LOGIN_GAME_SERVER_NOT_FOUND			= 1005,		// 게임 접속에 실패했습니다. 서버를 찾을 수 없습니다.
	CR_FAIL_LOGIN_POST_DB						= 1006,		// 로그인 DB 요청에 실패했습니다.
	CR_FAIL_LOGIN_NOT_EXIST_USER				= 1007,		// 존재하지 않는 사용자입니다.
	CR_FAIL_LOGIN_INVALID_CHAR_INFO				= 1008,		// 선택한 캐릭터 정보가 유효하지 않습니다.
	CR_FAIL_LOGIN_SERVERS_ARE_STARTING			= 1009,		// 서버를 구동 중입니다. 잠시 후 다시 접속해 주시기 바랍니다.
	CR_FAIL_DUPLICATED_PLAYER_LOGIN				= 1010,		// 다른 곳에서 중복으로 로그인하여 연결을 종료합니다.
	CR_INFO_LOGIN_KICKING_DUPLICATED_PLAYER		= 1011,		// 중복으로 접속한 플레이어를 처리 중입니다. 잠시 기다리시면 자동 로그인 처리 됩니다. 몇 분 정도 걸릴 수 있습니다. 게임을 종료하시려면 아래 [닫기] 버튼을 클릭하시기 바랍니다.
	CR_FAIL_LOGIN_WORLD_IS_FULL					= 1012,		// 월드가 포화 상태입니다. 잠시 후 다시 접속해 주시기 바랍니다.
	CR_FAIL_LOGIN_INVALID_PLAYER_WORLD_INFO		= 1013,		// 플레이어의 월드 정보가 올바르지 않습니다. 다시 접속해 주시기 바랍니다.
	CR_FAIL_LOGIN_NOT_STAND_ALONE_SERVER		= 1014,		// Stand-alone 서버가 아닙니다. 접속 인자를 확인해 주시기 바랍니다.


	// FIELD
	CR_FAIL_ENTER_FIELD							= 1101,		// 해당 필드로 이동에 실패했습니다.
	CR_FAIL_ENTER_FIELD_PROCEED_COMBAT			= 1102,		// 해당 필드는 전투가 진행 중이라 진입할 수 없습니다.
	CR_FAIL_ENTER_FIELD_PROCEED_INTERMISION		= 1103,		// 해당 필드는 진입 불가능한 상태라 진입할 수 없습니다.
	CR_FAIL_EXIT_FIELD_PROCEED_COMBAT			= 1104,		// 전투 중에 필드를 나갈 수 없습니다.


	// CHAR
	CR_FAIL_CHAR_EXIST_CHAR_NAME				= 1201,		// 이미 같은 이름이 있습니다.	
	CR_FAIL_CHAR_LIMIT_CHAR_SLOT_COUNT			= 1202,		// 더는 캐릭터를 만들 수 없습니다.
	CR_FAIL_CHAR_ALEADY_RESERVED_CONNECTION		= 1203,		// 이미 게임서버 접속 예약이 되어 있습니다.
	CR_FAIL_CHAR_TIMEOUT						= 1204,		// 캐릭터 선택 처리 요청에 대한 응답이 늦어져 실패처리 되었습니다.	
	CR_FAIL_CHAR_INVALID_PARAMETER				= 1205,		// 사용할 수 없는 캐릭터 정보가 있습니다.
	CR_FAIL_CHAR_POST_INSERT_DB					= 1206,		// 캐릭터를 추가하는데 DB에 접근할 수 없습니다.	
	CR_FAIL_CHAR_CANNOT_DELETE_GUILDMASTER_CHAR	= 1207,		// 길드마스터 캐릭터는 삭제 할 수 없습니다. 길드 마스터 권한을 양도 하세요.


	// EQUIP
	CR_FAIL_EQUIP_NEED_CONDITION				= 1301,		// 이 아이템을 장착할 수 있는 조건을 만족하지 않습니다.	|system_male_7
	CR_FAIL_EQUIP_NEED_LICENSE					= 1302,		// 장착하려면 라이센스가 필요합니다.	|system_male_7
	CR_FAIL_EQUIP_INVALID_ITEMSLOT				= 1303,		// 잘못된 슬롯의 아이템입니다.
	CR_FAIL_EQUIP_ALREADY						= 1304,		// 이미 장착한 아이템입니다.


	// TRAINING
	CR_FAIL_TRAINING_CONDITION_CHECK_FAIL		= 1401,		// 해당 탤런트를 배울 수 있는 조건을 만족 시키지 못했습니다.
	CR_FAIL_TRAINING_ALREADY_LEARNED			= 1402,		// 이미 배운 탤런트입니다.
	CR_FAIL_TRAINING_NOT_ENOUGH_TP				= 1403,		// TP가 부족해서 배울 수 없습니다.
	CR_FAIL_TRAINING_NEED_LEVEL					= 1404,		// 레벨이 부족해서 배울 수 없습니다.
	CR_FAIL_TRAINING_NEED_LINE					= 1405,		// 선행 탤런트 라인을 배우지 않아서 배울 수 없습니다.
	CR_FAIL_TRAINING_NEED_STYPE_TP				= 1406,		// 스타일에 투자된 TP가 적어서 배울 수 없습니다.
	CR_FAIL_TRAINING_NOT_LEARNED_TALENT			= 1407,		// 학습한 탤런트가 아닙니다.
	CR_FAIL_TRAINING_EXIST_TNEED_TALENT			= 1408,		// 해당 탤런트가 선행으로 필요한 탤런트가 있습니다.
	CR_FAIL_TRAINING_EXIST_HIGH_RANK_TALENT		= 1409,		// 상위 랭크의 탤런트가 있습니다.
	CR_FAIL_TRAINING_NEED_USEITEM				= 1410,		// 아이템 사용을 통해서만 배울 수 있습니다.
	CR_FAIL_TRAINING_CONSUMETP_ZERO_NOT_UNTRAIN	= 1411,		// TP를 소모하지 않는 탤런트는 지울 수 없습니다.
	CR_FAIL_TRAINING_NOT_UNTRAINABLE_TALENT		= 1412,		// 초기화할 수 없는 탤런트입니다.
	
	
	// TRADE
	CR_FAIL_TRADE_PUTUP_OVER_ITEM				= 1501,		// 소지하고 있는 아이템보다 더 많은 아이템을 올릴 수 없습니다.
	CR_FAIL_TRADE_PUDOWN_OVER_ITEM				= 1502,		// 거래창에 있는 아이템보다 더 많은 아이템을 내릴 수 없습니다.
	CR_FAIL_TRADE_PUTUP_OVER_MONEY				= 1503,		// 가지고 있는 돈보다 더 많은 돈을 거래창에 올릴 수 없습니다.
	CR_FAIL_TRADE_NOT_TRADING					= 1504,		// 현재 거래 중이 아닙니다.
	CR_FAIL_TRADE_OVER_MAX_TRADE_SAPCE			= 1505,		// 거래창에 올릴 수 있는 최대 아이템의 개수보다 더 많이 올릴 수 없습니다.
	CR_FAIL_TRADE_TARGET_DISTANCE				= 1506,		// 거래 하려는 플레이어에게 좀 더 가까이 가야 거래 할 수 있습니다.
	CR_FAIL_TRADE_OPPONENT_IS_BUSY				= 1507,		// 상대방은 거래 요청을 받을 수 없는 상태입니다.
	CR_FAIL_TRADE_PUTUP_NOT_TRADABLE_ITEM		= 1508,		// 거래 불가능한 아이템은 거래창에 올릴 수 없습니다.
	CR_FAIL_TRADE_TARGET_CANT_TAKE_THISITEM		= 1509,		// 상대방의 가방이 부족하거나, 상대방이 받을 수 없는 아이템입니다.
	CR_FAIL_TRADE_TARGET_CANT_TAKE_THISMONEY	= 1510,		// 상대방의 금액보유 한도를 초과하는 돈입니다.

	// NPCSHOP
	CR_FAIL_NPCSHOP_NOT_BUYABLE_ITEM			= 1601,		// 상인에게서 살 수 없는 아이템입니다.
	CR_FAIL_NPCSHOP_SELL_OVER_ITEM				= 1602,		// 상인에게 소지중인 아이템 보다 더 많은 아이템을 팔 수 없습니다.
	CR_FAIL_NPCSHOP_TARGET_DISTANCE				= 1603,		// 상인에게 좀 더 가까이 가야 거래 할 수 있습니다.
	CR_FAIL_NPCSHOP_NOT_EXIST_ITEM_IN_INVEN		= 1604,		// 인벤토리에 없는 아이템은 팔 수 없습니다.
	CR_FAIL_NPCSHOP_NOT_ENOUGH_MONEY			= 1605,		// 돈이 부족합니다.					|system_male_1
	CR_FAIL_NPCSHOP_NOT_NPCTRADING				= 1606,		// 상인과 거래 중이 아닙니다.
	CR_FAIL_NPCSHOP_NOT_MERCHANT				= 1607,		// 상인이 아닌 NPC와 거래 할 수 없습니다.
	CR_FAIL_NPCSHOP_CONDITION_CHECK_FAIL		= 1608,		// 거래 가능한 조건을 만족 시키지 못했습니다.
	CR_FAIL_NPCSHOP_NOT_REPAIRER				= 1609,		// 수리 할 수 있는 상인이 아닙니다.
	CR_FAIL_NPCSHOP_NOT_SELLABLE_ITEM			= 1610,		// 상인에게 팔 수 없는 아이템입니다.


	// PARTY
	CR_BOUNDARY_PARTY_BEGIN,

	CR_FAIL_PARTY_FULL_PARTY							= 1701,		// 파티가 꽉 찼습니다.
	CR_FAIL_PARTY_NOT_PARTY_LEADER						= 1702,		// 파티 리더가 아닙니다.
	CR_FAIL_PARTY_INVALID_PARTY							= 1703,		// 유효하지 않은 파티입니다.
	CR_FAIL_PARTY_NOT_IN_PARTY							= 1704,		// 당신은 파티에 속해있지 않습니다.
	CR_FAIL_PARTY_TARGET_NOT_IN_PARTY					= 1705,		// 상대방은 파티에 속해있지 않습니다.
	CR_FAIL_PARTY_ALEADY_HAS_PARTY						= 1706,		// 당신은 이미 파티가 있습니다.
	CR_FAIL_PARTY_TARGET_ALEADY_HAS_PARTY				= 1707,		// 상대방은 이미 파티가 있습니다.
	CR_FAIL_PARTY_NOT_INVITE_SELF						= 1708,		// 자기 자신을 초대할 수 없습니다.
	CR_FAIL_PARTY_NOT_SAME_PARTY						= 1709,		// 같은 파티가 아닙니다.
	CR_FAIL_PARTY_REFUSE_INVITATION						= 1710,		// 상대방이 파티 요청을 거절하였습니다.
	CR_FAIL_PARTY_REFUSE_INVITATION_FOR_ME				= 1711,		// 상대방이 파티 요청을 거절하였습니다.
	CR_FAIL_PARTY_KICK_SELF								= 1712,		// 자기 자신을 추방시킬 수 없습니다.
	CR_FAIL_PARTY_CHANGE_SAME_LEADER					= 1713,		// 현재 리더는 새로운 리더로 임명할 수 없습니다.
	CR_FAIL_PARTY_OPPONENT_IS_BUSY						= 1714,		// 상대방은 파티 요청을 받을 수 없는 상태 입니다.

	CR_FAIL_PARTY_OPPONENT_IS_DUEL						= 1715,		// 결투 중인 대상을 파티 초대할 수 없습니다.
	CR_FAIL_PARTY_OPPONENT_IN_PVPAREA					= 1716,		// PVP지역에 있는 대상을 파티 초대할 수 없습니다.
	CR_FAIL_PARTY_OPPONENT_IN_BATTLEARENA				= 1717,		// 전장에 있는 대상을 파티 초대할 수 없습니다.

	CR_FAIL_PARTY_REQUESTER_IS_DUEL						= 1718,		// 결투 중일 때는 파티 초대를 할 수 없습니다.
	CR_FAIL_PARTY_REQUESTER_IN_PVPAREA					= 1719,		// PVP지역에 있을때는 파티 초대를 할 수 없습니다.
	CR_FAIL_PARTY_REQUESTER_IN_BATTLEARENA				= 1720,		// 전장에 있을때는 파티 초대를 할 수 없습니다.

	CR_FAIL_PARTY_RESPONDER_IS_DUEL						= 1721,		// 결투 중일 때는 파티 초대에 응답할 수 없습니다.
	CR_FAIL_PARTY_RESPONDER_IN_PVPAREA					= 1722,		// PVP지역에 있을때는 파티 초대에 응답할 수 없습니다.
	CR_FAIL_PARTY_RESPONDER_IN_BATTLEARENA				= 1723,		// 전장에 있을때는 파티 초대에 응답할 수 없습니다.


	CR_FAIL_PARTY_PLAYER_IS_BUSY						= 1724,		// 파티 요청을 할 수 없는 상태입니다.
	CR_FAIL_PARTY_RESPOND_PLAYER_IS_BUSY				= 1725,		// 다른 행위 중에 파티 요청에 응답할 수 없습니다.
	CR_FAIL_PARTY_NOT_INVITE_TO_PLAYER_IN_TRIALFIELD	= 1726,		// 도전 필드에 있는 플레이어는 초대할 수 없습니다.
	CR_FAIL_PARTY_INVALID_SETTING						= 1727,		// 잘못된 파티 설정입니다.

	CR_BOUNDARY_PARTY_END,


	// LOOT
	CR_FAIL_LOOT_NOT_EXIST_DROPITEM				= 1801,		// 획득할 전리품이 없습니다.
	CR_FAIL_LOOT_TARGET_DISTANCE				= 1802,		// 전리품을 획득할 대상에게 좀 더 가까이 가야 합니다.
	CR_FAIL_LOOT_NOT_LOOTING					= 1803,		// 전리품 획득 중이 아닙니다.
	CR_FAIL_LOOT_ALREADY_OTHER_INTERACT_LOOTING	= 1804,		// 이미 다른 사람이 전리품을 획득하고 있습니다.
	CR_FAIL_LOOT_ALREADY_OTHER_GATHERING		= 1805,		// 이미 다른 사람이 채집하고 있습니다.
	CR_FAIL_LOOT_NOT_EXIST_ITEM					= 1806,		// 존재하지 않는 전리품입니다.
	CR_FAIL_LOOT_NOT_MASTERLOOTABLE_ITEM		= 1809,		// 마스터 루팅이 가능한 전리품이 아닙니다.
	CR_FAIL_LOOT_MASTERLOOT_POSSIBLE_ONLY_MASTER= 1810,		// 마스터 루팅은 마스터만 할 수 있습니다.
	CR_FAIL_LOOT_NOT_MASTERLOOTABLE_PLAYER		= 1811,		// 마스터 루팅으로 아이템을 건네줄 수 있는 플레이어가 아닙니다.
	CR_FAIL_LOOT_MASTER_FAR_DISTANCE			= 1812,		// 마스터가 멀리 있어서 아이템을 배분할 수 없습니다.
	CR_FAIL_LOOT_GATHER_NEED_GATHERTOOL			= 1814,		// 채집을 하려면 채집 도구가 필요합니다.			|system_male_9
	CR_FAIL_LOOT_GATHER_NEED_TRAINING			= 1815,		// 채집을 하려면 탐험가 교본을 읽어야 합니다.		|system_male_10
	CR_FAIL_LOOT_GATHER_NEED_HIGHER_TRAINING	= 1816,		// 상위 레벨의 탐험가 교본을 읽어야 합니다.			|system_male_10
		
	
	// MSG
	CR_BOUNDARY_MSG_BEGIN,

	CR_FAIL_MSG_NO_WHISPER_RECEIVER				= 1901,		// 대상이 접속하지 않았거나 잘못된 이름입니다.
	CR_FAIL_MSG_ALREADY_EXIST_CHANNEL_NAME		= 1902,		// 이미 같은 이름의 채널이 있습니다.
	CR_FAIL_MSG_NOT_PRIVATE_CHANNEL				= 1903,		// 현재 채널에서는 사용할 수 없는 명령입니다.
	CR_FAIL_MSG_NOT_HOST						= 1904,		// 방장이 아닙니다.
	CR_FAIL_MSG_NOT_BANED						= 1905,		// 추방당한 플레이어가 아닙니다.
	CR_FAIL_MSG_BANED_CHANNEL					= 1906,		// 입장할 수 없습니다. 추방당한 채널입니다.
	CR_FAIL_MSG_NOT_EXIST_PLAYER				= 1907,		// 해당 플레이어가 없습니다.
	CR_FAIL_MSG_NOT_EXIST_CHANNEL				= 1908,		// 해당 채널이 없습니다.
	CR_FAIL_MSG_ALREADY_ENTERED_CHANNEL			= 1909,		// 이미 입장한 채널입니다.
	CR_FAIL_MSG_NOT_ENTERED_CHANNEL				= 1910,		// 입장하지 않은 채널입니다.
	CR_FAIL_MSG_WRONG_TARGET					= 1911,		// 대상이 올바르지 않습니다.
	CR_FAIL_MSG_OVER_CHATTING_CHANNEL_NAME		= 1912,		// 채널 이름이 너무 깁니다.
	CR_FAIL_MSG_LIMIT_ENTER_PRIVATE_CHANNEL		= 1913,		// 더는 사설 채널에 입장할 수 없습니다.
	CR_FAIL_MSG_ABUSE_MSG						= 1914,		// 부적절한 단어가 포함되어 있습니다.
	CR_FAIL_MSG_LIMIT_AUCTION_MSG_TIME			= 1915,		// 거래 채팅은 1분에 한번만 사용 가능합니다.

	CR_BOUNDARY_MSG_END,


	// ITEM
	CR_FAIL_ITEM_NOT_EXIST_IN_INVENTORY			= 2001,		// 인벤토리에 아이템이 없습니다.				|system_male_8
	CR_FAIL_ITEM_USE_NOTENOUGH_COOLDOWN			= 2002,		// 아이템을 사용하려면 조금 더 기다려야 합니다.	|system_male_8
	CR_FAIL_ITEM_NOT_REMOVABLE					= 2003,		// 버릴 수 없는 아이템입니다.
	CR_FAIL_ITEM_NOT_ADD_OVER_MAXSTACK			= 2004,		// 한번에 최대 스택 개수보다 많이 획득 할 수 없습니다.
	CR_FAIL_ITEM_NEED_CONDITION					= 2005,		// 이 아이템은 아직 사용하지 못합니다.			|system_male_8


	// INTERACTION
	CR_FAIL_INTERACTION_NOT_INTRACTABLE_NPC		= 2101,		// 상호작용할 수 없는 NPC 입니다.
	CR_FAIL_INTERACTION_NOT_LIVING_NPC			= 2102,		// 죽은 NPC와는 상호작용 할 수 없습니다.


	// USE_TALENT
	CR_FAIL_USE_TALENT							= 2201,		// 탤런트를 사용할 수 없습니다.					|system_male_2
	CR_FAIL_USE_TALENT_NOTENOUGH_COOLDOWN		= 2202,		// 탤런트를 쓰려면 조금 더 기다려야 합니다.		|system_male_3
	CR_FAIL_USE_TALENT_NOTENOUGH_HP				= 2203,		// 탤런트를 쓰기에 생명력이 충분하지 않습니다.	|system_male_4
	CR_FAIL_USE_TALENT_NOTENOUGH_EN				= 2204,		// 탤런트를 쓰기에 정신력이 충분하지 않습니다.	|system_male_5
	CR_FAIL_USE_TALENT_NOTENOUGH_STA			= 2205,		// 탤런트를 쓰기에 기력이 충분하지 않습니다.	|system_male_6
	CR_FAIL_USE_TALENT_PASSIVE					= 2206,		// 패시브 탤런트는 사용할 수 없습니다.
	CR_FAIL_USE_TALENT_NOFOCUS					= 2207,		// 탤런트를 쓰기에 필요한 포커스가 없습니다.
	CR_FAIL_USE_TALENT_ALREADY_RUNNING			= 2208,		// 이미 탤런트를 사용 중입니다.
	CR_FAIL_USE_TALENT_DISABLED					= 2209,		// 지금은 탤런트를 사용할 수 없습니다.
	CR_FAIL_USE_TALENT_NOTENOUGH_SPEED			= 2210,		// 탤런트를 사용하기에 이동 속도가 충분하지 않습니다.
	CR_FAIL_USE_TALENT_NONCOMBAT_ONLY			= 2211,		// 비전투 중에만 사용할 수 있는 탤런트입니다.
	CR_FAIL_USE_TALENT_GM_ONLY					= 2212,		// 운영자만 사용할 수 있는 탤런트입니다.
	CR_FAIL_USE_TALENT_NOTUSE_IN_THISFIELD		= 2213,		// 현재 필드에서 사용할 수 없는 탤런트입니다.

	// QUEST
	CR_FAIL_QUEST_NOT_REPEATABLE_QUEST			= 2302,		// 반복 할 수 없는 퀘스트 입니다.
	CR_FAIL_QUEST_NOT_HAVE_QUEST				= 2304,		// 수행 중인 퀘스트가 아닙니다.
	CR_FAIL_QUEST_NOT_COMPLETE_QUEST			= 2305,		// 완료한 퀘스트가 아닙니다.
	CR_FAIL_QUEST_NOT_TAKABLE_QUEST				= 2306,		// 받을 수 있는 퀘스트가 아닙니다.
	CR_FAIL_QUEST_IS_FAILED						= 2308,		// 실패한 퀘스트입니다.
	CR_FAIL_QUEST_NOT_SELECTABLE_REWARD			= 2310,		// 선택 가능한 보상이 아닙니다.
	CR_FAIL_QUEST_NOT_CANCELABLE_QUEST			= 2311,		// 취소할 수 없는 퀘스트입니다.
	CR_FAIL_QUEST_NOT_HAVE_REWARD_NEED_ITEM		= 2312,		// 보상에 필요한 아이템이 없습니다.
	CR_FAIL_QUEST_NOT_TAKABLE_SHARE_QUEST		= 2313,		// 공유된 퀘스트를 받을 수 없습니다.

	//////////////////////////////////////////////////////////////////////////
	// GUILD
	CR_BOUNDARY_GUILD_BEGIN,

	CR_FAIL_GUILD_ALREADY_JOIN					= 2401,		// 이미 길드에 가입해 있습니다.
	CR_FAIL_GUILD_EXIST_NAME					= 2402,		// 존재하는 길드명 입니다.	
	CR_FAIL_GUILD_TOO_LONG_NAME					= 2403,		// 길드명이 너무 깁니다.
	CR_FAIL_GUILD_TOO_SHORT_NAME				= 2404,		// 길드명이 너무 짧습니다.
	CR_FAIL_GUILD_CREATE_NOT_ENOUGH_MONEY		= 2405,		// 길드 생성비가 부족 합니다.
	CR_FAIL_GUILD_NO_JOIN						= 2406,		// 가입한 길드가 없습니다.
	CR_FAIL_GUILD_NOT_MASTER					= 2407,		// 길드 마스터만 가능 합니다.
	CR_FAIL_GUILD_NOT_INVITED_GUEST				= 2408,		// 길드 초대를 받은 적이 없습니다.
	CR_FAIL_GUILD_MASTER_CANNOT_LEAVE			= 2409,		// 탈퇴하려면 길드 마스터 권한을 다른이에게 양도해야 합니다.
	CR_FAIL_GUILD_MAX_MEMBER_COUNT				= 2410,		// 최대 길드 인원입니다.
	CR_FAIL_GUILD_NOT_VALID_NAME				= 2411,		// 사용할 수 없는 길드명입니다.
	CR_FAIL_GUILD_GUEST_IS_ENEMY				= 2412,		// 적대적인 대상을 길드에 초대할 수 없습니다.
	CR_FAIL_GUILD_OPPONENT_IS_DUEL				= 2413,		// 결투 중인 대상을 길드 초대할 수 없습니다.
	CR_FAIL_GUILD_OPPONENT_IN_PVPAREA			= 2414,		// PVP지역에 있는 대상을 길드 초대할 수 없습니다.
	CR_FAIL_GUILD_OPPONENT_IN_BATTLEARENA		= 2415,		// 전장에 있는 대상을 길드 초대할 수 없습니다.
	CR_FAIL_GUILD_SELF_IS_DUEL					= 2416,		// 결투 중일 때는 길드 초대에 응답할 수 없습니다.
	CR_FAIL_GUILD_SELF_IN_PVPAREA				= 2417,		// PVP지역에 있을때는 파티 초대에 응답할 수 없습니다.
	CR_FAIL_GUILD_SELF_IN_BATTLEARENA			= 2418,		// 전장에 있을때는 파티 초대에 응답할 수 없습니다.
	CR_FAIL_GUILD_NEED_EMPTY_MEMBER				= 2419,		// 길드원이 남아 있으면 안됩니다.
	CR_FAIL_GUILD_CANNOT_MOVE_BINDITEM_TO_STORAGE	= 2420,	// 귀속된 아이템은 길드 보관함으로 이동할 수 없습니다.
	CR_FAIL_GUILD_CANNOT_MOVE_PERIODITEM_TO_STORAGE	= 2421,	// 기간제 아이템은 길드 보관함으로 이동할 수 없습니다.
	CR_FAIL_GUILD_MEMBER_MODIFY_STORAGE			= 2422,		// 다른 길드원이 길드 보관함을 변경 했습니다.

	CR_BOUNDARY_GUILD_END,
	//////////////////////////////////////////////////////////////////////////

	// DUEL
	CR_FAIL_DUEL_ALREADY_DUELING				= 2501,		// 당신은 이미 결투중입니다.
	CR_FAIL_DUEL_TARGET_IS_DUELING				= 2502,		// 상대가 이미 결투중입니다.
	CR_FAIL_DUEL_INVALID_FIELD					= 2503,		// 결투할 수 없는 장소입니다.
	CR_FAIL_DUEL_DONT_DURING_COMBAT				= 2504,		// 전투 중에는 결투를 신청할 수 없습니다.
	CR_FAIL_DUEL_TARGET_IS_COMBATING			= 2505,		// 상대는 전투 중입니다.
	CR_FAIL_DUEL_YOUR_BUSY						= 2506,		// 다른 행동 중에 결투를 신청할 수 없습니다.
	CR_FAIL_DUEL_OPPONENT_IS_BUSY				= 2507,		// 상대는 결투를 받을 수 없는 상태입니다.
	CR_FAIL_DUEL_OPPONENT_IS_YOU				= 2508,		// 자기 자신과 결투 할 수 없습니다.
	CR_FAIL_DUEL_OPPONENT_IS_ENEMY				= 2509,		// 이미 대상과 적대적입니다.


	// PALETTE
	CR_FAIL_PALETTE_PUTUP_ONLY_ACTIVE_TALENT	= 2606,		// 액티브 탤런트만 팔레트에 등록할 수 있습니다.
	CR_FAIL_PALETTE_PUTUP_ONLY_USABLE_ITEM		= 2607,		// 사용 가능한 아이템만 팔레트에 등록할 수 있습니다.


	// SOUL
	CR_FAIL_SOUL_FULL_AMULET_SOULQUANTITY		= 2650,		// 애뮬렛에 영혼이 다 찼습니다.


	// BATTLEARENA
	CR_FAIL_BATTLEARENA_NEED_PARTY				= 2701,		// 파티가 있어야 투기장에 참여할 수 있습니다.
	CR_FAIL_BATTLEARENA_ALREADY_REGISTER		= 2702,		// 이미 등록된 참가 그룹입니다.
	CR_FAIL_BATTLEARENA_NOT_REGISTER			= 2703,		// 등록이 되지 않은 참가 그룹입니다.
	CR_FAIL_BATTLEARENA_ALREADY_PLAYING			= 2704,		// 대전이 진행 중인 참가 그룹은 참가를 취소할 수 없습니다.


	// MAIL
	CR_FAIL_MAIL_POST_SELF						= 2801,		// 자신에게 편지 보낼 수 없습니다.
	CR_FAIL_MAIL_POST_LIMIT_TITLE				= 2802,		// 제목이 너무 깁니다.
	CR_FAIL_MAIL_POST_LIMIT_TEXT				= 2803,		// 본문이 너무 깁니다.
	CR_FAIL_MAIL_POST_DB_ERROR					= 2804,		// 현재 서버가 편지를 보낼 수 없습니다.
	CR_FAIL_MAIL_POST_DB_FAIL					= 2805,		// 받는 사람이 편지를 받을 수 있는 상태가 아닙니다.
	CR_FAIL_MAIL_POST_DB_CHECK_FAIL				= 2806,		// 받는 사람의 상태를 알 수 없습니다.
	CR_FAIL_MAIL_POST_DB_CHECK_ERROR			= 2807,		// 현재 서버가 받는 사람의 상태를 확인할 수 없습니다.	
	CR_FAIL_MAIL_POST_NOT_FIND_USER				= 2808,		// 받는 사람이 없습니다.
	CR_FAIL_MAIL_POST_NOT_SEND_GM				= 2809,		// GM에게 직접 편지를 보낼 수 없습니다.
	CR_FAIL_MAIL_POST_FULL_BOX					= 2810,		// 받는 사람의 우편함이 꽉찼있습니다.
	CR_FAIL_MAIL_POST_NOT_ENOUGH_MONEY			= 2811,		// 첨부할 돈이 부족합니다.
	CR_FAIL_MAIL_POST_WRONG_APPENDED_ITEM		= 2812,		// 첨부할 수 없는 아이템이 발생했습니다.

	CR_FAIL_MAIL_APPEND_ITEM_NO_INVENTORY		= 2813,		// 인벤토리에 없는 아이템입니다.
	CR_FAIL_MAIL_APPEND_ITEM_NOT_ENOUGH_AMOUNT_UP = 2814,	// 첨부할 수량이 부족합니다.
	CR_FAIL_MAIL_APPEND_ITEM_NOT_ENOUGH_AMOUNT_DOWN = 2815,	// 첨부된 수량이 부족합니다.
	CR_FAIL_MAIL_APPEND_ITEM_BIND				= 2816,		// 귀속 아이템은 첨부할 수 없습니다.
	CR_FAIL_MAIL_APPEND_ITEM_NOT_TRADABLE		= 2817,		// 거래불가 아이템은 첨부할 수 없습니다.
	CR_FAIL_MAIL_APPEND_ITEM_PERIOD				= 2818,		// 기간제 아이템은 첨부할 수 없습니다.
	CR_FAIL_MAIL_APPEND_ITEM_BUSY				= 2819,		// 현재 다른 기능에서 사용 중인 아이템은 첨부할 수 없습니다.
	CR_FAIL_MAIL_APPEND_ITEM_NOT_INTERATION		= 2820,		// 우편함 인터랙션 중이 아닙니다.
	
	CR_FAIL_MAIL_TAKE_DB_ERROR					= 2821,		// 현재는 서버가 첨부내용을 받아올 수 없습니다.
	CR_FAIL_MAIL_NOT_FIND_MAIL					= 2822,		// 해당 편지를 찾을 수 없습니다.
	CR_FAIL_MAIL_NO_APPENDED_MONEY				= 2823,		// 첨부된 게임머니가 없습니다.

	CR_FAIL_MAIL_POST_NOT_ENOUGH_POSTAGE		= 2824,		// 우편요금이 모자릅니다.
	CR_FAIL_MAIL_APPEND_ITEM_ENCHANT			= 2825,		// 강화된 아이템은 첨부할 수 없습니다.
	CR_FAIL_MAIL_NOT_ENOUGH_MONEY_STORAGE		= 2826,		// 최대 소지금 제한으로 인해 첨부 게임머니를 받을 수 없습니다.


	// ENCHANT
	CR_FAIL_ENCHANT_REQ_NOTENOUGH_LEVEL			= 2901,		// 강화실패: 레벨이 부족합니다.
	CR_FAIL_ENCHANT_REQ_UNABLE_EQUIP_PART		= 2902,		// 강화실패: 강화 할 수 없는 장비 부위입니다.
	CR_FAIL_ENCHANT_REQ_UNABLE_WEAPON			= 2903,		// 강화실패: 강화 할 수 없는 무기입니다.
	CR_FAIL_ENCHANT_REQ_NOTENOUGH_SLOT			= 2904,		// 강화실패: 강화할 수 있는 슬롯이 없습니다.
	CR_FAIL_ENCHANT_REQ_WRONG_AGENT				= 2905,		// 강화실패: 알맞지 않은 강화제입니다.
	CR_FAIL_ENCHANT_REQ_ITEM_BUSY				= 2906,		// 강화실패: 현재 다른 기능에서 사용 중인 아이템은 첨부할 수 없습니다.


	// ENTERING WORLD
	CR_ENTER_WORLD_FIND_GAMESERVER				= 3001,		// 입장할 게임 서버를 찾습니다.
	CR_ENTER_WORLD_START_MOVING_TO_GAMESERVER	= 3002,		// 게임 서버로 이동을 시작합니다.
	CR_ENTER_WORLD_CONNECT_TO_GAMESERVER		= 3003,		// 게임 서버에 접속합니다.
	CR_ENTER_WORLD_LOGIN_GAMESERVER				= 3004,		// 인증 처리 중입니다.
	CR_ENTER_WORLD_SUCCESS_LOGIN_GAMESERVER		= 3005,		// 인증 성공.
	CR_ENTER_WORLD_GAMESTART					= 3006,		// 월드에 입장합니다.


	// MOVE TO SELECT CHAR
	CR_MOVE_TO_SELECT_CHAR_INVALID_STATE		= 3101,		// 캐릭터 선택 화면으로 이동할 수 없는 상태입니다.
	CR_MOVE_TO_SELECT_CHAR_BUSY					= 3102,		// 다른 행동 중에 캐릭터 선택 화면으로 빠져나갈 수 없습니다.
	CR_MOVE_TO_SELECT_CHAR_DONT_DURING_COMBAT	= 3103,		// 전투 중에 캐릭터 선택 화면으로 빠져나갈 수 없습니다.
	CR_MOVE_TO_SELECT_CHAR_TIMEOUT				= 3104,		// 처리시간이 초과되어 요청이 취소되었습니다.


	// CHANGE CHANNEL
	CR_FAIL_CHANGE_CHANNEL_INVALID_STATE		= 3201,		// 채널을 변경할 수 없는 상태입니다.
	CR_FAIL_CHANGE_CHANNEL_BUSY					= 3202,		// 다른 행동 중에 채널을 변경할 수 없습니다.
	CR_FAIL_CHANGE_CHANNEL_DONT_DURING_COMBAT	= 3203,		// 전투 중에 채널을 변경할 수 없습니다.
	CR_FAIL_CHANGE_CHANNEL_IS_FULL				= 3204,		// 해당 채널은 포화 상태입니다.
	CR_FAIL_CHANGE_CHANNEL_IS_INVALID			= 3205,		// 존재하지 않는 채널입니다.
	CR_FAIL_CHANGE_CHANNEL_SAME_CHANNEL			= 3206,		// 지금 있는 채널로 이동할 수 없습니다.
	CR_FAIL_CHANGE_CHANNEL_UNABLE_PLACE			= 3207,		// 채널이동을 할 수 없는 장소입니다.
	CR_FAIL_CHANGE_CHANNEL_PROCEED				= 3208,		// 채널이동이 진행 중입니다.
	CR_FAIL_CHANGE_CHANNEL_INVALID_PREPARE_TIME	= 3209,		// 채널이동을 하기에 유효하지 않은 요청시각입니다.

	// STORAGE
	CR_FAIL_STORAGE_CANNOT_MOVE_PERIODITEM_TO_STORAGE= 3301,// 기간제 아이템은 보관함으로 이동할 수 없습니다.

	// DEBUG
	CR_FAIL_DEBUG_MIN							= 10000,	// 여기서부터는 디버그일 경우 서버에서 보내지 않음 --------------------------------------
	CR_FAIL_TALENTCANCEL_REQUEST				= 10001,	// D)요청에 의해 탤런트가 취소되었습니다.
	CR_FAIL_ITEMTALENTCANCEL_REQUEST			= 10002,	// D)요청에 의해 아이템 탤런트가 취소되었습니다.
	CR_FAIL_TALENTCANCEL_COOLTIMESYNCH			= 10003,	// D)이동모팩중이라 탤런트가 취소되었습니다.
	CR_FAIL_TALENTCANCEL_MOVE					= 10004,	// D)탤런트 진행 중에 이동하여 취소되었습니다.
};


bool IsCommandResult_Party(int nResult)
{
	return (CR_BOUNDARY_PARTY_BEGIN < nResult && nResult < CR_BOUNDARY_PARTY_END);
}
bool IsCommandResult_Guild(int nResult)
{
	return (CR_BOUNDARY_GUILD_BEGIN < nResult && nResult < CR_BOUNDARY_GUILD_END);
}
bool IsCommandResult_Msg(int nResult)
{
	return (CR_BOUNDARY_MSG_BEGIN < nResult && nResult < CR_BOUNDARY_MSG_END);
}

//////////////////////////////////////////////////////////////////////////

enum CCommandResultTable_MasterServer
{
	CRM_FAIL									= 0,		// 실패하였습니다.
	CRM_SUCCESS									= 1,		// 성공하였습니다.

	CRM_FAIL_LOGIN_COMMAND_INVALID_VERSION		= 100,		// 잘못된 커맨드 버전입니다.
	CRM_FAIL_LOGIN_ACCEPT						= 101,		// 로그인에서 Accept되지 못했습니다.
	CRM_FAIL_LOGIN_INVALID_WORLD_ID				= 102,		// World ID가 다릅니다.
	CRM_FAIL_LOGIN_ALREADY_EXIST				= 103,		// 이미 로그인 돼있는 서버입니다.
};

#endif	// _CCOMMAND_RESULT_TABLE_H