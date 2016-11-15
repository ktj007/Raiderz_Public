#pragma once

class GNPCIETextMaker;
class GNPCIEApplier;
class GNPCIEChecker;
class GEntityPlayer;
class GEntityNPC;
class GNPCInteractionElementInfo;
class GItem;


enum ITRIGGER_TYPE;
enum ITARGET_TYPE;
enum INTERACTION_TYPE;

class GNPCInteractor : public MTestMemPool<GNPCInteractor>
{
private:	
	GNPCIETextMaker*	m_pNPCIETextmaker;
	GNPCIEApplier*		m_pNPCIEApplier;
	GNPCIEChecker*		m_pNPCIEChecker;

private:
	void RouteInteractableIElement(GEntityPlayer* pPlayer, const MUID& uidNPC, const wchar_t* szOpeningText, const vector<GNPCInteractionElementInfo*>& vecIElementInfo);	

public:
	GNPCInteractor(void);
	~GNPCInteractor(void);

	bool InteractionByItem(GEntityPlayer* pPlayer, GEntityNPC* pNPC, MUID uidUseItem);
	bool InteractionByClick(GEntityPlayer* pPlayer, GEntityNPC* pNPC);
	bool Interaction(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GNPCInteractionElementInfo*>& vecIElementInfo, ITRIGGER_TYPE nITrigType, MUID uidUseItem=MUID::ZERO);

	bool InteractionElement(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nIElementID);
	bool CheckITarget(GEntityPlayer* pPlayer);
	bool End(GEntityPlayer* pPlayer);
	bool End(GEntityNPC* pNPC);	

public :
	virtual bool CheckIProgress(GEntityPlayer* pPlayer, INTERACTION_TYPE nIET);	
};
