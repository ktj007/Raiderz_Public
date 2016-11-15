#ifndef _XNPCICON_EFFECT_H
#define	_XNPCICON_EFFECT_H

#define ICON_EFFECT_TRIGGER				L"ef_head_icon_switch"
#define ICON_EFFECT_ITEM_UNUSABLE		L"ef_head_icon_unuse"
#define ICON_EFFECT_ITEM_USABLE			L"ef_head_icon_use"
#define ICON_EFFECT_HELP				L"ef_head_icon_h00"
#define ICON_EFFECT_GUILD_CREATE		L"ef_head_icon_l00"
#define ICON_EFFECT_TALK				L"ef_head_icon_l00"
#define ICON_EFFECT_QUEST_NOT_BEGIN		L"ef_head_icon_q99"		//퀘스트 있으나 받지 못함
#define ICON_EFFECT_QUEST_BEGIN			L"ef_head_icon_q00"		//퀘스트 있음
#define ICON_EFFECT_QUEST_INTERACTION	L"ef_head_icon_q01"		//퀘스트 인터랙션 하기
#define ICON_EFFECT_QUEST_ACCEPT		L"ef_head_icon_q01_1"	//퀘스트 받음. -> CONTINUE로
#define ICON_EFFECT_QUEST_END			L"ef_head_icon_q02"		//퀘스트 완료
#define ICON_EFFECT_QUEST_CONTINUE		L"ef_head_icon_q02_1"	//퀘스트 진행중
#define ICON_EFFECT_QUEST_REWARD		L"ef_head_icon_q03"		//보상 받음.
#define ICON_EFFECT_COMBAT				L"ef_head_icon_s00"
#define ICON_EFFECT_CRAFT				L"ef_head_icon_meister"
#define ICON_EFFECT_SHOP				L"ef_head_icon_store"
#define ICON_EFFECT_CUTSCENE			L"ef_head_icon_cutscene"
#define ICON_EFFECT_GUARD_DISABLE		L"ef_head_icon_nguard01" // 가드 불가 이펙트


class XNPCIConEffect
{
protected:
	MUID			m_uidEffect;
	void			CreateNPCIConEffect(XObject * pObject, NPC_ICON_STATE NIT);

public:
	XNPCIConEffect();
	virtual ~XNPCIConEffect();

	void			SetNPCICon(XObject * pObject, NPC_ICON_STATE NIS);
	void			Destroy(XObject * pObject=NULL);
	MUID			CreateEffect(XObject* pObject, wstring& strEffectName, bool	bLaseframe);
};

#endif // _XNPCICON_EFFECT_H
