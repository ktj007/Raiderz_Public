#pragma once

#include "TSwordTrailSampling.h"

enum CHECK_DUMMY_LOC_RESULT
{
	CDLR_OK,
	CDLR_NO_ANIMATION,
	CDLR_NO_DUMMY_NODE,
	CDLR_NOT_USE,
};

class AniInfo
{
public:
	string name;
	string loop;
	string desc;
};
typedef std::vector<AniInfo*> ANIINFOVECTOR;

struct stANIINFO
{
	ANIINFOVECTOR	vecPlayerAniInfo;
	ANIINFOVECTOR	vecMonsterAniInfo;
	ANIINFOVECTOR	vecNPCAniInfo;
};

class TTabAnimation
{
private:
	stANIINFO		m_aniInfoData;

	vector<RAnimationEvent*> m_vecCurrentEventList;
protected:
	ANIINFOVECTOR*	m_pvecAniIfo;

private:
	bool	CheckSwordTrailEvent(string strEventName, RAnimation* pAnimation, bool& bSwordTrailEvent);
	bool	CheckAnimationDummyLocPositionkeyTrack(RAnimationNode* pAnimationNode);
	bool	CheckAnimationDummyLocRotationkeyTrack(RAnimationNode* pAnimationNode);
	bool	CheckAnimationDummyLocScalekeyTrack(RAnimationNode* pAnimationNode);
	bool	CheckAnimationDummyLocViskeyTrack(RAnimationNode* pAnimationNode);

	bool	LoadAniInfo(std::vector<AniInfo*>& vecAniInfo, const char* Model);
	void	DelAniInfo();

	void	ReSetSwordTrailEventID(RAnimation* pAnimation, string& strEvent);

	void	ModifyEventList(int index);
	void	DeleteEventList(int index);

	string	GetHitSoundName(string& strAniName);
	string	GetSelectAnimationName();

	void	GetSortAnimationEvent(vector<RAnimationEvent*>& vecAnimationEvent, vector<RAnimationEvent*>& vecOutAnimationEvent);

	int		GetAnimationEventIndex(RAnimation* pAnimation, int nIndex);

public:
	TTabAnimation(void);
	virtual ~TTabAnimation(void);

	TCharacter* GetActor();

	bool	DeleteAnimation();
	void	AddEvent();
	void	ModifyEvent(int index);
	void	DeleteEvent(int index);

	void	Init();
	void	InitEventList();
	void	InitAnitreeView();
	void	InitAnimationInfo();
	void	InitAniInfo();

	void	InitHitSoundInfo();
	void	ModifyHitSoundInfo();

	RAnimation* GetAnimation(System::String^ Aniname);
	RAnimation* GetAnimation(const char* aniName);

	bool	SetAniInfo(System::String^ str1);
	bool	LoadAniFile(const char* FileName);

	void	SelectedEventList(int index);

	void	GetCustomEffectList(vector<string>& vecEffectList);

	CHECK_DUMMY_LOC_RESULT	CheckAnimationDummyLocNode(const char* aniName);
	bool	CheckUsableAnimationEvent(const RAnimationEvent* pEvent);

	void	GetAnimationList(System::String^ Aniname, vector<string>& vecOutList);
	void	GetFacialAnimationEvent(vector<string>& vecOutList);

	void	ResetAnimationEventList();

	void	SetAniTreeViewName(System::String^ strAniTreeViewName);
	string	GetAniTreeViewName() { return m_strAniTreeViewName; }

protected:
	string m_strAniTreeViewName;
};
