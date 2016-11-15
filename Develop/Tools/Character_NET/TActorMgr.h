#ifndef _TACTOR_MANAGER_H
#define _TACTOR_MANAGER_H

#pragma once

class TActorMgr
{
private:
	struct stACTORDATA
	{
		TCharacter*					pActor;
		float						fRemainTime;
		string						strFileName;

		stACTORDATA()
		{
			pActor					= NULL;
			init();
		}

		void init()
		{
			fRemainTime				= 12000.0f;
		}
	};

private:
	vector<stACTORDATA>			m_vecActorList;

	stACTORDATA					m_FemalePlayerModel;
	stACTORDATA					m_MalePlayerModel;

	RSceneManager*				m_pSceneManager;
	TSoulHuntAnimationEventListener* m_pEventListener;
	TCharacter*					m_pCurrentActor;

private:
	bool						CheckDelTime(float fDelta);
	bool						CheckDelCount();
	TCharacter*					CheckGetActor(string strFileName);
	stACTORDATA*				CheckPlayerActor(string strFileName);

	void						SetDefaultPlayerModel();

	string						GetPlayerModelFileName(SEX nSex);

	TCharacter*					CreateActor(const char* FileName, bool bUseBackgroud = false);

	void						CreateNPCFaceAnimation( TCharacter* pActor, string FileName );
	void						DelCharacter(stACTORDATA* pData);

	void						UpdateAnimation(float fDelta);

public:
	TActorMgr(RSceneManager* pSceneManager, TSoulHuntAnimationEventListener* pEventListener);
	virtual ~TActorMgr();

	void						Init();
	void						Update(float fDelta);
	void						Destroy();
	void						ActorListDestroy();
	
	TCharacter*					ModelLoad(const char* FileName);
	bool						ModelDel(const char* FileName);

	TCharacter*					GetFemalePlayerModel()	{ return m_FemalePlayerModel.pActor; }
	TCharacter*					GetMalePlayerModel()	{ return m_MalePlayerModel.pActor; }
};


#endif