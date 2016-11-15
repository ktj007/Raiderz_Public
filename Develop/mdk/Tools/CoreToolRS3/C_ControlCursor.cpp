#include "stdafx.h"
#include <assert.h>

#include "C_ControlCursor.h"
#include "C_ControlMediatorHelperRender.h"
#include "M_ToolActor.h"
#include "M_ToolSceneNodeInstance.h"

#include "RVType.h"
#include "RMeshUtil.h"
#include "RMesh.h"
#include "RSceneNodeID.h"
#include "RRenderHelper.h"
#include "MFileSystem.h"

RVt_pos_tex_rhw vDragLine[5] =
{
	MVector4(0, 0, 0, 1), 0.0f, 0.0f,
	MVector4(0, 0, 0, 1), 0.0f, 0.0f,
	MVector4(0, 0, 0, 1), 0.0f, 0.0f,
	MVector4(0, 0, 0, 1), 0.0f, 0.0f,
	MVector4(0, 0, 0, 1), 0.0f, 0.0f,
};

RVt_pos_tex_rhw vDragBox[4] =
{
	MVector4(0, 0, 0, 1), 0.0f, 0.0f,
	MVector4(0, 0, 0, 1), 0.0f, 0.0f,
	MVector4(0, 0, 0, 1), 0.0f, 0.0f,
	MVector4(0, 0, 0, 1), 0.0f, 0.0f,
};


using namespace rs3;

L3DCursor::L3DCursor()
{
	for ( int i = 0; i<DX_MESH_NUM; ++i )
		m_pCursorObject[i] = NULL;

	m_pd3dDevice = NULL;
	m_pMesh = NULL;
	m_bVolumeType = false;

	m_pCursorNode = NULL;
	m_dwColor = D3DCOLOR_XRGB(0, 0, 255);
	m_pSceneManager = NULL;
	m_eType = NONE;

	m_pControlMediator = NULL;
}

L3DCursor::~L3DCursor()
{
}

void Init(CControlMediator* pControlMediator);
void Release();


void L3DCursor::Init(CControlMediator* pControlMediator, rs3::RDeviceD3D *pd3dDevice, rs3::RToolSceneManager *pSceneManager)
{
	ASSERT(m_pd3dDevice == NULL);
	ASSERT(m_pSceneManager == NULL);
	ASSERT(m_pControlMediator == NULL);

	m_pd3dDevice = pd3dDevice;
	m_pSceneManager = pSceneManager;
	m_pControlMediator = pControlMediator;

	if ( m_pCursorObject[BOX] == NULL )
		m_pd3dDevice->D3DXCreateBox( 1.0f, 1.0f, 1.0f, m_pCursorObject[BOX]);

	if ( m_pCursorObject[SPHERE] == NULL )
		m_pd3dDevice->D3DXCreateSphere(1.0f, 32, 32, m_pCursorObject[SPHERE]);

	if ( m_pCursorObject[CYLINDER] == NULL )
		m_pd3dDevice->D3DXCreateCylinder(1.0f, 1.0f, 1.0f, 32, 32, m_pCursorObject[CYLINDER]);

	if ( m_pCursorObject[RCONE] == NULL )
		m_pd3dDevice->D3DXCreateCylinder(0.0f, 1.0f, 1.0f, 32, 32, m_pCursorObject[RCONE]);
}

void L3DCursor::Release()
{
	ClearCursorNode();

	for ( int i = 0; i<DX_MESH_NUM; ++i )
		SAFE_RELEASE(m_pCursorObject[i]);

	m_pd3dDevice = NULL;
	m_pMesh = NULL;
	m_bVolumeType = false;

	m_pCursorNode = NULL;
	m_dwColor = D3DCOLOR_XRGB(0, 0, 255);
	m_pSceneManager = NULL;
	m_eType = NONE;

	m_pControlMediator = NULL;
}

void L3DCursor::SetType(TYPE eType, bool bVolume)
{
	m_eType = eType;
	m_bVolumeType = bVolume;

	if ( m_pCursorNode && eType != NODE )
	{
		m_pCursorNode->RemoveFromParent();
		SAFE_DELETE(m_pCursorNode);
	}
}

void L3DCursor::SetTransform(const RVector &pos, const RVector &scale)
{
	D3DXMATRIX matTranslate, matScale;
	D3DXMATRIX matRot;

	D3DXMatrixTranslation(&matTranslate, pos.x, pos.y, pos.z);
	D3DXMatrixScaling(&matScale, scale.x, scale.y, scale.z);
	D3DXMatrixRotationZ(&matRot,D3DXToRadian(90.0f));

	matWorld = matScale * matRot * matTranslate;
	m_vPos = pos;
	m_vScale = scale;

	if ( m_eType == NODE )
	{
		if ( m_pCursorNode )
		{
			m_pCursorNode->SetVisible(true);
			m_pCursorNode->SetPosition(pos);

			RSceneNodeUpdateVisitor visitor;
			m_pCursorNode->Traverse(&visitor);
		}
	}
}

void L3DCursor::ClearCursorNode()
{
	if ( m_pCursorNode )
	{
		m_pCursorNode->RemoveFromParent();
		SAFE_DELETE(m_pCursorNode);
	}
}

void L3DCursor::SetNode(RSceneNode *pSceneNode)
{
	ClearCursorNode();

	if ( pSceneNode )
	{
		m_pCursorNode = pSceneNode;
		m_pCursorNode->SetVisible(false);
		m_pSceneManager->AddSceneNode(m_pCursorNode);
	}
}

void L3DCursor::SetNodeFile(const char *szFilename, const char *szNodename)
{
	if ( szFilename == NULL )
		return;

	ClearCursorNode();

	string sceneXMLPath = m_pSceneManager->GetRootSceneNode().GetXMLFilePath();
	const string* pSceneXMLPath = NULL;
	if( sceneXMLPath.length() )
		pSceneXMLPath = &sceneXMLPath;

	// 씬 로드
	RSceneNode* pNode = m_pSceneManager->CreateSceneNode(szNodename);
	bool bRet = m_pSceneManager->LoadSceneNodeFromFile(pNode, szFilename);
	if ( !bRet )
	{
		delete pNode;
		return;
	}

	// TREE는 Re-Scaleing ( x 33 )
	if ( stricmp(pNode->GetNodeIDString(), RSID_TREE) == 0 )
		pNode->SetScale(33.0f);

	m_pCursorNode = pNode;
	m_pCursorNode->SetVisible(false);
	m_pSceneManager->AddSceneNode(pNode);
}

void L3DCursor::RenderBandMesh(RDeviceD3D* pd3dDevice)
{
	D3DXMATRIX matTranslate, matScale;
	D3DXMATRIX matRot;

	float sx = m_vScale.x;
	float sy = m_vScale.y;
	float sz = m_vScale.z;

	D3DXMatrixTranslation(&matTranslate, m_vPos.x, m_vPos.y, m_vPos.z);
	D3DXMatrixScaling(&matScale, sx, sy, sz);
	D3DXMatrixRotationZ(&matRot,D3DXToRadian(90.0f));

	D3DXMATRIX mat;
	mat = matScale * matRot * matTranslate;
	RMatrix matWorld = RMatrix(mat.m);

	pd3dDevice->SetTransform( RST_WORLD, matWorld);
	pd3dDevice->SetFvF( m_pMesh->GetFVF());

	m_pMesh->DrawSubset(0);

	if ( (sx -= 20.0f) < 0 )	sx = 0.0f;
	if ( (sy -= 20.0f) < 0 )	sy = 0.0f;

	D3DXMatrixScaling(&matScale, sx, sy, sz);
	mat = matScale * matRot * matTranslate;
	matWorld = RMatrix(mat.m);

	pd3dDevice->SetTextureStageState( 0, RTSS_COLOROP, RTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 0, RTSS_COLORARG1, RTA_CURRENT);
	pd3dDevice->SetTransform( RST_WORLD, matWorld);
	pd3dDevice->SetFvF( m_pMesh->GetFVF());

	m_pMesh->DrawSubset(0);
}

void L3DCursor::SetColor(DWORD color)
{
	m_dwColor = color;
}

void L3DCursor::Render(RDeviceD3D *pd3dDevice)
{
	if ( m_eType == NONE )
		return;

	pd3dDevice->ShaderOff();
	pd3dDevice->SetTexture(0, R_NONE);
	pd3dDevice->SetTexture(1, R_NONE);
	pd3dDevice->SetLighting( false );
	pd3dDevice->SetDepthEnable( true, true);
	pd3dDevice->SetAlphaTestEnable(false);
	pd3dDevice->SetStencilEnable( false );

	RMatrix mat(matWorld.m);

	if ( m_eType >= DX_MESH_NUM )
	{
		switch ( m_eType )
		{
		case AXIS:
			{
//				_draw_matrix(pd3dDevice,mat, 10.0f);
				RRenderHelper::RenderAxis( mat, 10.f );
			}
			break;
		case BOX2D:
			{
				float x1 = m_vPos.x;
				float y1 = m_vPos.y;
				float x2 = m_vScale.x;
				float y2 = m_vScale.y;

				pd3dDevice->SetCullMode(RCULL_CCW);
				pd3dDevice->SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
				pd3dDevice->SetFvF ( RVt_pos_tex_rhw::FVF );

				// drag box
				vDragBox[0].vPos.x = x1;
				vDragBox[0].vPos.y = y1;

				vDragBox[1].vPos.x = x2;
				vDragBox[1].vPos.y = y1;

				vDragBox[2].vPos.x = x1;
				vDragBox[2].vPos.y = y2;

				vDragBox[3].vPos.x = x2;
				vDragBox[3].vPos.y = y2;

				pd3dDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
				pd3dDevice->SetTextureStageState(0, RTSS_COLORARG1, RTA_TFACTOR);
				pd3dDevice->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_SELECTARG1);
				pd3dDevice->SetTextureStageState(0, RTSS_ALPHAARG1, RTA_TFACTOR);
				pd3dDevice->SetTextureStageState(1, RTSS_COLOROP, RTOP_DISABLE);
				pd3dDevice->SetTextureStageState(1, RTSS_ALPHAOP, RTOP_DISABLE);

				pd3dDevice->SetTextureFactor( 0x77aaaaaa );
				pd3dDevice->DrawPrimitiveUP( RPT_TRIANGLESTRIP, 2, (void*)vDragBox, sizeof(RVt_pos_tex_rhw));


				// drag line
				vDragLine[0].vPos.x = x1;
				vDragLine[0].vPos.y = y1;

				vDragLine[1].vPos.x = x2;
				vDragLine[1].vPos.y = y1;

				vDragLine[2].vPos.x = x2;
				vDragLine[2].vPos.y = y2;

				vDragLine[3].vPos.x = x1;
				vDragLine[3].vPos.y = y2;

				vDragLine[4].vPos.x = x1;
				vDragLine[4].vPos.y = y1;

				pd3dDevice->SetTextureFactor(0xffffffff);
				pd3dDevice->DrawPrimitiveUP( RPT_LINESTRIP, 4, (void*)vDragLine, sizeof(RVt_pos_tex_rhw));
			}
			break;
		case NODE:
			{
			}
		break;
		}

		return;
	}
	RMatrix matW(matWorld.m);
	pd3dDevice->SetTransform(RST_WORLD, matW);
	m_pMesh = m_pCursorObject[m_eType];

	if ( !m_bVolumeType )
	{
		pd3dDevice->SetTextureStageState( 0, RTSS_COLOROP, RTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 0, RTSS_COLORARG1, RTA_TFACTOR);
		pd3dDevice->SetTextureStageState( 1, RTSS_COLOROP, RTOP_DISABLE);
		pd3dDevice->SetTextureFactor( m_dwColor );

		pd3dDevice->SetFvF( m_pMesh->GetFVF());
		m_pMesh->DrawSubset(0);
	}
	else
	{
		pd3dDevice->SetFvF(m_pMesh->GetFVF());
		pd3dDevice->SetTexture( 0, R_NONE);

		pd3dDevice->SetStencilRef(0x01);
		pd3dDevice->SetStencilMask(0xffffffff);
		pd3dDevice->SetStencilWriteMask(0xffffffff);

		pd3dDevice->SetDepthEnable(true, false);
		pd3dDevice->SetStencilEnable(true);
		pd3dDevice->SetBlending( RBLEND_ZERO, RBLEND_ONE);
		pd3dDevice->SetStencilFunc( RCMP_ALWAYS );

		pd3dDevice->SetStencilOp(RSTENCILOP_KEEP, RSTENCILOP_KEEP, RSTENCILOP_INCR);

		pd3dDevice->SetCullMode(RCULL_CCW);
		RenderBandMesh(pd3dDevice);

		pd3dDevice->SetStencilOp(RSTENCILOP_KEEP, RSTENCILOP_KEEP, RSTENCILOP_DECRSAT );
		pd3dDevice->SetCullMode(RCULL_CW);
		pd3dDevice->SetTransform(RST_WORLD, matW);

		RenderBandMesh(pd3dDevice);

		pd3dDevice->SetBlending( RBLEND_SRCCOLOR, RBLEND_INVSRCCOLOR);

		DWORD dwRef = 0x01;
		pd3dDevice->SetStencilRef( dwRef );
		pd3dDevice->SetStencilFunc( RCMP_EQUAL );

		pd3dDevice->SetStencilOp(RSTENCILOP_KEEP, RSTENCILOP_KEEP, RSTENCILOP_KEEP );

		pd3dDevice->SetTextureStageState( 0, RTSS_COLOROP, RTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 0, RTSS_COLORARG1, RTA_TFACTOR);
		pd3dDevice->SetTextureStageState( 1, RTSS_COLOROP, RTOP_DISABLE);

		pd3dDevice->SetTextureFactor( m_dwColor );

		pd3dDevice->SetCullMode( RCULL_NONE );
		pd3dDevice->SetTransform(RST_WORLD, matW);
		pd3dDevice->SetDepthEnable( false );

		m_pMesh->DrawSubset(0);
		pd3dDevice->SetDepthEnable( true );

		pd3dDevice->SetCullMode(RCULL_CW);

		pd3dDevice->SetDepthEnable( true, true);
		pd3dDevice->SetBlending( RBLEND_NONE, RBLEND_NONE );
		pd3dDevice->SetStencilEnable( false );

		pd3dDevice->SetTextureStageState( 0, RTSS_COLOROP, RTOP_MODULATE);
		pd3dDevice->SetTextureStageState( 0, RTSS_COLORARG1, RTA_CURRENT);
		pd3dDevice->SetTextureStageState( 0, RTSS_COLORARG2, RTA_TEXTURE);
	}
}

RVector L3DCursor::GetNodeHalfRange()
{
	if ( m_pCursorNode )
	{
		// 강제 UpdateAABB
		RVector vPos = m_pCursorNode->GetPosition();
		m_pCursorNode->SetPosition(vPos);
		m_pCursorNode->UpdateBoundingVolume();

		return (m_pCursorNode->GetAABB().vmax - m_pCursorNode->GetAABB().vmin)*0.5f;
	}

	return RVector(0, 0, 0);
}

float L3DCursor::GetNodeHeight()
{
	if ( m_pCursorNode )
	{
		// 강제 UpdateAABB
		RVector vPos = m_pCursorNode->GetPosition();
		m_pCursorNode->SetPosition(vPos);
		m_pCursorNode->UpdateBoundingVolume();

		return (m_pCursorNode->GetAABB().vmax.z - m_pCursorNode->GetAABB().vmin.z);
	}
	return 0;
}
