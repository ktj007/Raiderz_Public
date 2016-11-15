#ifndef _XCUT_SCENE_FIELD_INTRO_MGR_H
#define _XCUT_SCENE_FIELD_INTRO_MGR_H

#include "XPath.h"
#include "XGame.h"
#include "RBackgroundWorker.h"
#include "RMesh.h"
#include "RMeshMgr.h"

#include "XGlobal.h"
#include "XSystem.h"
#include "XUISystem.h"

#include "XPost_Cutscene.h"

class XCutSceneFieldIntroMgr
{
private:
	struct SIntroCutSceneInfo
	{
		SIntroCutSceneInfo(int _nId = -1, int _nTime = -1, int _nWeather = -1)
		{
			m_nCutSceneId = _nId;
			m_nTime = _nTime;
			m_nWeather = _nWeather;
			m_bIntroCutSceneEnv = false;
		};

		int m_nCutSceneId;
		int m_nTime;
		int m_nWeather;
		bool m_bIntroCutSceneEnv;

		std::vector< wstring > m_vecPreLoadElu;
	};

private:
	XCutSceneFieldIntroMgr();
	~XCutSceneFieldIntroMgr(){ ReleaseResource(); }

public:
	static XCutSceneFieldIntroMgr& GetCutSceneFieldIntroMgr()
	{
		static XCutSceneFieldIntroMgr mgr;
		return mgr;
	}

	void Reset();
	void ReleaseResource();

	bool SetIntroCutSceneEnv(int* nTime, int* nWeather);
	bool PreLoadIntroCutScene();
	bool beginIntroCutScene();
	void RenderMaskForCutScene();
	void RenderBlackScreen();

	void InitBlockProc(int nBlockStartCnt, int nBlockEndCnt){ m_nBlockStartCnt = nBlockStartCnt; m_nBlockEndCnt = nBlockEndCnt; }

	void SetObjectLoading(bool bObjectLoading) { m_bObjectLoading = bObjectLoading; }
	bool GetObjectLoading() { return m_bObjectLoading; }

	void MoveMyPlayerPos();

private:
	void FillResourceData(SIntroCutSceneInfo& info);

private:
	int m_nBlockStartCnt;
	int m_nBlockEndCnt;
	int m_nPreIntroCutSceneID;
	bool m_bObjectLoading;

	map<int, SIntroCutSceneInfo > m_mapIntroCutScene;
	std::vector< rs3::RResource* > m_vecPreLoadEluRs3;
	vec3 vecMyPlayerPos;
};

#endif