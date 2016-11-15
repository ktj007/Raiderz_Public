#pragma once


enum TALENT_ANIMATION_TYPE
{
	TAT_CASTING,
	TAT_CASTINGLOOP,
	TAT_USE,
	TAT_EXTRA,
	TAT_EXTRA2,
	TAT_EXTRA3
};

enum TALENT_ANIMATION_LIST_TYPE
{
	TALT_ANIMATION,
	TALT_MOTION
};

struct  ANIMATION_INFO
{
	string				strAnimationName;

	string				strAnimationPathName;
	string				strAnimationFileName;
	float				fAnimationSpeed;
	int					nFrameOver;
	AnimationLoopType	loopType;

	ANIMATION_INFO()
	{
		fAnimationSpeed = 1.0;
		nFrameOver		= -1;

		loopType		= RAniLoopType_Loop;
	}
};

struct WEAPON_ANIMATION_INFO
{
	WEAPON_TYPE				eWeaponType;
	vector<ANIMATION_INFO>	vecAniInfo;

	WEAPON_ANIMATION_INFO()
	{
		eWeaponType		= WEAPON_NONE;
	}
};

struct TALENT_ANIMATION_INFO
{
	TALENT_ANIMATION_LIST_TYPE	eType;	
	string				strMotionName;
	bool					bAutoPrefix;

	vector<WEAPON_ANIMATION_INFO>	vecTalentAniInfo;
};

struct  SIMPLEANIMATION_INFO
{
	string				strAnimationName;
	string				strAnimationPathName;
	string				strAnimationFileName;
	float				fAnimationSpeed;
	AnimationLoopType	loopType;

	SIMPLEANIMATION_INFO()
	{
		fAnimationSpeed = 1.0;

		loopType		= RAniLoopType_Loop;
	}
};

class TAnimationHelper
{
	enum ONLY_ONE_WEAPON_TYPE_RESULT
	{
		OOWTR_NONE,			// 관계없다.
		OOWTR_YES,			// 같다.
		OOWTR_NO			// 아니다.
	};

private:
	// 공통

	// 시작
	TALENT_ANIMATION_INFO	GetMotionAnimationStart(CSTalentInfo* pTalentInfo, string& strTalentAnimationName);
	TALENT_ANIMATION_INFO	GetAnimtionStart(CSTalentInfo* pTalentInfo, string& strTalentAnimationName);

	// 애니메이션 추출
	bool					GetMotionAnimation(vector<ANIMATION_INFO>& vecOutAniInfo, string strMeshPath, string strModelName, XMotion* pMotion, int nWeaponType);
	bool					GetAnimation(vector<ANIMATION_INFO>& vecOutAniInfo, string strMeshPath, string strModelName, string strUseAnimation, int nWeaponType);

	void					GetAnimationInfo( string strMeshPath, string strModelName, int NPCID, int nWeaponTypeMax, CSTalentInfo* pTalentInfo, bool bNPC, string& strTalentAnimationName, TALENT_ANIMATION_INFO &talentAniInfo );
	void					GetMotionAnimationInfo( string strMeshPath, string strModelName, int NPCID, int nWeaponTypeMax, CSTalentInfo* pTalentInfo, bool bNPC, XMotion* pMotion, TALENT_ANIMATION_INFO &talentAniInfo );

	ONLY_ONE_WEAPON_TYPE_RESULT			CheckAnimationByOnlyOneWeaponType(CSTalentInfo* pTalentInfo, WEAPON_TYPE eWeaponType);

public:
	TAnimationHelper();
	virtual ~TAnimationHelper();

	static void				GetModelPathName(string& strOutMeshPath, string& strOutModelName, int nNPCID = 0);
	static string			GetModelFilePathFullName(int nNPCID = 0);
	static bool				CalAnimaitonPos(TCharacter* pActor, TPlayMotion* pMotion, vector<RMatrix>& vecDummyLocLast);
	static bool				GetAnimationInfo(SIMPLEANIMATION_INFO& stOutData, string strMeshPath, string strModelName, string strAniName);
	static bool				GetAnimationInfoInXml(SIMPLEANIMATION_INFO& stOutData, string strAnimationXMLFileName, string strCompareAniName, string strFilter);
	static bool				GetAnimationInfoInXml(vector<SIMPLEANIMATION_INFO>& vecOutData, string strAnimationXMLFileName, string strFilter = "");

	TALENT_ANIMATION_INFO	GetTalentAnimationInfo(CSTalentInfo* pTalentInfo, TALENT_ANIMATION_TYPE eType);

	static bool				SetupActor( rs3::RActor &actor, CSTalentInfo* pTalentInfo );
};