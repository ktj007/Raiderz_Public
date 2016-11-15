#ifndef _G_PLAYER_INTERACTION_H_
#define _G_PLAYER_INTERACTION_H_

#include "MTime.h"
#include "GNPCInteractionInfo.h"

class GEntityPlayer;
class GEntityNPC;
class GItem;
class GDialogSayInfo;

enum ITRIGGER_TYPE;
enum ITARGET_TYPE;

class GPlayerInteraction : public MTestMemPool<GPlayerInteraction>
{
private:
	GEntityPlayer*				m_pOwner;

	ITRIGGER_TYPE	m_ITriggerType;				///< 인터랙션을 하게된 계기의 종류
	ITARGET_TYPE		m_ITargetType;				///< 인터랙션중인 대상의 종류
	MUID						m_ITargetUID;				///< 인터랙션중인 대상의 UID
	MUID						m_IUseItemUID;				///< 인터랙션을 위해 사용한 아이템 UID

	map<int, GNPCInteractionElementInfo*>	m_mapSelectableElementInfo;	///< 선택 가능한 인터랙션 엘리먼트
	GNPCInteractionElementInfo				m_SelectedIElementInfo;		///< 선택된 인터랙션 엘리먼트

	int							m_nDilaogID;				///< 대화중인 다이얼로그 ID
	GDialogSayInfo*				m_pDialogSayInfo;			///< 대화중인 정보

	MRegulator					m_SleepRegulator;				///< IET_INN_SLEEP 소요시간 타이머	
	MRegulator					m_InteractQObjectiveRegulator;	///< IET_QUEST_INTERACT 소요시간 타이머	

public:
	GPlayerInteraction(GEntityPlayer* pOwner);
	~GPlayerInteraction();

	void Update(float fDelta);
	
	void BeginInteraction(ITRIGGER_TYPE nITrigType, ITARGET_TYPE nITarType, MUID uidITarget, const vector<GNPCInteractionElementInfo*>& vecIElementInfo, MUID uidUseItem=MUID::ZERO);	
	void SelectIElement(const GNPCInteractionElementInfo& IElementInfo);
	void EndInteraction();	

	void ConsumeIUseItem();

	ITARGET_TYPE GetITargetType()	{ return m_ITargetType; }
	ITRIGGER_TYPE GetITriggerType()	{ return m_ITriggerType; }
	GNPCInteractionElementInfo* GetSelectableIElementInfo(int nElementID);
	GNPCInteractionElementInfo& GetSelectedIElementInfo()	{ return m_SelectedIElementInfo; }
	GEntityNPC* GetInteractingNPC();
	MUID GetInteractingNPCUID();
	int GetInteractingNPCID();
	GItem* GetInteractingItem();

	void BeginDialog(int nDialogID, GDialogSayInfo* pDialogSayInfo);
	void ContinueDialog(GDialogSayInfo* pDialogSayInfo);
	void EndDialog();		
	
	GDialogSayInfo* GetDialogSayInfo()			{ return m_pDialogSayInfo; }
	bool IsDialogExit();
	int GetDialogID() { return m_nDilaogID; }
	
	void BeginCraft(int nRecipeID, float fTime);
	void BeginSleep(float fTime);	
	void BeginInteractQObjectvie(float fTime);
	void CancelInteractQObjectvie();

	bool IsInteractQObjectiveRegulatorActive();
	bool IsGuildStorageInteraction();
};

#endif//_G_PLAYER_INTERACTION_H_