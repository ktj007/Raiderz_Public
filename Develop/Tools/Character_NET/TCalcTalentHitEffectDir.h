#pragma once

class XTalentInfo;

class TCalcTalentHitEffectDir
{
private:
	bool				CalcTalentHitEffectPosDirByTalentHitTime(float fHitTime, string strModelName, vec3& vOutPos, vec3& vOutDir);

	bool				CheckSwordNodeExist(TCharacter* pActor, TPlayMotion* pPlayMotion, string strModelName);
	bool				CheckDuplicationSwordTrailEffectDir(vector<XTalentHitEffectPosDir>& vecOrg, vector<XTalentHitEffectPosDir>& vecNew);
	bool				CheckCreateTalentHitEffectDir(TCharacter* pActor, TPlayMotion* pPlayMotion, string strModelName, XTalentEffectInfo* pEffectInfo);
	
	void				ClearSwordHitEffectDir(XTalentEffectInfo* pEffectInfo, string strModelName);
	void				DelHitEffectDir( XTalentEffectDataMgr* pEffectData );

	void				GetSwordTrailEffectDirByModelName(string strModelName, vector<XTalentHitEffectPosDir>& vecOrg, vector<XTalentHitEffectPosDir>& vecOut);
public:
	TCalcTalentHitEffectDir();
	virtual ~TCalcTalentHitEffectDir();

	bool				CalcTalentHitEffectDir(XTalentInfo * pTalentInfo, string strModelName, bool bCheck = false);
	bool				CalcTalentHitEffectDir(XTalentInfo * pTalentInfo, string strModelName, int nSegID);

};