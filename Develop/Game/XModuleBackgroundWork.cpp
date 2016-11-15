#include "StdAfx.h"
#include "XModuleBackgroundWork.h"
#include "RBackgroundWorkUnit.h"

#include "RSceneManager.h"
#include "REffectManager.h"

#include "XNonPlayer.h"
#include "XNPCInfo.h"
#include "XTalentInfoMgr.h"
#include "XEffectInfo.h"

#include "XResourceManager.h"

class XBackgroundWorkUnit : public RBackgroundWorkUnit
{
public:
	XBackgroundWorkUnit() : m_bBGWorkCompleted(false) {}
	virtual ~XBackgroundWorkUnit(){}

	virtual void Do() = 0;
	virtual bool CheckDone() = 0;

	bool IsBGWorkCompleted(){ return m_bBGWorkCompleted; }

protected:
	virtual void OnBackgroundWork(){ Do(); }
	virtual void OnUpdateCompleteWork()
	{
		m_bBGWorkCompleted = true;
		FinishWork();
	}
	virtual int				CalcPriority(){ return (RBGWK_HIGHEST-1); }
	bool					m_bBGWorkCompleted;
};

class XBackgroundWorkUnit_EffectCreator : public XBackgroundWorkUnit
{
public:
	XBackgroundWorkUnit_EffectCreator(const std::wstring& _strEffectName)
	{
		m_strEffectName = _strEffectName;
		m_pEffectSceneNode = NULL;
	}
	virtual ~XBackgroundWorkUnit_EffectCreator()
	{
		SAFE_DELETE(m_pEffectSceneNode);
		REngine::GetSceneManager().GetEffectManager()->RemoveRefEffectSource(MLocale::ConvUTF16ToAnsi(m_strEffectName.c_str()));
	}

	virtual void Do()
	{
		m_pEffectSceneNode = REngine::GetSceneManager().GetEffectManager()->CreateEffectSource(MLocale::ConvUTF16ToAnsi(m_strEffectName.c_str()).c_str(), true, false);
		//mlog("XBackgroundWorkUnit_EffectCreator : Do\n");
	}

	virtual bool CheckDone()
	{
		if (NULL == m_pEffectSceneNode)
			return true;

		if (m_pEffectSceneNode->IsLoadingCompleted())
		{
			if (1 != REngine::GetSceneManager().GetEffectManager()->AddRefEffectSource(MLocale::ConvUTF16ToAnsi(m_strEffectName.c_str()), m_pEffectSceneNode))
			{
				delete m_pEffectSceneNode;
			}
			m_pEffectSceneNode = NULL;
			return true;
		}

		return false;
	}

private:
	std::wstring		m_strEffectName;
	REffectSceneNode*	m_pEffectSceneNode;
};

XModuleBackgroundWork::XModuleBackgroundWork( XObject* pOwner/*=NULL*/ )
: XModule(pOwner)
{
}

XModuleBackgroundWork::~XModuleBackgroundWork( void )
{
	for(std::list<XBackgroundWorkUnit*>::iterator itr = m_BackgroundWorkUnits.begin();
		itr != m_BackgroundWorkUnits.end(); ++itr)
	{
		XBackgroundWorkUnit* pUnit = *itr;

		if ( false == pUnit->IsBGWorkCompleted() )
			pUnit->InvalidateWork();

		delete pUnit;
	}

	for(std::list<XBackgroundWorkUnit*>::iterator itr = m_BackgroundWorkUnitsChecked.begin();
		itr != m_BackgroundWorkUnitsChecked.end(); ++itr)
	{
		XBackgroundWorkUnit* pUnit = *itr;

		if ( false == pUnit->IsBGWorkCompleted() )
			pUnit->InvalidateWork();

		delete pUnit;
	}
}

void XModuleBackgroundWork::OnUpdate( float fDelta )
{
	for(std::list<XBackgroundWorkUnit*>::iterator itr = m_BackgroundWorkUnits.begin();
		itr != m_BackgroundWorkUnits.end();)
	{
		XBackgroundWorkUnit* pWorkUnit = *itr;
		if (pWorkUnit->IsBGWorkCompleted() && pWorkUnit->CheckDone())
		{
			itr = m_BackgroundWorkUnits.erase(itr);
			m_BackgroundWorkUnitsChecked.push_back(pWorkUnit);
			//mlog("XModuleBackgroundWork::OnUpdate\n");
		}
		else
		{
			++itr;
		}
	}
}

XEventResult XModuleBackgroundWork::OnEvent( XEvent& msg )
{
	class BGUnitCreator
	{
	public:
		BGUnitCreator(std::list<XBackgroundWorkUnit*>* _pUnits)
		{
			m_pUnits = _pUnits;
		}

		void AddUnitFromResourceName(const std::wstring& _rResourceName)
		{
			if (false == _rResourceName.empty() && m_RequestResources.find(_rResourceName) == m_RequestResources.end())
			{
				m_RequestResources.insert(_rResourceName);

				// record cache
				if (global.res && global.res->IsUsingEffectCacheLog())
					global.res->LogEffectCache(_rResourceName, true);

				if (NULL == REngine::GetSceneManager().GetEffectManager()->GetEffect(MLocale::ConvUTF16ToAnsi(_rResourceName.c_str()).c_str()))
				{
					XBackgroundWorkUnit_EffectCreator* pWorkUnit = new XBackgroundWorkUnit_EffectCreator(_rResourceName);
					m_pUnits->push_back(pWorkUnit);
					REngine::GetBackgroundWorker().AddToReadyQueue(pWorkUnit);
				}
			}
		}

		void AddUnit(std::wstring& _effectName)
		{
			XEffectInfo * pEffectInfo = info.effect_Info->GetEffectInfo(_effectName);
			if (pEffectInfo &&
				pEffectInfo->m_vecEffectInfoItem.empty() == false)
			{
				for (vector<XBaseEffectInfoItem *>::iterator itr = pEffectInfo->m_vecEffectInfoItem.begin();
					itr != pEffectInfo->m_vecEffectInfoItem.end(); ++itr)
				{
					XBaseEffectInfoItem* pEffectInfoItem = *itr;
					if (pEffectInfoItem->eEffectType == CET_MODEL)
					{
						XEffectInfoItem_Model* pModelEffectInfoItem = (XEffectInfoItem_Model*)pEffectInfoItem;
						AddUnitFromResourceName(pModelEffectInfoItem->strResourceName);
					}
				}
			}
		}

		void AddUnitFromEvent(CSTalentEventInfo& talentEventInfo)
		{
			if(talentEventInfo.m_nEvent == TE_EFFECT || talentEventInfo.m_nEvent == TE_FIRE_EFFECT)
			{
				AddUnit(talentEventInfo.m_strParam1);
			}
			else if(talentEventInfo.m_nEvent == TE_LAUNCH_PROJECTILE)
			{
				AddUnit(talentEventInfo.m_ProjectileInfo.m_strEffectName);
				AddUnit(talentEventInfo.m_ProjectileInfo.m_strAttackEffectName);
				AddUnit(talentEventInfo.m_ProjectileInfo.m_strHitGroundEffectName);
				AddUnit(talentEventInfo.m_ProjectileInfo.m_strFireEffectName);
			}
		}

	private:
		std::set< std::wstring >			m_RequestResources;
		std::list<XBackgroundWorkUnit*>*	m_pUnits;
	};

	if (msg.m_nID == XEVTD_MESH_LOADING_COMPLETE)
	{
		if (NULL == global.res)
			return MR_TRUE;

		if (false == global.res->IsUsingEffectCache())
			return MR_TRUE;

		if (false == m_BackgroundWorkUnits.empty())
			return MR_TRUE;

		XNonPlayer* pNPC = NULL;
		if (m_pOwner->IsNPC() && m_pOwner->GetType() == XOBJ_NPC)
			pNPC = m_pOwner->AsNPC();

		if (pNPC && pNPC->IsDead() == false)
		{
			BGUnitCreator bgUnitCreator(&m_BackgroundWorkUnits);
			XNPCInfo* pNPCInfo = pNPC->GetInfo();

			for (CSTalentInfoMgr::iterator itr = info.talent->begin(); itr != info.talent->end(); ++itr)
			{
				XTalentInfo* pTalentInfo = (XTalentInfo*)itr->second;
				if (pTalentInfo->m_setNPCID.find(pNPCInfo->nID) != pTalentInfo->m_setNPCID.end())
				{
					// talent effect
					XTalentEffectDataMgr* pTalentEffectDataMgr = pTalentInfo->GetEffectDataMgr();
					if (pTalentEffectDataMgr)
					{
						for(XTalentEffectDataMgr::iterator itr_talent = pTalentEffectDataMgr->begin();
							itr_talent != pTalentEffectDataMgr->end(); ++itr_talent)
						{
							bgUnitCreator.AddUnit((*itr_talent)->m_strEffectName);
						}
					}

					// talent animation effect
					for(vector<CSTalentEventInfo>::iterator itNor = pTalentInfo->m_Events.m_vecEvents.begin(); itNor != pTalentInfo->m_Events.m_vecEvents.end(); itNor++)
					{
						bgUnitCreator.AddUnitFromEvent(*itNor);
					}

					for(vector<CSTalentEventInfo>::iterator itAct = pTalentInfo->m_Events.m_vecActEvents.begin(); itAct != pTalentInfo->m_Events.m_vecActEvents.end(); itAct++)
					{
						bgUnitCreator.AddUnitFromEvent(*itAct);
					}

					// buff effect
					const int ID_SIZE = 2;
					int buffIds[ID_SIZE] = { pTalentInfo->m_Buff1.nID, pTalentInfo->m_Buff2.nID };
					for (int buff_idx = 0; buff_idx < ID_SIZE; ++buff_idx)
					{
						XBuffInfo* pBuffInfo = info.buff->Get(buffIds[buff_idx]);
						if (pBuffInfo)
						{
							// 버프 이펙트 정보 가져오기
							vector<XBuffEffectInfo *> vecEffectInfoByType;
							for (int i = 0; i < BUFF_EFFECT_MAX; ++i)
							{
								pBuffInfo->m_BuffEffect.GetEffectInfoList((BUFF_EFFECT_TYPE)i, vecEffectInfoByType);
							}

							for(std::vector<XBuffEffectInfo *>::iterator itr_buff_effect = vecEffectInfoByType.begin();
								itr_buff_effect != vecEffectInfoByType.end(); ++itr_buff_effect)
							{
								bgUnitCreator.AddUnit((*itr_buff_effect)->m_strEffectName);
							}
						}

					}
				}
			}

			// effect from PreLoad.xml
			if(global.res)
			{
				std::vector<std::wstring>* pEffectList = global.res->GetNpcEffectCacheList(pNPCInfo->nID);
				if (pEffectList)
				{
					int nEffectListCnt = (int)pEffectList->size();
					for (int nEffectListIdx = 0; nEffectListIdx < nEffectListCnt; ++nEffectListIdx)
					{
						bgUnitCreator.AddUnitFromResourceName(pEffectList->at(nEffectListIdx));
					}
				}
			}

			// flush
			if (false == m_BackgroundWorkUnits.empty())
			{
				REngine::GetBackgroundWorker().ReOrderReadyQueue();
				REngine::GetBackgroundWorker().FlushReadyQueueAndRun();
			}
		}
	}

	return MR_TRUE;
}

void XModuleBackgroundWork::OnSubscribeEvent()
{
	XModule::OnSubscribeEvent();
	Subscribe(XEVTD_MESH_LOADING_COMPLETE);
}
