#include "StdAfx.h"
#include "TTabAnimation.h"
#include "RAnimationResource.h"
#include "TNaming.h"
#include "TAnimationInfoMgr.h"

#define ANIMATIONINFO_PLAYER "PLAYER"
#define ANIMATIONINFO_MONSTER "MONSTER"
#define ANIMATIONINFO_NPC "NPC"


bool SortingAnimationEvent( RAnimationEvent* p1, RAnimationEvent* p2 )
{
	return (p1->m_nFrame < p2->m_nFrame);
}

bool stringCompare(const string& left, const string& right)
{
	for(string::const_iterator lit = left.begin(), rit = right.begin();
		lit != left.end() && rit != right.end(); ++lit, ++rit)
	{
		if(tolower(*lit) < tolower(*rit))
			return true;
		else if(tolower(*lit) > tolower(*rit))
			return false;
	}

	if(left.size() < right.size())
		return true;

	return false;
}

TTabAnimation::TTabAnimation(void)
{
	m_pvecAniIfo = NULL;
	m_strAniTreeViewName.clear();
}

TTabAnimation::~TTabAnimation( void )
{
	DelAniInfo();
}

TCharacter* TTabAnimation::GetActor()
{
	return g_pMainApp->GetActor();
}

bool TTabAnimation::DeleteAnimation()
{
	if( GetActor() == NULL)
		return false;

	string str = m_strAniTreeViewName;

	if( GetActor()->GetCurAnimation() != NULL && str.compare(GetActor()->GetCurAnimation()->GetAliasName()) == 0)
	{//http://dev/redmine/issues/18056
	 //애니메이션 파일이 존재하고, 트리뷰의 이름과 현재 셋팅된 애니메이션 이름이 같을때만 이 구문을 수행한다.
		g_pMainApp->OnStopAnimation();
		GetActor()->InitMeshNodeNoiseRef();
		GetActor()->StopAnimation();
		GetActor()->ResetAnimationControl();
	}

	RMeshAnimationMgr* pAMgr = &GetActor()->m_pMesh->m_AniMgr;

	pAMgr->Delete(str.c_str());

	GlobalObjects::g_SaveNotifier.SetSaveAnimation(true);

	InitAnitreeView();

	return true;
}

void TTabAnimation::AddEvent()
{
	if( !GetActor() ) return;

	RAnimation* pAnimation = GetActor()->GetCurAnimation();
	if(!pAnimation) return;


	AnimationEvent EventEdit;
	EventEdit.FrametextBox->Text = String::Format("{0}", GetActor()->GetAnimationFrame());

	vector<string> vecEffectList;
	GetCustomEffectList(vecEffectList);
	for(vector<string>::iterator itEffectList = vecEffectList.begin(); itEffectList != vecEffectList.end(); itEffectList++)
	{
		EventEdit.CustomEffectNameListBox->Items->Add(String::Format("{0}", gcnew String((*itEffectList).c_str())));
	}
	
	EventEdit.ShowDialog();

	System::Windows::Forms::DialogResult res = EventEdit.DialogResult;
	if (res == System::Windows::Forms::DialogResult::OK)
	{
		string cstr1 = MStringToCharPointer(EventEdit.FrametextBox->Text);
		int nFrame = atoi(cstr1.c_str());

		//////////////////////////////////////////////////////////////////////////
		// CustomEvent
		cstr1.clear();
		cstr1 = MStringToCharPointer(EventEdit.EffectNameTextBox->Text);
		if(cstr1 != "")
		{
			RAnimationEvent* pEvent = new RAnimationEvent;

			pEvent->m_nFrame = nFrame;
			pEvent->m_strEvent = "sh_custom";
			pEvent->m_strParam1 = cstr1;
			pEvent->m_strParam2 = MStringToCharPointer(EventEdit.CustomParamtextBox->Text);
			pEvent->m_strParam3 = MStringToCharPointer(EventEdit.CustomBoneNametextBox->Text);
			pEvent->m_bUnconditional = EventEdit.unconditionalcheckBox->Checked;

			pAnimation->m_animationEvents.push_back(pEvent);
		}


		//////////////////////////////////////////////////////////////////////////
		// NormalEvent
		cstr1.clear();
		cstr1 = MStringToCharPointer(EventEdit.EventNameTextBox->Text);
		if(cstr1 != "")
		{
			bool bSwordTrailEvent = false;
			if(CheckSwordTrailEvent(cstr1, pAnimation, bSwordTrailEvent) == false)
			{
				return;
			}

			RAnimationEvent* pEvent = new RAnimationEvent;

			pEvent->m_nFrame = nFrame;
			pEvent->m_strEvent = cstr1;
			pEvent->m_strParam1 = MStringToCharPointer(EventEdit.GetParam1Text());
			pEvent->m_strParam2 = MStringToCharPointer(EventEdit.GetParam2Text());
			pEvent->m_strParam3 = MStringToCharPointer(EventEdit.Param3textBox->Text);
			pEvent->m_bUnconditional = EventEdit.unconditionalcheckBox->Checked;

			pAnimation->m_animationEvents.push_back(pEvent);

			if(bSwordTrailEvent)
			{
				// 캐릭터 툴에서만 쓰일 ID
				string strModelName = TNaming::GetMeshAliasName(g_pMainApp->GetActor());
				ReSetSwordTrailEventID(pAnimation, cstr1);
				g_pMainApp->m_SwordTrailSampling.StartSwordTrailSampling(strModelName, pAnimation);
			}
		}

		InitEventList();

		GlobalObjects::g_SaveNotifier.SetSaveAnimation(true);
	}
}

void TTabAnimation::ModifyEvent(int index)
{
	if( !GetActor() ) return;

	RAnimation* pAnimation = g_pMainApp->GetActor()->GetCurAnimation();
	if(!pAnimation) return;

	RAnimationEvent* pEvent = m_vecCurrentEventList[index];
	//RAnimationEvent* pEvent = pAnimation->m_animationEvents[index];
	if(!pEvent) return;

	AnimationEvent EventEdit;
	EventEdit.FrametextBox->Text	= String::Format("{0}",pEvent->m_nFrame);

	vector<string> vecEffectList;
	GetCustomEffectList(vecEffectList);
	for(vector<string>::iterator itEffectList = vecEffectList.begin(); itEffectList != vecEffectList.end(); itEffectList++)
	{
		EventEdit.CustomEffectNameListBox->Items->Add(String::Format("{0}", gcnew String((*itEffectList).c_str())));
	}


	if(pEvent->m_strEvent == "sh_custom")
	{
		EventEdit.EffectNameTextBox->Text = gcnew String(pEvent->m_strParam1.c_str());
		EventEdit.CustomParamtextBox->Text = gcnew String(pEvent->m_strParam2.c_str());
		EventEdit.CustomBoneNametextBox->Text = gcnew String(pEvent->m_strParam3.c_str());
		EventEdit.AnimationEventTab->SelectedIndex = 0;
	}
	else
	{
		EventEdit.EventNameTextBox->Text = gcnew String(pEvent->m_strEvent.c_str());
		EventEdit.SetParam1Text(gcnew String(pEvent->m_strParam1.c_str()));
		EventEdit.SetParam2Text(gcnew String(pEvent->m_strParam2.c_str()));
		EventEdit.Param3textBox->Text = gcnew String(pEvent->m_strParam3.c_str());
		EventEdit.AnimationEventTab->SelectedIndex = 1;
	}

	EventEdit.unconditionalcheckBox->Checked = pEvent->m_bUnconditional;
	EventEdit.ShowDialog();
	System::Windows::Forms::DialogResult res = EventEdit.DialogResult;
	if (res == System::Windows::Forms::DialogResult::OK)
	{
		string cstr1 = MStringToCharPointer(EventEdit.FrametextBox->Text);
		int nFrame = atoi(cstr1.c_str());

		//////////////////////////////////////////////////////////////////////////
		// CustomEvent
		cstr1.clear();
		cstr1 = MStringToCharPointer(EventEdit.EffectNameTextBox->Text);
		if(cstr1 != "")
		{
			RAnimationEvent* pEventCustom;
			if(pEvent->m_strEvent != "sh_custom")
			{
				pEventCustom = new RAnimationEvent;
				pAnimation->m_animationEvents.push_back(pEventCustom);
			}
			else
			{
				pEventCustom = pEvent;
			}

			pEventCustom->m_nFrame = nFrame;
			pEventCustom->m_strEvent = "sh_custom";
			pEventCustom->m_strParam1 = cstr1;
			pEventCustom->m_strParam2 = MStringToCharPointer(EventEdit.CustomParamtextBox->Text);
			pEventCustom->m_strParam3 = MStringToCharPointer(EventEdit.CustomBoneNametextBox->Text);
			pEventCustom->m_bUnconditional = EventEdit.unconditionalcheckBox->Checked;
		}


		//////////////////////////////////////////////////////////////////////////
		// NormalEvent
		cstr1.clear();
		cstr1 = MStringToCharPointer(EventEdit.EventNameTextBox->Text);
		if(cstr1 != "")
		{
			bool bSwordTrailEvent = false;
			if(CheckSwordTrailEvent(cstr1, pAnimation, bSwordTrailEvent) == false)
			{
				return;
			}

			RAnimationEvent* pEventNormal;
			if(pEvent->m_strEvent == "sh_custom")
			{
				pEventNormal = new RAnimationEvent;
				pAnimation->m_animationEvents.push_back(pEventNormal);
			}
			else
			{
				pEventNormal = pEvent;
			}

			pEventNormal->m_nFrame = nFrame;
			pEventNormal->m_strEvent = cstr1;
			pEventNormal->m_strParam1 = MStringToCharPointer(EventEdit.GetParam1Text());
			pEventNormal->m_strParam2 = MStringToCharPointer(EventEdit.GetParam2Text());
			pEventNormal->m_strParam3 = MStringToCharPointer(EventEdit.Param3textBox->Text);
			pEventNormal->m_bUnconditional = EventEdit.unconditionalcheckBox->Checked;

			if(bSwordTrailEvent)
			{
				// 캐릭터 툴에서만 쓰일 ID
				string strModelName = TNaming::GetMeshAliasName(g_pMainApp->GetActor());
				ReSetSwordTrailEventID(pAnimation, cstr1);
				g_pMainApp->m_SwordTrailSampling.StartSwordTrailSampling(strModelName, pAnimation);
			}
		}

		int nSelectIndex = GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->SelectedIndex;
		int nTopIndex = GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->TopIndex;

		GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->SelectedIndex = nSelectIndex;
		GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->TopIndex = nTopIndex;

		ModifyEventList(index);

		GlobalObjects::g_SaveNotifier.SetSaveAnimation(true);
	}
}

void TTabAnimation::DeleteEvent(int index)
{
	if( !GetActor() ) return;

	RAnimation* pAnimation = GetActor()->GetCurAnimation();
	int nIdex = GetAnimationEventIndex(pAnimation, index);
	if(nIdex!=LB_ERR && pAnimation && nIdex<(int)pAnimation->m_animationEvents.size())
	{
		string strEvent = pAnimation->m_animationEvents[nIdex]->m_strEvent;
		string strIndex = pAnimation->m_animationEvents[nIdex]->m_strParam3;

		if(strIndex.empty() == false &&
			(strEvent == "sh_sword_trail_start" ||
			strEvent == "sh_sword_trail_end"))
		{
			System::Windows::Forms::MessageBox::Show("연결된 검광 이벤트를 삭제합니다.", "안내",MessageBoxButtons::OK);
			vector<RAnimationEvent*>::iterator it = pAnimation->m_animationEvents.begin();
			while(it != pAnimation->m_animationEvents.end())
			{
				if((*it)->m_strParam3 == strIndex)
				{
					SAFE_DELETE(*it);
					it = pAnimation->m_animationEvents.erase(it);

					continue;
				}

				++it;
			}

			string strModelName = TNaming::GetMeshAliasName(g_pMainApp->GetActor());
			g_pMainApp->m_SwordTrailSampling.DeleteSwordTrailSampling(strModelName, pAnimation);
		}	
		else
		{
			delete pAnimation->m_animationEvents[nIdex];
			pAnimation->m_animationEvents.erase(pAnimation->m_animationEvents.begin()+nIdex);
		}
	}

	int nTopIndex = GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->TopIndex;

	DeleteEventList(index);

	if(index >= GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->Items->Count)
		index = GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->Items->Count - 1;

	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->SelectedIndex = index;
	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->TopIndex = nTopIndex;

	GlobalObjects::g_SaveNotifier.SetSaveAnimation(true);
}

void TTabAnimation::InitEventList()
{
	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->Items->Clear();

	if( !GetActor() ) return;

	RAnimation* pAni = GetActor()->GetCurAnimation();

	if( !pAni) return;


	vector<RAnimationEvent*> outAnimationEvents;
	GetSortAnimationEvent(pAni->m_animationEvents, outAnimationEvents);
	for(int i=0;i<(int)outAnimationEvents.size();i++)
	{
		GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->Items->Add(
			String::Format("{0} : {1} - {2}",
			outAnimationEvents[i]->m_nFrame,
			gcnew String(outAnimationEvents[i]->m_strEvent.c_str()),
			gcnew String(outAnimationEvents[i]->m_strParam1.c_str())));
	}

	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->AllChecked(true);

	// 검광 이벤트 체크
	ReSetSwordTrailEventID(pAni, string(""));

	// 애니메이션 이벤트 리셋
	GetActor()->ClearAnimationEvent();
	for(vector<RAnimationEvent*>::iterator itAniEvent = pAni->m_animationEvents.begin(); itAniEvent != pAni->m_animationEvents.end(); itAniEvent++)
	{
		ANIMATION_EVENT aniEvent;
		aniEvent.pAnimationEvent = *itAniEvent;
		GetActor()->AddAnimationEvent(aniEvent);
	}
}

void TTabAnimation::DeleteEventList(int index)
{
	vector<bool> vecChecked;
	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->GetCheckedEventList(vecChecked);
	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->Items->Clear();

	int nCount = 0;
	for(vector<bool>::iterator it = vecChecked.begin(); it != vecChecked.end(); ++it)
	{
		if(nCount == index)
		{
			vecChecked.erase(it);
			break;
		}
		++nCount;
	}

	if( !GetActor() ) return;

	RAnimation* pAni = GetActor()->GetCurAnimation();

	if( !pAni) return;

	vector<RAnimationEvent*> outAnimationEvents;
	GetSortAnimationEvent(pAni->m_animationEvents, outAnimationEvents);
	for(int i=0;i<(int)outAnimationEvents.size();i++)
	{
		GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->Items->Add(
			String::Format("{0} : {1} - {2}",
			outAnimationEvents[i]->m_nFrame,
			gcnew String(outAnimationEvents[i]->m_strEvent.c_str()),
			gcnew String(outAnimationEvents[i]->m_strParam1.c_str())));

		GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->CheckedEvent(i, vecChecked[i]);
	}

	// 검광 이벤트 체크
	ReSetSwordTrailEventID(pAni, string(""));

	// 애니메이션 이벤트 리셋
	GetActor()->ClearAnimationEvent();
	for(vector<RAnimationEvent*>::iterator itAniEvent = pAni->m_animationEvents.begin(); itAniEvent != pAni->m_animationEvents.end(); itAniEvent++)
	{
		ANIMATION_EVENT aniEvent;
		aniEvent.pAnimationEvent = *itAniEvent;
		GetActor()->AddAnimationEvent(aniEvent);
	}
}

/************************************************************************/
/* 애니메이션 종류별로 tree 구조로...
/************************************************************************/
void TTabAnimation::InitAnitreeView()
{
	GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Nodes->Clear();
	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->Items->Clear();

	if( !GetActor() )
		return ;

	GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->BeginUpdate();

	bool bPlayerNpc = false;

	if( ( MODEL_TYPE_NPC == g_pMainApp->m_TabModel.GetModelType() ) && GetActor()->m_pMesh )
	{
		// 플레이어 npc인지 체크
		if( GetActor()->m_pMesh->GetName().find( "hf" ) != string::npos || GetActor()->m_pMesh->GetName().find( "hm" ) != string::npos )
			bPlayerNpc = true;	
	}

	// 기본 노드를 추가하지 말고 애니메이션을 돌면서 규칙에 따라 이름 앞을 따라 노드를 생성
	// 애니메이션 파일이 존재하는지.
	if( GetActor() && GetActor()->m_pMesh )
	{
		RMeshAnimationMgr* pAMgr = &GetActor()->m_pMesh->m_AniMgr;

		vector<string> strErrorMessage;
		RAnimationHashList_Iter it;
		string strAniName;
		string pureAniName;

		for( it = pAMgr->m_NodeHashList.begin() ; it != pAMgr->m_NodeHashList.end() ; ++it )
		{
			RAnimation* pAnimation = *it;
			if( NULL == pAnimation )
				continue;

			strAniName		= pAnimation->GetAliasName();
			pureAniName		= strAniName.substr( 0, strAniName.find("_") );

			String^ strNodeName;

			if( ( MODEL_TYPE_MONSTER == g_pMainApp->m_TabModel.GetModelType() ) || 
				( MODEL_TYPE_NPC == g_pMainApp->m_TabModel.GetModelType() ) && ( false == bPlayerNpc ) )
				strNodeName		= "none";
			else
			{
				if( pureAniName != strAniName )
					strNodeName		= gcnew String( pureAniName.c_str() );
				else
					strNodeName		= gcnew String( strAniName.c_str() );;//"none";
			}
			

			TreeNode^ tn = SearchNode( GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Nodes, strNodeName );
			if( nullptr == tn )
			{
				GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Nodes->Add( strNodeName );
				tn = SearchNode( GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Nodes, strNodeName );

				if( nullptr == tn )
					continue;
			}

			String^ Name = gcnew String(strAniName.c_str() );
			tn->Nodes->Add( Name );

			//파일이 존재하는냐?
			//노드 백컬러 교체.
			if( pAnimation->GetID() < 0 || pAnimation->GetResource() == NULL || !ExistFile(pAnimation->GetResource()->GetName().c_str()))
				tn->BackColor = System::Drawing::Color::LightGray;
		}
	}

	//sort
	GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Sort();

	if (g_pMainApp->m_TabModel.GetModelType() == MODEL_TYPE_MONSTER ||
		g_pMainApp->m_TabModel.GetModelType() == MODEL_TYPE_NPC)
	{
		TreeNode^ tn = SearchNode(GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Nodes, "none");
		if( tn != nullptr)	tn->ExpandAll();
	}

	GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->EndUpdate();
	GlobalObjects::g_pMainForm->m_AnimationTabPage->AnimationInfoBtnEnable( true );


/*
	GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Nodes->Clear();
	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->Items->Clear();

	if( !GetActor() ) return;

	GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->BeginUpdate();

	bool bPlayerNpc = false;

	if(g_pMainApp->m_TabModel.GetModelType() == MODEL_TYPE_NPC && 
		GetActor() && GetActor()->m_pMesh)
	{
		// 플레이어 npc인지 체크
		if(GetActor()->m_pMesh->GetName().find("hf") != string::npos ||
			GetActor()->m_pMesh->GetName().find("hm") != string::npos)
		{
			bPlayerNpc = true;	
		}		
	}

	int weaponmax = (int)WEAPON_TYPE_MAX;
	if (g_pMainApp->m_TabModel.GetModelType() == MODEL_TYPE_MONSTER ||
		(g_pMainApp->m_TabModel.GetModelType() == MODEL_TYPE_NPC && bPlayerNpc == false))
	{
		weaponmax = 1;
	}


	for( int i = 0; i < weaponmax; i++)
	{
		String^ nodename = gcnew String(TStrings::WeaponType(i));
		GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Nodes->Add(nodename);
	}

	if(weaponmax > 1)
	{
		// 1보다 큰건 플레이어라는 뜻이다.
		// 플레이어에만 있는 페이스 애니메이션 추가
		GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Nodes->Add(gcnew String("facial"));
	}

	// 애니메이션 파일이 존재하는지.
	if( GetActor() && GetActor()->m_pMesh)
	{
		RMeshAnimationMgr* pAMgr = &GetActor()->m_pMesh->m_AniMgr;

		vector<string> strErrorMessage;

		for(RAnimationHashList_Iter it = pAMgr->m_NodeHashList.begin(); it != pAMgr->m_NodeHashList.end();it++)
		{
			RAnimation* pAnimation = *it;
			if(pAnimation){

				string strAniName = pAnimation->GetAliasName();

				int count = GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Nodes->Count;
				if (count > 0)
				{
					TreeNode^ temp = nullptr;

					// 앞글자를 따온다.
					string pureAniName = strAniName.substr(0, strAniName.find("_"));

					TreeNode^ tn = SearchNode(GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Nodes, gcnew String(pureAniName.c_str()));
					if (g_pMainApp->m_TabModel.GetModelType() == MODEL_TYPE_MONSTER ||
						(g_pMainApp->m_TabModel.GetModelType() == MODEL_TYPE_NPC && bPlayerNpc == false))
					{
						tn = nullptr;
					}

					if( tn != nullptr)
					{
						String^ Name = gcnew String(strAniName.c_str() );
						temp = tn->Nodes->Add(Name);
					}
					else 
					{
						bool bNone = true;
						// npc 플레이어만...
						string strMeshName = GetActor()->m_pMesh->GetName();
						if(strMeshName.find("hm_") != std::string::npos ||
							strMeshName.find("hf_") != std::string::npos)
						{
							if(pureAniName != strAniName)
							{
								bNone = false;
								
								// 없으면 none으로 붙음(삭제)
								// 새로 추가함.(http://dev/redmine/issues/17151)
								GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Nodes->Add(gcnew String(pureAniName.c_str() ));
								TreeNode^ tn = SearchNode(GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Nodes, gcnew String(pureAniName.c_str()));
								if( tn != nullptr) 
								{
									String^ Name = gcnew String(strAniName.c_str() );
									temp = tn->Nodes->Add(Name);
								}
							}
						}


						if(bNone)
						{
							// 이름에 '_'가 없으면 그냥 순수 이름이면 none에 붙인다.
							TreeNode^ tn = SearchNode(GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Nodes, "none");
							if( tn != nullptr) 
							{
								String^ Name = gcnew String(strAniName.c_str() );
								temp = tn->Nodes->Add(Name);
							}
						}
					}


					//파일이 존재하는냐?
					//노드 백컬러 교체.
					RAnimation* pAnimation = GetAnimation(strAniName.c_str());
					if( pAnimation == NULL
						|| pAnimation->GetID() < 0
						|| pAnimation->GetResource() == NULL
						|| !ExistFile(pAnimation->GetResource()->GetName().c_str()))
					{
						temp->BackColor = System::Drawing::Color::LightGray;
					}
				}
			}
		}
	}

	//sort
	GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Sort();

	if (g_pMainApp->m_TabModel.GetModelType() == MODEL_TYPE_MONSTER ||
		g_pMainApp->m_TabModel.GetModelType() == MODEL_TYPE_NPC)
	{
		TreeNode^ tn = SearchNode(GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->Nodes, "none");
		if( tn != nullptr)	tn->ExpandAll();
	}

	GlobalObjects::g_pMainForm->m_AnimationTabPage->AnitreeView->EndUpdate();
	GlobalObjects::g_pMainForm->m_AnimationTabPage->AnimationInfoBtnEnable(true);
*/
}

RAnimation* TTabAnimation::GetAnimation(const char* aniName)
{
	if( !GetActor() ) return NULL;

	RAnimation* animation = GetActor()->GetAnimation(aniName);
	return animation;
}

RAnimation* TTabAnimation::GetAnimation(System::String^ Aniname)
{
	const char* cstr1 = MStringToCharPointer(Aniname);

	string str = cstr1;

	RAnimation* animation = GetActor()->GetAnimation((char*)cstr1);
	MFreeCharPointer(cstr1);

	return animation;
}

void TTabAnimation::InitAnimationInfo()
{
	if( !GetActor() ) return;
	
	RAnimation* pAnimation = GetActor()->GetCurAnimation();
	if( !pAnimation ) return;

	if(m_pvecAniIfo == NULL) return;

	GlobalObjects::g_pMainForm->m_AnimationTabPage->AniNamelabel->Text = gcnew String( pAnimation->GetAliasName().c_str() );

	float vsp = pAnimation->GetSpeed() * ANISPEED;

	GlobalObjects::g_pMainForm->AniSpeedtrackBar->Value = (int)vsp;

	ANIINFOVECTOR::iterator it;
	for(it = m_pvecAniIfo->begin(); it != m_pvecAniIfo->end(); it++)
	{
		AniInfo* aniii = *it;
		if( aniii->name == pAnimation->GetAliasName())
		{
			GlobalObjects::g_pMainForm->m_AnimationTabPage->LoopcheckBox->Enabled = false;

			AnimationLoopType MLoopType = RAniLoopType_HoldLastFrame;

			if( aniii )
			{
				if(strcmp(aniii->loop.c_str(),"loop")==0) {
					MLoopType = RAniLoopType_Loop;
				}
				else if(strcmp(aniii->loop.c_str(),"lastframe")==0) {
					MLoopType = RAniLoopType_HoldLastFrame;
				}
			}

			if( pAnimation->GetAnimationLoopType() != MLoopType)
			{
				mlog("정보 : 현재 애니에이션 루프타입이 aniinfo와 달라 수정합니다. 저장해주세요!! \r");

				pAnimation->SetAnimationLoopType(MLoopType);
				//g_pMainApp->SetAnimation(pAnimation);

				GlobalObjects::g_SaveNotifier.SetSaveAnimation(true);
			}

			break;
		}
	}

	if( it == m_pvecAniIfo->end())
		GlobalObjects::g_pMainForm->m_AnimationTabPage->LoopcheckBox->Enabled = true;

	GlobalObjects::g_pMainForm->m_AnimationTabPage->LoopcheckBox->Checked= (pAnimation->GetAnimationLoopType()==RAniLoopType_Loop)?true:false;
	GlobalObjects::g_pMainForm->AniSpeednumericUpDown->Value = Decimal(pAnimation->GetSpeed());
}

bool TTabAnimation::LoadAniInfo(std::vector<AniInfo*>& vecAniInfo, const char* Model)
{
	vecAniInfo.clear();

	MXml xml;
	if (xml.LoadFile("aniinfo.xml") == false) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pRootElement = docHandle.FirstChild("XML").Element();

	MXmlElement* pChildElement = pRootElement->FirstChildElement(Model);
	if( pChildElement)
	{
		MXmlElement* ppChildElement = pChildElement->FirstChildElement();

		for( ppChildElement; ppChildElement != NULL; ppChildElement=ppChildElement->NextSiblingElement() )
		{
			if(strcmp(ppChildElement->Value(), "AddAnimation")==0)
			{
				AniInfo* aniinfo = new AniInfo;
				_Attribute(aniinfo->name, ppChildElement,	"name", &xml);
				if( !_Attribute(aniinfo->loop, ppChildElement,	"motion_loop_type", &xml) )
				{
					delete aniinfo;
					continue;
				}

				_Attribute(aniinfo->desc, ppChildElement,	"description", &xml);
				vecAniInfo.push_back(aniinfo);
			}
		}
	}

	return true;	
}

bool TTabAnimation::SetAniInfo( System::String^ str1 )
{
	if(str1 == nullptr) return false;

	str1 = str1->ToUpper(); // 대문자로 변환
	string str = MStringToCharPointer(str1);

	m_pvecAniIfo = NULL;

	if(ANIMATIONINFO_PLAYER == str)
	{
		m_pvecAniIfo = (&m_aniInfoData.vecPlayerAniInfo);
	}
	else if(ANIMATIONINFO_MONSTER == str)
	{
		m_pvecAniIfo = (&m_aniInfoData.vecMonsterAniInfo);
	}
	else if(ANIMATIONINFO_NPC == str)
	{
		m_pvecAniIfo = (&m_aniInfoData.vecNPCAniInfo);
	}

	return true;
}
// TODO: 
// [4/19/2007 madduck] - none 애니메이션 네임에 none가 붙어서 들어오네...
bool TTabAnimation::LoadAniFile(const char* FileName)
{
	if( !GetActor()) return false;

	if(m_pvecAniIfo == NULL) return false;

	string pureName = GetPureFileName(FileName);
	string FolderName = GetFolderName(FileName);
	if (g_pMainApp->m_TabModel.GetModelType() == MODEL_TYPE_PLAYER)
	{
		FolderName = GetRemoveUnderBarlast(pureName);
	}

	int cs = FolderName.size() + 1;//'_'
	string strAnimationName = pureName.substr(cs, pureName.size()-cs-4);

	GetActor()->InitMeshNodeNoiseRef();
	GetActor()->StopAnimation();
	//////////////////////////////////////////////////////////////////////////
	//애니가 지정되어 있는데 넣는 경우 
	RAnimation* pAnimation = GetActor()->GetAnimation((char*)strAnimationName.c_str());
	if( pAnimation )
	{
		GetActor()->m_pMesh->m_AniMgr.Delete((char*)strAnimationName.c_str());
		pAnimation = NULL;
		mlog("정보 : 이름이 같아 기존 애니메이션을 지우고 새로운 파일로 대체 합니다. - %s \r", strAnimationName.c_str());
	}

	pAnimation = GetActor()->m_pMesh->m_AniMgr.Add(strAnimationName.c_str(),(char*)FileName );
	if (!pAnimation)
		return false;

	pAnimation->GetResource()->BuildAll();

	//// [3/28/2007 madduck] - 현재 애니메이션의 루프여부를 애니인포에서 얻어와서 세팅해준다.
	////RAnimation* pAnimation = m_pActor->GetAnimation((char*)strAnimationName.c_str());
	//if( !pAnimation ) return false;

	ANIINFOVECTOR::iterator it;
	for(it = m_pvecAniIfo->begin(); it != m_pvecAniIfo->end(); it++)
	{
		AniInfo*  aniii = *it;
		if( aniii->name == pAnimation->GetAliasName())
		{
			AnimationLoopType MLoopType = RAniLoopType_HoldLastFrame;

			if( aniii )
			{
				if(strcmp(aniii->loop.c_str(),"loop")==0) {
					MLoopType = RAniLoopType_Loop;
				}
				else if(strcmp(aniii->loop.c_str(),"lastframe")==0) {
					MLoopType = RAniLoopType_HoldLastFrame;
				}
			}
			pAnimation->SetAnimationLoopType(MLoopType);

			break;
		}
	}

	//// 흠 마지막이 이상하려나... [5/7/2007 madduck]
	////if( it == m_vecAniIfo.end())
	////	aniii = NULL;

	//g_pMainApp->SetAnimation(pAnimation);
	g_pMainApp->SetAnimation(strAnimationName);

	InitAnitreeView();
	
	InitAnimationInfo();

	GlobalObjects::g_SaveNotifier.SetSaveAnimation(true);

	return true;
}

void TTabAnimation::GetCustomEffectList( vector<string>& vecEffectList )
{
	for(vector<XEffectInfo *>::iterator it = g_pMainApp->m_pEffectInfoMgr->m_vecEffectInfo.begin(); it != g_pMainApp->m_pEffectInfoMgr->m_vecEffectInfo.end(); it++)
	{
		vecEffectList.push_back((*it)->m_strEffectName);
	}

	sort(vecEffectList.begin(), vecEffectList.end(), stringCompare);

}

void TTabAnimation::SelectedEventList(int index)
{
	if( !GetActor() ) return;

	RAnimation* pAni = GetActor()->GetCurAnimation();
	if( !pAni) return;

	if (index < 0 || index >= (int)pAni->m_animationEvents.size()) return;

	RAnimationEvent* pAnimationEvent = m_vecCurrentEventList[index];
	if (pAnimationEvent == NULL) return;

	g_pMainApp->m_NormalAnimation.CheckAnimationEvent(pAnimationEvent->m_nFrame);
	GetActor()->SetAnimationFrame(pAnimationEvent->m_nFrame);
	GlobalObjects::g_pMainForm->m_FrameSliderController->pressPauseButton();

	// 선택된 이벤트 플레이
	g_pMainApp->AnimationEventPlay(pAnimationEvent);
}

bool TTabAnimation::CheckSwordTrailEvent( string strEventName, RAnimation* pAnimation, bool& bSwordTrailEvent )
{
	bSwordTrailEvent = false;

	if(strEventName == "sh_sword_trail_start" ||
		strEventName == "sh_sword_trail_end")
	{
		// 이동더미 체크 박스가 꺼져 있어야 한다.
		if(g_pMainApp->m_bMoveDummy)
		{
			System::Windows::Forms::MessageBox::Show(L"죄송합니다. 검광 이벤트를 추가및 수정을 할려면 [MoveDummy] 체크 박스를 꺼주셔야 합니다.");

			return false;
		}

		bSwordTrailEvent = true;
	}
	
	return true;
}

void TTabAnimation::DelAniInfo()
{
	for(ANIINFOVECTOR::iterator itPlayer = m_aniInfoData.vecPlayerAniInfo.begin(); itPlayer != m_aniInfoData.vecPlayerAniInfo.end(); itPlayer++)
	{
		SAFE_DELETE(*itPlayer);
	}

	for(ANIINFOVECTOR::iterator itMonster = m_aniInfoData.vecMonsterAniInfo.begin(); itMonster != m_aniInfoData.vecMonsterAniInfo.end(); itMonster++)
	{
		SAFE_DELETE(*itMonster);
	}

	m_aniInfoData.vecPlayerAniInfo.clear();
	m_aniInfoData.vecMonsterAniInfo.clear();
}

void TTabAnimation::InitAniInfo()
{
	LoadAniInfo(m_aniInfoData.vecPlayerAniInfo, ANIMATIONINFO_PLAYER);
	LoadAniInfo(m_aniInfoData.vecMonsterAniInfo, ANIMATIONINFO_MONSTER);
	LoadAniInfo(m_aniInfoData.vecNPCAniInfo, ANIMATIONINFO_NPC);
}

CHECK_DUMMY_LOC_RESULT TTabAnimation::CheckAnimationDummyLocNode( const char* aniName )
{
	RAnimation* pAnimation = GetAnimation(aniName);
	if(pAnimation == NULL)
		return CDLR_NO_ANIMATION;

	RAnimationNode* pAnimationNode = pAnimation->GetResource()->GetNode("dummy_loc");
	if(pAnimationNode == NULL)
		return CDLR_NO_DUMMY_NODE;

	if(CheckAnimationDummyLocPositionkeyTrack(pAnimationNode) == false &&
		CheckAnimationDummyLocRotationkeyTrack(pAnimationNode) == false &&
		CheckAnimationDummyLocScalekeyTrack(pAnimationNode) == false &&
		CheckAnimationDummyLocViskeyTrack(pAnimationNode) == false)
	{
		// 쓰지도 않으면서 존재한다.
		return CDLR_NOT_USE;
	}

	return CDLR_OK;
}
bool TTabAnimation::CheckAnimationDummyLocPositionkeyTrack( RAnimationNode* pAnimationNode )
{
	if(pAnimationNode == NULL)
		return true;

	int nCount = pAnimationNode->m_positionKeyTrack.size();
	for(int i = 0; i < nCount; ++i)
	{
		RPosKey posKey = pAnimationNode->m_positionKeyTrack[i];
		if(posKey != vec3::ZERO)
		{
			return true;
		}
	}

	return false;
}

bool TTabAnimation::CheckAnimationDummyLocRotationkeyTrack( RAnimationNode* pAnimationNode )
{
	if(pAnimationNode == NULL)
		return true;

	MQuat emptyQuat;
	emptyQuat.x = emptyQuat.y = emptyQuat.z = 0.0f;
	emptyQuat.w = 1.0f;

	int nCount = pAnimationNode->m_rotationKeyTrack.size();
	for(int i = 0; i < nCount; ++i)
	{
		RQuatKey rotKey = pAnimationNode->m_rotationKeyTrack[i];
		if(rotKey != emptyQuat)
		{
			return true;
		}
	}

	return false;
}

bool TTabAnimation::CheckAnimationDummyLocScalekeyTrack( RAnimationNode* pAnimationNode )
{
	if(pAnimationNode == NULL)
		return true;

	int nCount = pAnimationNode->m_scaleKeyTrack.size();
	for(int i = 0; i < nCount; ++i)
	{
		RScaleKey scaleKey = pAnimationNode->m_scaleKeyTrack[i];
		if(scaleKey != vec3::ZERO)
		{
			return true;
		}
	}

	return false;
}

bool TTabAnimation::CheckAnimationDummyLocViskeyTrack( RAnimationNode* pAnimationNode )
{
	if(pAnimationNode == NULL)
		return true;

	int nCount = pAnimationNode->m_visKeyTrack.size();
	for(int i = 0; i < nCount; ++i)
	{
		RVisKey visKey = pAnimationNode->m_visKeyTrack[i];
		if(visKey.v != 0)
		{
			return true;
		}
	}

	return false;
}

void TTabAnimation::ReSetSwordTrailEventID( RAnimation* pAnimation, string& strEvent )
{
	if(pAnimation == NULL)
	{
		return;
	}

	int nSwordTrailEventID = 0;
	bool bStart = false;
	for(vector<RAnimationEvent*>::iterator it = pAnimation->m_animationEvents.begin(); it != pAnimation->m_animationEvents.end(); it++)
	{
		if((*it)->m_strEvent == "sh_sword_trail_start")
		{
			if(bStart == false)
			{
				bStart = true;
			}

			nSwordTrailEventID++;

			char chTemp[8] = {0,};
			sprintf_s(chTemp, "%d", nSwordTrailEventID);
			(*it)->m_strParam3 = chTemp;
		}
		else if((*it)->m_strEvent == "sh_sword_trail_end")
		{
			if(bStart == false)
			{
				nSwordTrailEventID++;
			}

			bStart = false;

			char chTemp[8] = {0,};
			sprintf_s(chTemp, "%d", nSwordTrailEventID);
			(*it)->m_strParam3 = chTemp;
		}
	}
}

void TTabAnimation::ModifyEventList( int index )
{
	if( !GetActor() ) return;

	RAnimation* pAni = GetActor()->GetCurAnimation();

	if( !pAni) return;

	if(index >= GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->Items->Count)
		return;

	// 검광 이벤트 체크
	ReSetSwordTrailEventID(pAni, string(""));


	// 애니메이션 이벤트 리셋
	GetActor()->ClearAnimationEvent();
	int nIndex = GetAnimationEventIndex(pAni, index);
	int nCount = 0;
	for(vector<RAnimationEvent*>::iterator itAniEvent = pAni->m_animationEvents.begin(); itAniEvent != pAni->m_animationEvents.end(); itAniEvent++)
	{
		ANIMATION_EVENT aniEvent;
		aniEvent.bEventDone = true;
		aniEvent.pAnimationEvent = *itAniEvent;

		if(nCount == nIndex)
			aniEvent.bEventDone = false;

		GetActor()->AddAnimationEvent(aniEvent);

		nCount++;
	}

	// 이벤트 리스트 수정
	vector<bool> vecChecked;
	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->GetCheckedEventList(vecChecked);
	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->Items->Clear();

	vector<RAnimationEvent*> outAnimationEvents;
	GetSortAnimationEvent(pAni->m_animationEvents, outAnimationEvents);
	for(int i=0;i<(int)outAnimationEvents.size();i++)
	{
		GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->Items->Add(
			String::Format("{0} : {1} - {2}",
			outAnimationEvents[i]->m_nFrame,
			gcnew String(outAnimationEvents[i]->m_strEvent.c_str()),
			gcnew String(outAnimationEvents[i]->m_strParam1.c_str())));

		GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->CheckedEvent(i, vecChecked[i]);
	}
}

void TTabAnimation::InitHitSoundInfo()
{
	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationHitSoundTabPage->AnimationName_label->Text = "";
	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationHitSoundTabPage->HitSound_comboBox->Text = "";

	string strAniName = GetSelectAnimationName();
	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationHitSoundTabPage->AnimationName_label->Text = String::Format("< {0} >", gcnew String(strAniName.c_str()));

	string strHitSound = GetHitSoundName(strAniName);
	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationHitSoundTabPage->HitSound_comboBox->Text = gcnew String(strHitSound.c_str());
}

void TTabAnimation::ModifyHitSoundInfo()
{
	string strAniName = GetSelectAnimationName();
	if(strAniName == "")
		return;

	string strHitSoundModifyText = MStringToCharPointer(GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationHitSoundTabPage->HitSound_comboBox->Text);
	string strHitSound = GetActor()->GetAnimationHitSoundInfo()->Get(strAniName);

	if(strHitSound != strHitSoundModifyText)
	{
		GetActor()->GetAnimationHitSoundInfo()->Set(strAniName, strHitSoundModifyText);
		g_pMainApp->GetActor()->GetAnimationInfoMgr()->SaveXml();
	}
}

std::string TTabAnimation::GetHitSoundName( string& strAniName )
{
	return GetActor()->GetAnimationHitSoundInfo()->Get(strAniName);
}

bool TTabAnimation::CheckUsableAnimationEvent( const RAnimationEvent* pEvent )
{
	if( !GetActor() ) return true;

	RAnimation* pAni = GetActor()->GetCurAnimation();

	if( !pAni || g_pMainApp->m_NormalAnimation.GetUseAnimationLinkListTest()) 
		return true;

	int nCount = 0;
	for(vector<RAnimationEvent*>::iterator itAniEvent = pAni->m_animationEvents.begin(); itAniEvent != pAni->m_animationEvents.end(); itAniEvent++)
	{
		if(pEvent->m_nFrame == (*itAniEvent)->m_nFrame &&
			pEvent->m_strEvent == (*itAniEvent)->m_strEvent &&
			pEvent->m_strParam1 == (*itAniEvent)->m_strParam1)
		{
			// 물인지 체크
			if(pEvent->m_strEvent == "sh_custom")
			{
				string strParam = pEvent->m_strParam1;
				XEffectInfo * pEffectInfo = g_pMainApp->m_pEffectInfoMgr->GetEffectInfo(strParam);
				if(pEffectInfo && pEffectInfo->bWaterPos && g_pMainApp->m_bShowWaterEffect == false)
				{
					return false;
				}
			}

			return GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->GetCheckedEvent(nCount);
		}

		++nCount;
	}

	return true;
}

std::string TTabAnimation::GetSelectAnimationName()
{
	if( !GetActor() ) return "";

	RAnimation* pAni = GetActor()->GetCurAnimation();

	if( !pAni) return "";

	return pAni->GetAliasName();
}

void TTabAnimation::GetAnimationList( System::String^ Aniname, vector<string>& vecOutList )
{
	if( !GetActor() ) return;

	// 애니메이션 파일이 존재하는지.
	if( GetActor() && GetActor()->m_pMesh)
	{
		RMeshAnimationMgr* pAMgr = &GetActor()->m_pMesh->m_AniMgr;

		vector<string> strErrorMessage;

		for(RAnimationHashList_Iter it = pAMgr->m_NodeHashList.begin(); it != pAMgr->m_NodeHashList.end();it++)
		{
			RAnimation* pAnimation = *it;
			if(pAnimation)
			{
				string strAniName = pAnimation->GetAliasName();

				String^ tnstr = gcnew String(strAniName.c_str());
				
				if(tnstr->LastIndexOf(Aniname) >= 0)
					vecOutList.push_back(strAniName);
			}
		}
	}
}

void TTabAnimation::GetFacialAnimationEvent( vector<string>& vecOutList )
{
	if( !GetActor() ) return;

	RAnimation* pAni = GetActor()->GetCurAnimation();

	if( !pAni) return;

	for(int i=0;i<(int)pAni->m_animationEvents.size();i++)
	{
		if("sh_face_ani_start" == pAni->m_animationEvents[i]->m_strEvent)
		{
			vecOutList.push_back(pAni->m_animationEvents[i]->m_strParam1);
		}
	}
}

void TTabAnimation::GetSortAnimationEvent( vector<RAnimationEvent*>& vecAnimationEvent, vector<RAnimationEvent*>& vecOutAnimationEvent )
{
	m_vecCurrentEventList.clear();
	bool bFaceEvent = GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->m_bViewFaceEvent;
	bool bSoundEvent = GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->m_bViewSoundEvent;
	bool bSwordEvent = GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->m_bViewSwordEvent;
	bool bOtherEvent = GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->m_bViewOtherEvent;

	for(vector<RAnimationEvent*>::iterator itView = vecAnimationEvent.begin(); itView != vecAnimationEvent.end(); ++itView)
	{
		if(bFaceEvent) 
		{
			if((*itView)->m_strEvent == "sh_face_ani_start" || (*itView)->m_strEvent == "sh_face_ani_end")
				vecOutAnimationEvent.push_back(*itView);
		}
		else if(bSoundEvent)
		{
			if((*itView)->m_strEvent == "sh_sound" || (*itView)->m_strEvent == "sh_walk")
				vecOutAnimationEvent.push_back(*itView);
		}
		else if(bSwordEvent)
		{
			if((*itView)->m_strEvent == "sh_sword_trail_start" || (*itView)->m_strEvent == "sh_sword_trail_end")
				vecOutAnimationEvent.push_back(*itView);
		}
		else if(bOtherEvent)
		{
			if((*itView)->m_strEvent != "sh_face_ani_start" && (*itView)->m_strEvent != "sh_face_ani_end" &&
				(*itView)->m_strEvent != "sh_sound" && (*itView)->m_strEvent != "sh_walk" &&
				(*itView)->m_strEvent != "sh_sword_trail_start" && (*itView)->m_strEvent != "sh_sword_trail_end")
				vecOutAnimationEvent.push_back(*itView);
		}
		else
		{
			vecOutAnimationEvent.push_back(*itView);
		}
	}

	std::sort(vecOutAnimationEvent.begin(), vecOutAnimationEvent.end(), SortingAnimationEvent);
	m_vecCurrentEventList = vecOutAnimationEvent;
}

void TTabAnimation::ResetAnimationEventList()
{
	if(GlobalObjects::g_pMainForm == nullptr ||
		GlobalObjects::g_pMainForm->m_AnimationTabPage == nullptr ||
		GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage == nullptr)
		return;

	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->Items->Clear();

	if( !GetActor() ) return;

	RAnimation* pAni = GetActor()->GetCurAnimation();

	if( !pAni) return;


	vector<RAnimationEvent*> outAnimationEvents;
	GetSortAnimationEvent(pAni->m_animationEvents, outAnimationEvents);
	for(int i=0;i<(int)outAnimationEvents.size();i++)
	{
		GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->Event_checkedListBox->Items->Add(
			String::Format("{0} : {1} - {2}",
			outAnimationEvents[i]->m_nFrame,
			gcnew String(outAnimationEvents[i]->m_strEvent.c_str()),
			gcnew String(outAnimationEvents[i]->m_strParam1.c_str())));
	}

	GlobalObjects::g_pMainForm->m_AnimationTabPage->m_AnimationEventTabPage->AllChecked(true);
}

int TTabAnimation::GetAnimationEventIndex( RAnimation* pAnimation, int nIndex )
{
	if(m_vecCurrentEventList.size() <= nIndex)
		return -1;

	RAnimationEvent* pEvent = m_vecCurrentEventList[nIndex];
	int nCount = 0;
	for(vector<RAnimationEvent*>::iterator it = pAnimation->m_animationEvents.begin(); it != pAnimation->m_animationEvents.end(); ++it)
	{
		if((*it)->m_nFrame == pEvent->m_nFrame &&
			(*it)->m_strEvent == pEvent->m_strEvent &&
			(*it)->m_strParam1 == pEvent->m_strParam1 &&
			(*it)->m_strParam2 == pEvent->m_strParam2 &&
			(*it)->m_strParam3 == pEvent->m_strParam3)
			return nCount;

		++nCount;
	}

	return -1;
}

void TTabAnimation::Init()
{
	GlobalObjects::g_pMainForm->m_AnimationTabPage->AnimationInfoBtnEnable(false);
}

void TTabAnimation::SetAniTreeViewName(System::String^ strAniTreeViewName)
{
	const char* cstr1 = MStringToCharPointer(strAniTreeViewName);

	m_strAniTreeViewName = cstr1;
	
	MFreeCharPointer(cstr1);
}
