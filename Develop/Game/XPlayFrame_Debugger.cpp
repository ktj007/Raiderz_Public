#include "stdafx.h"
#include "XPlayFrame_Debugger.h"
#include "RSceneManager.h"
#include "XGame.h"
#include "RSceneManager.h"
#include "XWorld.h"
#include "XBaseApplication.h"
#include "XModuleTalent.h"
#include "XModuleEntity.h"
#include "XModuleMovable.h"
#include "XMyPlayer.h"
#include "XModuleCollision.h"
#include "XNetDebuger.h"
#include "XPost_GM.h"
#include "XReplay.h"
#include "MTime.h"
#include "RBackgroundWorker.h"
#include "CSMeshInfo.h"
#include "XSystem.h"
#include "XCameraManager.h"
#include "RMesh.h"
#include "XNetwork.h"
#include "XCameraHelper.h"
#include "XSensorManager.h"
#include "XSensor.h"
#include "XDebugInfoBuilder.h"
#include "XGameInitializer.h"
#include "XMovableProcObjectChecker.h"

XPlayFrame_Debugger::XPlayFrame_Debugger() : m_pRecorder(NULL)
{


}

XPlayFrame_Debugger::~XPlayFrame_Debugger()
{

}


void XPlayFrame_Debugger::RenderDebug()
{
#ifdef _PUBLISH
	return;
#endif

	//	TIMEPROFILE_THIS_FUNCTION;

	PFC_THISFUNC;

	if (!gg.omgr) return;

	XNetDebuger::DebugRender();

	XRenderHelper::SetRenderState();

	RenderSectorGrid();
	RenderCollision();

	RenderSensor();
	RenderSoundRange();

	RenderArea();

	XRenderHelper::EndRenderState();
}

bool XPlayFrame_Debugger::OnDebugEvent( MEvent& e )
{
	if (gvar.MyInfo.ChrInfo.nPlayerGrade >= PLAYER_GRADE_GM)
	{
		if (gg.controller)
		{
			switch(e.nMessage)
			{
			case MWM_CHAR:
				{
					switch (e.nKey) 
					{
					case ',':
						{
							if (gvar.MyInfo.ChrInfo.nPlayerGrade >= PLAYER_GRADE_DEVELOPER)
							{
								return ToggleFreeLookCamera();
							}

							return true;
						}
						break;
					}
				}
			}
		}
	}

	return false;

	XMyPlayer* pMyPlayer = gg.omgr->FindMyPlayer();

	// 시간 그래프 보고있을때
	//if(XScreenDebugger::GetInstance()->GetSelLayer()==XScreenDebugger::LAYER2
	//	&& e.nMessage==MWM_KEYUP )
	//	REngine::GetResourceProfiler().ProcessDebugKey(e.nKey);

	if ((gg.controller) /*&& (global.ui->IsFocusMainInput())*/)
		switch(e.nMessage)
	{
		case MWM_CHAR:
			{
				switch (e.nKey) 
				{
#ifdef _DEBUG
				case 'k':		// MyControl Action State 초기화
					{
						if (pMyPlayer == NULL) return false;
						XModuleMyControl* pModuleMyControl = pMyPlayer->GetModuleMyControl();
						pModuleMyControl->DoActionIdle();
					}
					break;
#endif
					//				case 'm':
				case ',':		// Ctrl + 'M'
					{
						if (gvar.MyInfo.ChrInfo.nPlayerGrade >= PLAYER_GRADE_DEVELOPER)
						{
							return ToggleFreeLookCamera();
						}

						return true;
					}
					break;
				case 'u':
					{
						ToggleRecordReplay();
					}
					break;
				case 'y':
					{
						gvar.Game.MyCombatStanceAutoReleaserData.bReserved = true;
						gvar.Game.MyCombatStanceAutoReleaserData.nReserveTime = XGetNowTime();
					}
					break;
				case '.':
					{

						//vector<XObject*> vecObj;
						//gg.omgr->GetSortDistanceEntityTypeList(vecObj, ETID_NPC);
						//if (vecObj.empty()) return false;

						//for (vector<XObject*>::iterator itor = vecObj.begin(); itor != vecObj.end(); ++itor)
						//{
						//	XObject* pObj = (*itor);
						//	if (pObj)
						//	{
						//		XMyPlayer* pMyPlayer = gg.omgr->FindMyPlayer();

						//		vec3 vDir = pMyPlayer->GetPosition() - pObj->GetPosition();
						//		vDir.Normalize();

						//		pObj->GetModuleEntity()->SetDirectionSmoothly(vDir, 1.0f);
						//	}
						//}

						gvar.Debug.bRenderMoveDebug = !gvar.Debug.bRenderMoveDebug;
					}
					break;
				//case '/':
				//	{
				//		if (pMyPlayer == NULL) return false;


				//		TD_TALENT_HIT td_talent_hit;
				//		td_talent_hit.nAttackerUIID = XGetMyUIID();
				//		td_talent_hit.nVictimUIID = XGetMyUIID();
				//		td_talent_hit.nTalentID = 100001;
				//		td_talent_hit.nDamage = 0;
				//		td_talent_hit.nFlags = 0;
				//		td_talent_hit.nMFWeight = gvar.Debug.nTestMFWeight;
				//		//td_talent_hit.nMFState = gvar.Debug.nTestMF;
				//		//td_talent_hit.nMFWeight = 700.0f;
				//		td_talent_hit.nMFState = MF_UPPERED;

				//		XMyPlayer* pMyPlayer = gg.omgr->FindMyPlayer();

				//		TD_TALENT_HIT_EX_KNOCKBACK td_ex_knockback;
				//		td_ex_knockback.vKnockbackDir = -pMyPlayer->GetDirection();
				//		td_ex_knockback.vKnockbackVictimPos = pMyPlayer->GetPosition();

				//		XPOSTCMD2(MC_LOCAL_TEST_SELF_MOTION_FACTOR, MCommandParameterSingleBlob(&td_talent_hit, sizeof(TD_TALENT_HIT)),
				//			MCommandParameterSingleBlob(&td_ex_knockback, sizeof(TD_TALENT_HIT_EX_KNOCKBACK)));

				//		////MResetProfile();
				//		//XMyPlayer* pMyPlayer = gg.omgr->FindMyPlayer();
				//		//pMyPlayer->GetModuleEntity()->GetActor()->DumpSceneNode("character.xml");
				//	}
				//	break;
				case 'v':
					{
						ReloadConst();
					}
					break;
				case '1':
				case '2':
				case '3':
				case '4':
					{
						if (global.app->GetMode() == EM_LOCAL_TEST)
						{
							XObject* pObj = gg.omgr->Find(gvar.Debug.uidTargetNPC);
							if (pObj == NULL) return CR_TRUE;

							XModuleActorControl* pModuleActorControl = pObj->GetModuleActorControl();
							if (pModuleActorControl)
							{
								pModuleActorControl->OnBreakParts(gvar.MyInfo.MyUID, e.nKey-48);
							}
						}
					}
					break;

				case '[' :
					{
						RSceneManager* pSceneMgr = (RSceneManager*)global.smgr;
						if ( pSceneMgr != NULL)
						{
							float fCoc = max( 0.0f, pSceneMgr->GetDOFCOCSize() - 1.0f);
							pSceneMgr->SetDOFCOCSize( fCoc);
							global.ui->OnSystemMsg( L"DOF COC 사이즈 설정 : {0}", FSParam(fCoc));							
						}
					}
					break;

				case ']' :
					{
						RSceneManager* pSceneMgr = (RSceneManager*)global.smgr;
						if ( pSceneMgr != NULL)
						{
							float fCoc = max( 0.0f, pSceneMgr->GetDOFCOCSize() + 1.0f);
							pSceneMgr->SetDOFCOCSize( fCoc);
							global.ui->OnSystemMsg( L"DOF COC 사이즈 설정 : {0}", FSParam(fCoc));
						}
					}
					break;

				case '{' :
					{
						RSceneManager* pSceneMgr = (RSceneManager*)global.smgr;
						if ( pSceneMgr != NULL)
						{
							float fFar = max( 0.0, pSceneMgr->GetDOFFarBlurDist() - 1000.0f);
							pSceneMgr->SetDOFFarBlurDist( fFar);
							global.ui->OnSystemMsg( L"DOF Far blur 거리 설정 : {0}", FSParam(fFar));
						}
					}
					break;

				case '}' :
					{
						RSceneManager* pSceneMgr = (RSceneManager*)global.smgr;
						if ( pSceneMgr != NULL)
						{
							float fFar = pSceneMgr->GetDOFFarBlurDist() + 1000.0f;
							pSceneMgr->SetDOFFarBlurDist( fFar);
							global.ui->OnSystemMsg( L"DOF Far blur 거리 설정 : {0}", FSParam(fFar));
						}
					}
					break;
				case 'l':
					{
						gvar.Debug.bCameraLock = !gvar.Debug.bCameraLock;

						if ( global.camera->IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW))
						{
							XCameraBackViewController* pBackViewCamera = global.camera->GetCameraController<XCameraBackViewController>();
							if (pBackViewCamera)
							{
								if(gvar.Debug.bCameraLock)
								{
									float fTemp = -1.0f;
									pBackViewCamera->StartCameraLock(gvar.Debug.uidCameraLock, gvar.Debug.strCameraLockBoneName, fTemp);
								}
								else
								{
									pBackViewCamera->EndCameraLock();
								}
							}
						}
					}
					break;
				//case '-':
				//	{
				//		++gvar.Debug.fTestCameraLockRightValue;
				//	}
				//	break;
				//case '+':
				//	{
				//		--gvar.Debug.fTestCameraLockRightValue;
				//	}
				//	break;
				//case '*':
				//	{
				//		++gvar.Debug.fTestCameraLockBackValue;
				//	}
				//	break;
				//case '/':
				//	{
				//		--gvar.Debug.fTestCameraLockBackValue;
				//	}
				//	break;
				//case 'h':
				//	{
				//		++gvar.Debug.fTestCameraLockHeightValue;
				//	}
				//	break;
				//case 'n':
				//	{
				//		--gvar.Debug.fTestCameraLockHeightValue;
				//	}
				//	break;
				}
			}
			break;
		case MWM_KEYUP:
			{
				switch (e.nKey)
				{
				case VK_SNAPSHOT:
					{
						if (e.bShift == true)
						{
							if (global.app)
							{
								XDebugInfoBuilder debugInfoBuilder;
								debugInfoBuilder.Build();

								global.app->SetClipboard(debugInfoBuilder.GetInfoForClipboard().c_str());
								mlog(debugInfoBuilder.GetInfo().c_str());

								global.ui->OnSystemMsg(L"디버그 정보가 클립보드에 복사되었습니다.");

								return true;
							}
						}
					}
					break;
				}
			}
			break;
	}

	return false;
}

bool XPlayFrame_Debugger::ToggleFreeLookCamera()
{
	if (!global.camera->GetPrimaryCamera()) return false;

	RCameraSceneNode* pNextCameraSceneNode = NULL;
	vec3 vOldCameraPos = global.camera->GetPrimaryCamera()->GetPosition();
	vec3 vOldCameraDir = global.camera->GetPrimaryCamera()->GetDirection();

	bool bEnableFog = global.camera->GetPrimaryCamera()->GetFogEnable();
	float fFogNear = global.camera->GetPrimaryCamera()->GetFogNear();
	float fFogFar = global.camera->GetPrimaryCamera()->GetFogFar();
	DWORD dwFogColor = global.camera->GetPrimaryCamera()->GetFogColor();

	bool bToggle = false;
	if (global.camera->IsPrimaryCameraType(CAMERA_MAIN_BACKVIEW))
	{
		m_nLastCameraType = CAMERA_MAIN_BACKVIEW;
		bToggle = true;
	}
	else if (global.camera->IsPrimaryCameraType(CAMERA_REPLAY_BACKVIEW))
	{
		m_nLastCameraType = CAMERA_REPLAY_BACKVIEW;
		bToggle = true;
	}

	if (bToggle)
	{
		global.camera->SetPrimaryCamera(CAMERA_FREE_LOOK);
		pNextCameraSceneNode = global.camera->GetPrimaryCamera();
		if (pNextCameraSceneNode)
		{
			pNextCameraSceneNode->SetPosition(vOldCameraPos);
			pNextCameraSceneNode->SetDirection(vOldCameraDir);
			pNextCameraSceneNode->SetFog( fFogNear, fFogFar, dwFogColor);
			pNextCameraSceneNode->SetFogEnable( bEnableFog);
		}
	}
	else if (global.camera->IsPrimaryCameraType(CAMERA_FREE_LOOK))
	{
		global.camera->SetPrimaryCamera(m_nLastCameraType);
	}

	return true;
}

void XPlayFrame_Debugger::Create()
{
#ifndef _PUBLISH
	if (global.app->GetMode() == EM_LOCAL_TEST)
	{
		global.app->GetInitializer()->Init_While_LoadingLoginCampaign();
	}
#endif

	m_pRecorder = new XRecorder();

	XNetClient* pNetClient = global.net->GetNetClient();
	if (pNetClient)
	{
		pNetClient->SetRecorder(m_pRecorder);
	}

	if (XCONST::AUTO_SAVE_REPLAY)
	{
		StartRecordReplay();
	}
}

void XPlayFrame_Debugger::Destroy()
{
	if (m_pRecorder) 
	{
		if (m_pRecorder->IsRecording()) m_pRecorder->Stop();
		SAFE_DELETE(m_pRecorder);

		XNetClient* pNetClient = global.net->GetNetClient();
		if (pNetClient)
		{
			pNetClient->ReleaseRecorder();
		}
	}

}

void XPlayFrame_Debugger::RenderRecordMessage()
{
#ifdef _PUBLISH
	return;
#endif

	PFC_THISFUNC;

	if (m_pRecorder==NULL || !m_pRecorder->IsRecording()) return;
	if (!global.system) return;

	wchar_t text[256];
	int height = REngine::GetConfig().m_nHeight;

	static MRegulator st_RegPrintRec(1.0f);
	static bool st_bPrintRec = true;

	if (st_RegPrintRec.IsReady((float)global.system->GetTimer().GetDelta()))
	{
		st_bPrintRec = !st_bPrintRec;
	}

	if (st_bPrintRec)
	{
		swprintf_s(text, L"Rec");
		XScreenDebugger::GetInstance()->Print(10, 90, text);
	}
}

void XPlayFrame_Debugger::RenderSensor()
{
	PFC_THISFUNC;

	wstring strText;
	if(gg.currentgamestate->GetWorld() == NULL ||
		gg.currentgamestate->GetWorld()->GetInfo() == NULL)
		return;

	if (!gvar.Debug.bShowSensor)
		return;

	SENSOR_INFO_MAP_BY_ID& SensorIDMap = gg.currentgamestate->GetWorld()->GetInfo()->m_mapSensorByID;

	XSensorManager* pSensorManager = gg.currentgamestate->GetSensorManager();
	for (XSensorManager::XSensorMap::iterator itor = pSensorManager->GetSensorMap().begin();
		itor != pSensorManager->GetSensorMap().end(); ++itor)
	{
		XSensor* pSensor = (*itor).second;

		SENSOR_INFO_MAP_BY_ID::iterator iter2 = SensorIDMap.find(pSensor->GetID());

		if (iter2 == SensorIDMap.end())
		{
			continue;
		}

		SENSOR_INFO Sensor = iter2->second;

		MMatrix matWorld = mat4::IDENTITY;
		matWorld.SetTranslation(Sensor.vPoint);
		RRenderHelper::RenderSphere(matWorld, Sensor.fRadius, 0xffeeddcc);

	}

	for (SENSOR_INFO_MAP_BY_ID::iterator iter = SensorIDMap.begin();
		iter != SensorIDMap.end();
		++iter)
	{
		SENSOR_INFO& Sensor = iter->second;
		MMatrix matWorld = mat4::IDENTITY;
		matWorld.SetTranslation(Sensor.vPoint);

		if (Sensor.bLocal)	RRenderHelper::RenderSphere(matWorld, Sensor.fRadius, 0xff0000ff);
		else				RRenderHelper::RenderSphere(matWorld, Sensor.fRadius, 0xff7f7f7f);
	}
}

void XPlayFrame_Debugger::RenderSoundRange()
{
	if (gvar.Debug.bShowSoundRange == false) return;

	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if (pFieldInfo==NULL) return;

	vector< FIELDSOUND_INFO>& vecSound = pFieldInfo->m_vecSound;
	for ( vector< FIELDSOUND_INFO>::iterator itr = vecSound.begin();  itr != vecSound.end();  itr++)
	{
		FIELDSOUND_INFO& info = ( *itr);

		MMatrix matWorld = mat4::IDENTITY;
		matWorld.SetTranslation( info.vPos);

		RRenderHelper::RenderSphere( matWorld, info.fMinDist, 0xff0000cc);
		RRenderHelper::RenderSphere( matWorld, info.fMaxDist, 0xff0000cc);
	}

	for ( vector< FIELDSOUND_INFO>::iterator itr = vecSound.begin();  itr != vecSound.end();  itr++)
	{
		FIELDSOUND_INFO& info = (*itr);
		XRenderHelper::DrawText( info.vPos, MCOLOR( 0x66aaaaff), info.strName.c_str());
	}
}

void XPlayFrame_Debugger::StartRecordReplay()
{
#ifdef _PUBLISH
	return;
#endif

	if (m_pRecorder->IsRecording()) return;

	m_pRecorder->Start();
	global.ui->OnSystemMsg(L"레코딩 시작");
}

void XPlayFrame_Debugger::EndRecordReplay()
{
#ifdef _PUBLISH
	return;
#endif

	if (!m_pRecorder->IsRecording()) return;

	m_pRecorder->Stop();
	global.ui->OnSystemMsg(L"레코딩 종료");
}

void XPlayFrame_Debugger::ToggleRecordReplay()
{
#ifdef _PUBLISH
	return;
#endif

	if (!m_pRecorder->IsRecording())
	{
		StartRecordReplay();
	}
	else
	{
		EndRecordReplay();
	}
}

void XPlayFrame_Debugger::ReloadConst()
{
#ifdef _PUBLISH
	return;
#endif

	XCONST::Init();
	XCameraHelper::SetDOFEnableAndParams( global.smgr, !global.smgr->IsEnableDOF() );
	XCameraHelper::SetDOFEnableAndParams( global.smgr, !global.smgr->IsEnableDOF() );
}

void XPlayFrame_Debugger::RenderCollision()
{
	MMatrix matWorld = mat4::IDENTITY;
	XMovableProcObjectChecker objectChecker;
	for (XObjectMap::iterator itor = gg.omgr->BeginItor(); itor != gg.omgr->EndItor(); ++itor)
	{
		XObject* pObj = (*itor).second;
		XModuleCollision* pModuleCollision = pObj->GetModuleCollision();
		XCharacter* pActor = pObj->GetActor();
		if( pActor==NULL ||
			pActor->m_pMesh == NULL) continue;

		//		mlog("char: pos=%.1f %.1f %.1f\n",
		//			pActor->GetPosition().x, pActor->GetPosition().y, pActor->GetPosition().z			);

		if(pModuleCollision)
			pModuleCollision->ObjColDebugRender();

		wstring n = MLocale::ConvAnsiToUTF16(pActor->m_pMesh->GetName().c_str());
		wstring nn = n.substr(n.rfind(L"/") + 1, n.rfind(L".") - n.rfind(L"/") - 1);

		// 오브젝트 충돌 모델
		matWorld.SetLocalMatrix(pObj->GetPosition(), -pObj->GetDirection(), vec3::AXISZ);

		RRenderHelper::SetRenderState(false);

		if(info.mesh_info == NULL) continue;

		// 메쉬정보
		CSMeshInfo * pMeshInfo = info.mesh_info->GetInfo(nn);
		if(pMeshInfo == NULL)
		{
			continue;
		}

		if (gvar.Debug.bShowCollision && pModuleCollision && pObj->IsModuleEntity())
		{
			// 2008-12-08 캐릭터들은 스피어에서 캡슐로 변경
			//if(pObj->GetUID() == XGetMyUID())
			//{
			//	// 내꺼는 스피어로 바꾼다.
			//	MSphere my_sphere = MSphere(pObj->GetPosition(), pModuleCollision->GetRadius());
			//	my_sphere.center.z += pModuleCollision->GetRadius();
			//	RRenderHelper::RenderSphere(my_sphere, 0x66ff00ff);
			//}
			//else
			{
				vector<CSColCapsule>::iterator itCol = pMeshInfo->m_vecColCapsules.begin();
				while(itCol != pMeshInfo->m_vecColCapsules.end())
				{
					MCapsule cap = pModuleCollision->CalCollisionCapsuleAnimation(&(*itCol), pObj);

					RRenderHelper::RenderCapsule(cap, 0x66ff00ff);

					XModuleMovable * pModuleMovable = pObj->GetModuleMovable();
					if(pModuleMovable)
					{
						bool bOffset = objectChecker.CheckCapsuleRadiusOffset(pModuleMovable->GetMovableFactor(), pObj);
						if(bOffset)
						{
							cap.radius += XCONST::COLLISION_CAPSULE_RADIUS_OFFSET;
							RRenderHelper::RenderCapsule(cap, 0x33ff00ff);
						}
					}

					itCol++;
				}
			}

			//pActor->RenderMeshCollision();
		}

		if(gvar.Debug.bShowHitEffectCapsule && pModuleCollision && pObj->IsModuleEntity())
		{
			vector<CSColCapsule>::iterator itCol = pMeshInfo->m_vecHitEffectInfo.begin();
			while(itCol != pMeshInfo->m_vecHitEffectInfo.end())
			{
				MCapsule cap = pModuleCollision->CalCollisionCapsuleAnimation(&(*itCol), pObj);

				RRenderHelper::RenderCapsule(cap, 0x6611ffff);

				itCol++;
			}
		}

		if(gvar.Debug.bShowHitCapsule)
		{
			XModuleTalent* pModuleTalent = pObj->GetModuleTalent();

			CSHitGroup * pHitGroup = pMeshInfo->FineHitGroup(pModuleCollision->GetOpenHitGroupID());
			if(pHitGroup && pModuleTalent)
			{
				vector<CSHitCapsule>::iterator itCapsule = pHitGroup->m_vecHitCapsules.begin();
				while(itCapsule != pHitGroup->m_vecHitCapsules.end())
				{
					bool bRenderCapsule = true;
					CSHitCapsule& hitCapsule = (*itCapsule);

					if (hitCapsule.IsAlwaysActive() == false)
					{
						XBaseTalent* pTalent = pModuleTalent->GetTalent();
						if (pTalent)
						{
							float fElapsedTime = pTalent->GetElapsedTime();

							bRenderCapsule = hitCapsule.CheckActiveTime(fElapsedTime);
						}
					}

					if (bRenderCapsule)
					{
						RCapsule tmpParts = hitCapsule.m_Capsule;
						tmpParts.Scale(pObj->GetScale());
						RRenderHelper::RenderCapsule(matWorld, tmpParts, 0x20FFFF00);
					}

					itCapsule++;
				}
			}
			
			// 스킬 판정 영역
			if (pModuleTalent && pActor)
			{
				if (pModuleTalent->IsActive())
				{
					XTalentInfo* pTalentInfo = pModuleTalent->GetInfo();

					for (size_t i=0; i<pTalentInfo->m_HitInfo.m_vSegments.size(); i++)
					{
						for (size_t k=0; k < pTalentInfo->m_HitInfo.m_vSegments[i].m_vCapsules.size(); k++)
						{
							MCapsule cap = pTalentInfo->m_HitInfo.m_vSegments[i].m_vCapsules[k];

							if(pTalentInfo->m_bHitCapsulePosSync)
								RRenderHelper::RenderCapsule(matWorld, cap, 0x20FF0000);
							else
								RRenderHelper::RenderCapsule(pModuleTalent->GetTalentServerStartMatrix(), cap, 0x20FF0000);
						}
					}
				}
			}
		}

		if (gvar.Debug.bShowAABB)
		{
			RRenderHelper::RenderBox(RMatrix::IDENTITY, pObj->GetActor()->GetAABB());
#ifdef _MADDUCK
			RRenderHelper::RenderBox(pObj->GetWorldTransform(), pObj->GetActor()->GetInteractionAABB(), 0xffff0000);
#endif
		}

		//		RRenderHelper::EndRenderState();
	}
}

void XPlayFrame_Debugger::RenderSectorGrid()
{
	XPlayer* user = static_cast<XPlayer*>(gg.omgr->Find(XGetMyUID()));

	if (gvar.Debug.bShowSector &&
		user)
	{
		float fHeight = 0.0f;
		float fWidth = 0.0f;

		float fStartX = 0.0f;
		float fStartY = 0.0f;

		if(gg.currentgamestate->GetWorld()->GetTerrain())
		{
			XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();

			if(pFieldInfo)
			{
				fHeight = pFieldInfo->m_fMaxY - pFieldInfo->m_fMinY;
				fWidth = pFieldInfo->m_fMaxX - pFieldInfo->m_fMinX;	

				fStartX = pFieldInfo->m_fMinX;
				fStartY = pFieldInfo->m_fMinY;
			}
			else
			{
				fHeight = gg.currentgamestate->GetWorld()->GetTerrain()->GetHeightSize();
				fWidth = gg.currentgamestate->GetWorld()->GetTerrain()->GetWidthSize();
			}
		}
		else
		{
			// 던전
		}

		MMatrix matWorldForSector = mat4::IDENTITY;
		matWorldForSector.SetTranslation(RVector3(0.0f, 0.0f, user->GetPosition().z + 1.0f));

		int nHeightIndex = (fHeight / SECTOR_SIZE) + 0.5f;
		int nWeightIndex = (fWidth / SECTOR_SIZE) + 0.5f;
		RRenderHelper::RenderGrid(matWorldForSector, SECTOR_SIZE, nWeightIndex, nHeightIndex, 0x0000FF00);	// 소형섹터
	}

	//if (gvar.Debug.bShowSector2 &&
	//	user)
	//{
	//	float fHeight = gg.currentgamestate->GetWorld()->GetTerrain()->GetHeightSize();
	//	float fWidth = gg.currentgamestate->GetWorld()->GetTerrain()->GetWidthSize();

	//	MMatrix matWorldForSector = mat4::IDENTITY;
	//	matWorldForSector.SetTranslation(RVector3(fWidth*.5f,fHeight*.5f,user->GetPosition().z));

	//	RRenderHelper::RenderGrid(matWorldForSector, HUGE_SECTOR_SIZE, fHeight/HUGE_SECTOR_SIZE, fHeight/HUGE_SECTOR_SIZE, 0x00FF0000);	// 대형섹터
	//}
}

#ifdef _MADDUCK
#include "RDebugPassRenderer.h"
#include "RCameraSceneNode.h"
#endif

void XPlayFrame_Debugger::RenderArea()
{
	if (gvar.Debug.bShowArea == false) return;

	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if (pFieldInfo==NULL) return;

	AREA_INFO_VECTOR& vecArea = pFieldInfo->m_Areas;
	for (AREA_INFO_VECTOR::iterator itor = vecArea.begin(); itor != vecArea.end(); ++itor)
	{
		AREA_INFO& info = (*itor);
		vector<XCylinder>&	veccylinder = info.veccylinder;

		RBoundingBox bbox;
		bbox.Initialize();

		for (vector<XCylinder>::iterator itor2 = veccylinder.begin(); itor2 != veccylinder.end(); ++itor2)
		{
			XCylinder& Cylionderinfo = (*itor2);

			MMatrix matWorld = mat4::IDENTITY;
			matWorld.SetTranslation(Cylionderinfo.bottom);
			RRenderHelper::RenderCylinder(matWorld, Cylionderinfo.radius, Cylionderinfo.height, 0xff0099cc);
			RRenderHelper::RenderCylinder(matWorld, Cylionderinfo.radius+AREA_BUFFER_RADIUS, Cylionderinfo.height, 0xff7799cc);
			XRenderHelper::DrawText(Cylionderinfo.bottom, MCOLOR(0x66aaaaff), info.GetName().c_str());

			XCylinder cylinder;
			cylinder = Cylionderinfo;
			cylinder.radius += AREA_BUFFER_RADIUS;
			bbox.Add(cylinder.GetAABB());
		}
#ifdef _MADDUCK
		global.smgr->GetPassRenderer< RDebugPassRenderer >()->AddDebugBox(RMatrix::IDENTITY, (RBoundingBox&)bbox, 0.1f );
#endif
	}
}