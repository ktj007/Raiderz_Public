#ifndef _XBUFF_EFFECT_H
#define	_XBUFF_EFFECT_H

#pragma once

#include "XEffectInfo.h"
#include "XEffectInvoker.h"

//////////////////////////////////////////////////////////////////////////
struct BUFF_EFFECT_SET_DATA
{
	int					nBuffID; 
	BUFF_EFFECT_TYPE	nType;
	bool				bRemoveOncePlay;
	int					nPartsSlotType;

	BUFF_EFFECT_SET_DATA() : nBuffID(0), nType(BUFF_GAIN_EFFECT), bRemoveOncePlay(false), nPartsSlotType(-1) {}
};

struct BUFF_EFFECT_LOOP
{
	CUSTOM_EFFECT_TYPE	type;

	MUID				uidEffect;

	int					nBuffID;

	BUFF_EFFECT_LOOP() : type(CET_NONE), uidEffect(MUID::ZERO), nBuffID(-1) {}
	BUFF_EFFECT_LOOP(CUSTOM_EFFECT_TYPE etype, int nID, MUID uid) : type(etype), nBuffID(nID), uidEffect(uid) {}

};

struct BUFF_EFFECT_DISAPPEAR
{
	BUFF_EFFECT_LOOP	buffEffect;

	float				fDisappearTime;
	BUFF_EFFECT_DISAPPEAR() : fDisappearTime(0.0f) {}
};

typedef vector<BUFF_EFFECT_LOOP> VEC_BUFF_EFFECT_LOOP;
enum BUFF_EFFECT_GAIN_STAT
{
	BEGS_NONE,
	BEGS_START,
	BEGS_LOOP,
	BEGS_RELEASE
};

struct stBUFF_EFFECT_GAIN
{
	BUFF_EFFECT_SET_DATA	effectData;

	BUFF_EFFECT_GAIN_STAT	eState;

	VEC_BUFF_EFFECT_LOOP	vecUIDStart;
	VEC_BUFF_EFFECT_LOOP	vecUIDLoop;

	stBUFF_EFFECT_GAIN()
	{
		eState			= BEGS_NONE;
	}
};

struct stEnchantBuffEffect
{
	int								nBuffID;
	
	VEC_BUFF_EFFECT_LOOP			vecEnchantBuffEffect;
	
	wstring							strResourceName;
	wstring							strBoneName;

	int								nPartsSlotType;
};

//////////////////////////////////////////////////////////////////////////
class XBuffEffect
{
private:
	VEC_BUFF_EFFECT_LOOP			m_vBuffLoopEffect;									// ∑Á«¡ ¿Ã∆Â∆Æ UID
	vector<BUFF_EFFECT_DISAPPEAR>	m_vBuffDisappearEffect;
	vector<stBUFF_EFFECT_GAIN>		m_vecGainBuffEffect;
	vector<stEnchantBuffEffect>		m_vEnchantBuffEffect;

	XObject*						m_pObject;

private:
	void							CreateBuffEffect(int nID, BUFF_EFFECT_TYPE nType, vector<EFFECT_RESULT_DATA>* pvecResult = NULL);
	void							CreateEnchantBuffEffect(int nBuffID, wstring& strEffectName, wstring& strBoneName, int nPartsSlotType);
	
	void							DelLoopBuffEffect(VEC_BUFF_EFFECT_LOOP& vecEffect, bool bImmediatlyDelete = false);
	void							AllDelBuffEffect(bool bImmediatlyDelete = false);

	void							UpdateDisAppearFadeHide(float fDelta);
	void							UpdateGainEffect();

	bool							CheckExistEffect(VEC_BUFF_EFFECT_LOOP& vecEffect);

	void							SetGainBuffEffect(vector<EFFECT_RESULT_DATA>& vecResult, int nBuffID, VEC_BUFF_EFFECT_LOOP& vecBuffEffect);
	void							SetBuffLoopEffect(vector<EFFECT_RESULT_DATA>& vecResult, int nBuffID, bool bRemoveOncePlay);

	stBUFF_EFFECT_GAIN*				GetGainBuffEffect(int nID);
	void							DelGainBuffEffect(int nID);
	
public:
	XBuffEffect(XObject* pOwner);
	virtual ~XBuffEffect();

	void							Update(float fDelta);

	bool							AddBuffEffect(BUFF_EFFECT_SET_DATA& data);
	void							DelBuffEffect(int nBuffID, int nPartsSlotType);
	void							DieBuffEffect(int nBuffID);

	bool							AddEnchantBuffEffect(int nBuffID, int nPartsSlotType);

	void							Init_ForChangeField();
};

#endif // _XBUFF_EFFECT_H
