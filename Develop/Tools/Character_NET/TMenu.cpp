#include "StdAfx.h"
#include "TMenu.h"
#include "RRenderHelper.h"
#include "RFont.h"
#include "RShaderCompositeManager.h"

TMenu::TMenu(void)
{
	Init();
}

TMenu::~TMenu(void)
{
}

void TMenu::Destroy()
{
	if( m_pTextNode )
	{
		m_pTextNode->RemoveFromParent();
		SAFE_DELETE(m_pTextNode);
	}

	if( m_pTextFont)
	{
		//m_pTextFont->Finalize(); //Finalize는 RSceneManager에서 한번 호출됩니다.
		SAFE_DELETE(m_pTextFont);
	}

	if(m_pTextNodeActorNodeInfo)
	{
		m_pTextNodeActorNodeInfo->RemoveFromParent();
		SAFE_DELETE(m_pTextNodeActorNodeInfo);
	}
}

void TMenu::Init()
{
	m_bShowAABB	= false;
	m_bShowInteractionAABB	= false;
	m_bVertexNormal = false;
	m_bShowWire = false;
	m_FillMode	= RFILL_SOLID;
	m_bShowSpecular = true;
	m_nCurShowBoneState = SBS_NONE;

	m_pTextNode = NULL;
	m_pTextFont = NULL;
	m_pTextNodeActorNodeInfo = NULL;
}

void TMenu::InitFont()
{
	m_pTextFont = new RFont;
	m_pTextNode = new RTextSceneNode;

	m_pTextFont->Create(GetDevice(),L"굴림",12);
	//	m_pTextFont->Initialize(GetDevice());
	m_pTextNode->SetFont(m_pTextFont);
	m_pTextNode->SetText("test");
	m_pTextNode->SetPosition(0,0);
	//g_pMainApp->GetSceneMgr()->AddSceneNode(m_pTextNode);

	m_pTextNodeActorNodeInfo = new RTextSceneNode;
	m_pTextNodeActorNodeInfo->SetFont(m_pTextFont);
	m_pTextNodeActorNodeInfo->SetText("");
	m_pTextNodeActorNodeInfo->SetPosition(0,0);
	//g_pMainApp->GetSceneMgr()->AddSceneNode(m_pTextNodeActorNodeInfo);
}

void TMenu::SetDeviceFillMode()
{
	if(	!m_bShowWire)
		g_pMainApp->GetDevice()->SetFillMode(m_FillMode);
}

void TMenu::DrawWire()
{
	if( g_pMainApp->GetActor() )
	{
		if( m_bShowWire)
		{
			for(int i=0;i<g_pMainApp->GetActor()->GetActorNodeCount();i++)
			{
				g_pMainApp->GetActor()->GetActorNodes()[i]->RenderWire();
			}
		}
	}
}

void TMenu::DrawBone()
{
	if(m_nCurShowBoneState == SBS_SIMPLE && g_pMainApp->GetActor())
	{
		g_pMainApp->GetDevice()->SetFillMode(RFILL_SOLID);

		g_pMainApp->m_TabModelInfo.OnBoneRander();

		SetDeviceFillMode();
	}
	else if(m_nCurShowBoneState == SBS_EXACT && g_pMainApp->GetActor())
	{
		g_pMainApp->GetDevice()->SetDepthEnable(false,false);
		g_pMainApp->GetActor()->RenderSkeleton();
	}
}

void TMenu::DrawNormal()
{
	if( m_bVertexNormal && g_pMainApp->GetActor())
	{
		for(int i=0;i<g_pMainApp->GetActor()->GetActorNodeCount();i++)
		{
			g_pMainApp->GetActor()->GetActorNodes()[i]->RenderWire();
			g_pMainApp->GetActor()->GetActorNodes()[i]->RenderNormal();
		}
	}
}

void TMenu::OnShowAABB()
{
	m_bShowAABB = !m_bShowAABB;
	if( m_bShowAABB)
	{
		GlobalObjects::g_pMainForm->DrawBoxtoolStripButton->Checked = true;
		GlobalObjects::g_pMainForm->drawAABBToolStripMenuItem->Checked = true;
	}
	else
	{
		GlobalObjects::g_pMainForm->DrawBoxtoolStripButton->Checked = false;
		GlobalObjects::g_pMainForm->drawAABBToolStripMenuItem->Checked = false;
	}
}

void TMenu::DrawAABBbox()
{
	if(g_pMainApp->GetActor())
		g_pMainApp->GetActor()->UpdateTransform();

	if(m_bShowAABB)
	{
		if(g_pMainApp->GetActor())
		{
			//m_pActor->OnUpdateAABB();
			g_pMainApp->GetActor()->UpdateBoundingVolume();
			RRenderHelper::RenderBox(RMatrix::IDENTITY,g_pMainApp->GetActor()->GetAABB());

			vec3 gab = g_pMainApp->GetActor()->GetAABB().vmax - g_pMainApp->GetActor()->GetAABB().vmin;

			string str;
			//GetFormatString(str, "AABB vMin : %f, %f, %f vMax : %f, %f, %f\n",
			//	m_pActor->GetAABB().vmin.x,m_pActor->GetAABB().vmin.y,m_pActor->GetAABB().vmin.z,
			//	m_pActor->GetAABB().vmax.x,m_pActor->GetAABB().vmax.y,m_pActor->GetAABB().vmax.z);
			//m_pTextNode->SetText(str.c_str());
			GetFormatString(str, "AABB vMin : %f, %f, %f\r",
				gab.x, gab.y, gab.z);
			m_pTextNode->SetText(str.c_str());

			//히트 오브젝트를 만드는 원형(?)
			//RBox b = m_pActor->m_pMesh->GetStaticSkeletonBoundingBox();
			//RRenderHelper::RenderBox(RMatrix::IDENTITY, b, 0xff00ff00);
		}
		//if(m_pWeapon)
		//{
		//	m_pWeapon->OnUpdateAABB();
		//	RRenderHelper::RenderBox(RMatrix::IDENTITY,m_pWeapon->GetAABB());
		//}
	}
	else
	{
		string str;
		GetFormatString(str, "\n");
		m_pTextNode->SetText(str.c_str());
	}
}

void TMenu::DrawPartsText()
{
	if( g_pMainApp->GetActor() == NULL) return;
	if(GlobalObjects::g_pMainForm->tabControl2->SelectedIndex != 5) return;

	string str;

	for(int i = 0; i < g_pMainApp->GetActor()->GetActorNodeCount();i++)
	{
		RActorNode *pActorNode = g_pMainApp->GetActor()->GetActorNodes()[i];
		String^ nodename = gcnew String(pActorNode->m_pMeshNode->m_Name.c_str());

		if( nodename->StartsWith("Bip") || nodename->StartsWith("dummy") )
			continue;

		if( pActorNode->GetParentActorNode() == NULL)
		{
			str += pActorNode->m_pMeshNode->m_Name;
			str += " : ";
			str += GetPureFileName(pActorNode->m_pMeshNode->m_pBaseMesh->GetName());
			str += "\n";
		}
	}

	m_pTextNode->SetText(str.c_str());
	m_pTextNode->OnRender( NULL, 0 );
}

void TMenu::OnShowSpecular()
{
	m_bShowSpecular = !m_bShowSpecular;

	REngine::GetSceneManagerPtr()->GetShaderCompositeManager()->SetSpecularEnable( m_bShowSpecular );
}

void TMenu::OnShowBones()
{
	m_nCurShowBoneState++;
	if((m_nCurShowBoneState % SBS_MAX) == 0)
	{
		m_nCurShowBoneState = 0;
	}
}

bool TMenu::CheckDrawBone( SHOWBONESTATE eState )
{
	if(m_nCurShowBoneState == eState)
		return true;

	return false;
}

void TMenu::SetShowBoneType( int nBoneTypeIndex )
{
	if(nBoneTypeIndex >= SBS_MAX)
		return;

	m_nCurShowBoneState = nBoneTypeIndex;
}

void TMenu::SetFillMode( int nFillmode )
{
	m_FillMode = (RFILLMODE)nFillmode;
}

void TMenu::DrawActorNodeInfo( RActorNode* pActorNode )
{
	if(pActorNode == NULL)
		return;

	string str;
	string strFinal;

	const int nStringLineCount = 4;
	strFinal = GetFormatString(str, "Bone Name : %s\n", pActorNode->m_strName.c_str());
	strFinal += GetFormatString(str, "Visibility : %f\n", pActorNode->GetVisibility());
	strFinal += GetFormatString(str, "World Position : x = %f, y = %f, z = %f\n", pActorNode->GetWorldPosition().x, pActorNode->GetWorldPosition().y, pActorNode->GetWorldPosition().z);
	strFinal += GetFormatString(str, "Scale : x = %f, y = %f, z = %f\n", pActorNode->GetScale().x, pActorNode->GetScale().y, pActorNode->GetScale().z);
	
	m_pTextNodeActorNodeInfo->SetText(strFinal.c_str());

	int y = GlobalObjects::g_pMainForm->MainView->Size.Height;
	m_pTextNodeActorNodeInfo->SetPosition(0, y - (14 * nStringLineCount) - 2);
	m_pTextNodeActorNodeInfo->OnRender( NULL, 0 );
}

void TMenu::OnShowInteractionAABB()
{
	if(g_pMainApp->GetActor() == NULL) return;

	m_bShowInteractionAABB = !m_bShowInteractionAABB;
	if( m_bShowInteractionAABB)
	{
		GlobalObjects::g_pMainForm->DrawInteractionAABBToolStripMenuItem->Checked = true;
	}
	else
	{
		GlobalObjects::g_pMainForm->DrawInteractionAABBToolStripMenuItem->Checked = false;
	}
}

void TMenu::DrawInteractionAABBbox()
{
	if(m_bShowInteractionAABB)
	{
		//g_pMainApp->GetActor()->RefreshInteractionAABB();

		RBoundingBox InteractionAABB = g_pMainApp->GetActor()->GetInteractionAABB();
		RRenderHelper::RenderBox(RMatrix::IDENTITY, InteractionAABB);

		string str;
		string strFinal;

		const int nStringLineCount = 3;
		strFinal = GetFormatString(str, "AABB Max x = %.2f, y = %.2f, z = %.2f\n", InteractionAABB.maxx, InteractionAABB.maxy, InteractionAABB.maxz);
		strFinal += GetFormatString(str, "AABB Min x = %.2f, y = %.2f, z = %.2f\n", InteractionAABB.minx, InteractionAABB.miny, InteractionAABB.minz);
		strFinal += GetFormatString(str, "Height = maxz - minz = %.2f\n", InteractionAABB.maxz - InteractionAABB.minz);

		m_pTextNodeActorNodeInfo->SetText(strFinal.c_str());

		int X = GlobalObjects::g_pMainForm->MainView->Size.Width;
		int y = GlobalObjects::g_pMainForm->MainView->Size.Height;
		m_pTextNodeActorNodeInfo->SetPosition(X*0.5f, y - (14 * nStringLineCount) - 2);
		m_pTextNodeActorNodeInfo->OnRender( NULL, 0 );
	}
}

void TMenu::Render()
{
	DrawAABBbox();
	DrawInteractionAABBbox();
	DrawPartsText();
}