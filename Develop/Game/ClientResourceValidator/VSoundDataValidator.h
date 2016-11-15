#pragma once

#include "VDataValidator.h"
#include "XEffectBaseInvoker.h"

class VCheckEffectBaseInvoker : public XEffectBaseInvoker
{
private:
	vector<string>						m_vecWeaponType_SS;
	vector<string>						m_vecArmorType_SS;
	vector<string>						m_vecFootStepMaterialName;
	vector<string>						m_vecArmorType_AS;
	vector<string>						m_vecSoundType_SD;

private:
	bool								CheckResrvedEffectData_SS(string strPre, string strSoundName);
	bool								CheckResrvedEffectData_FootStep(string strSoundResourceName, string strSoundName);
	bool								CheckResrvedEffectData_AS(string strSoundResourceName, string strSoundName);
	bool								CheckResrvedEffectData_SD(string strSoundName);

public:
	VCheckEffectBaseInvoker();
	virtual ~VCheckEffectBaseInvoker() {}

	bool								CheckResrvedEffectData(string strSoundName);
};

class VSoundDataValidator : VDataValidator
{
private:
	int									m_nSoundCount;

	VCheckEffectBaseInvoker				m_CheckEffectBaseInvoker;

public:
	VErrorMsg							m_SoundInfoValidatorError;

private:
	bool								CheckSoundFileName(VSoundInfoMgr* pSoundInfoMgr, string strFileName);
	bool								CheckSoundUsableFromEffectInfo(VEffectInfoMgr* pEffectInfoMgr, string strSoundName);
	bool								CheckSoundUsableFromTalentInfo(VTalentInfoMgr* pTalentInfoMgr, string strSoundName);
	bool								CheckSoundUsableFromMeshInfo(VMeshInfoMgr* pMeshInfoMgr, string strSoundName);
	bool								CheckSoundUsableFromNpcInfo(VNpcInfoMgr* pNpcInfoMgr, string strSoundName);
	bool								CheckSoundUsableFromItemInfo(VItemInfoMgr* pItemInfoMgr, string strSoundName);
	bool								CheckSoundUsableFromFieldInfo(string strSoundName);
	bool								CheckSoundUsableFromSystemInfo(VValidatorInfoMgr* vInfoMgr, string strSoundName);

public:
	VSoundDataValidator();
	virtual ~VSoundDataValidator();

	virtual void						Init(VValidatorInfoMgr* vInfoMgr);
	virtual	void						CheckStart(VValidatorInfoMgr* vInfoMgr);

	void								CheckSoundInfo(VSoundInfoMgr* pSoundInfoMgr, string& strName, RSampleInfo* pSoundInfo, VValidatorInfoMgr* vInfoMgr);
	void								CheckSoundUsable(string& strName, VValidatorInfoMgr* vInfoMgr);

	virtual int							GetCheckCount() { return m_nSoundCount; }
};
