#pragma once
#ifndef _XMODULE_B_PARTS_H
#define _XMODULE_B_PARTS_H

#include "XModule.h"
#include "XBPartActorManager.h"

class XModuleBParts : public XModule, public MMemPool<XModuleBParts>
{
	DECLARE_ID(XMID_BREAKABLEPARTS);

private:

	// 메쉬 로딩 전에 파괴된 파츠 명령어가 왔을때
	struct stWaitBParts
	{
		set<int>		setParts;
		bool			bWait;

		stWaitBParts()
		{
			Init();
		}

		void Init()
		{
			setParts.clear();
			bWait = false;
		}

	} m_WaitBrokenParts ;

	vec3					m_vForceDir[UBD_MAX];

	XBPartActorManager		m_BPartActorManager;

protected:
	virtual void			OnUpdate(float fDelta);							///< 매 틱마다 호출
	virtual void			OnInitialized();
	virtual void			OnFinalized()							{}
	virtual void			OnSubscribeEvent();
	virtual XEventResult	OnEvent(XEvent& msg);

	void					SetBreakablePartsEffect(RActorNode* pBreakablePartsNode, XObject* pObject, wstring strEffectName);

	bool					CheckMature(int nIndex);

	void					_SetBreakableParts(int nIndex, bool bEffect, MUID uidAttacker=MUID::ZERO, float fXYForce=-1.0f, float fZForce=-1.0f, TD_USABLE_BPART* pUsableBpart = NULL, int nUsableBpartCount = 0);

	vec3					MakeUpwardForce( float fXYForce, float fZForce, XModuleEntity * pModuleEntity, vec3 vForceDir );
	void					ChangeTransformForBPartActorNode(std::vector<std::string>& _rActorNodeName);

public:
	XModuleBParts(XObject* pOwner = NULL);
	virtual ~XModuleBParts(void);

	void					SetBreakableParts(MUID uidAttacker, int nIndex);
	void					SetBreakableParts(int nIndex, float fXYForce, float fZForce);
	void					SetBreakablePartEx(MUID uidAttacker, int nIndex, TD_USABLE_BPART* pUsableBpart, int nUsableBpartCount);

	void					SetBrokenParts(int nIndex);
	void					BreakPartsRecovery();

	void					ClearForReload();

	wstring					GetBreakablePartsActorNodeName(int nIndex);

	void					SetLootableBPartNPC(MUID uid, bool bLootable);
};


#endif //_XMODULE_BREAKABLE_PARTS_H