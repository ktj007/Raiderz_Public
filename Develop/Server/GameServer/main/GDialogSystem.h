#ifndef _GDIALOG_SYSTEM_H
#define _GDIALOG_SYSTEM_H

class GNPCMgr;
class GDialogInfo;
class GDialogSayInfo;
class GDialogSelectInfo;
class GEntityPlayer;
class GItemAddRemoveInfo;


/// 다이얼로그 시스템
class GDialogSystem
{
public:
	GDialogSystem();
	~GDialogSystem();

	bool Start(GEntityPlayer* pPlayer, int nDialogID);	///< 인터랙션중에 대화 시작
	bool Select(GEntityPlayer* pPlayer, int nSelectID);			///< 선택지 선택
	
private:
	bool Start_Check(GEntityPlayer* pPlayer, int nDialogID, GDialogInfo*& outpDialogInfo);
	bool Start_Exit(GDialogInfo* pDialogInfo, GEntityPlayer* pPlayer);
	void Start_Route(GDialogInfo* pDialogInfo, GEntityPlayer* pPlayer);

	bool Select_Check(GEntityPlayer* pPlayer, int nSelectID, GDialogSelectInfo*& outpSelectInfo);
	bool Select_Exit(GDialogSelectInfo* pSelectInfo, GEntityPlayer* pPlayer);
	void Select_Route(GEntityPlayer* pPlayer );

	bool ApplyItemAddRemove(GEntityPlayer* pPlayer, GItemAddRemoveInfo* pItemAddRemoveInfo);
	bool ApplyInteractionElement(GEntityPlayer* pPlayer, int nDialogID, int nExit);	
};

#endif //_GDIALOG_SYSTEM_H