#include "stdafx.h"
#include "XMiniHpBar.h"
#include "RSceneManager.h"
#include "XEngine.h"

#define MINIBAR_WIDTH		35.0f
#define MINIBAR_HEIGHT		2.0f
#define MINIBAR_DOWN_Z		17.0f

XMiniHpBar::XMiniHpBar(MID nID, rs3::RSceneNode* pSceneNode) : XCharacterCaption(nID, pSceneNode)
{
	m_hHpTexture		= R_NONE;
	m_hBarTexture		= R_NONE;

	m_bActiveBar		= false;

	m_matForBillBoard.MakeIdentity();

	m_strText			= L"";
}

XMiniHpBar::~XMiniHpBar()
{
	Destroy();
}

void XMiniHpBar::Destroy()
{
	if(m_hHpTexture != R_NONE)
	{
		global.engine->DeleteTexture(m_hHpTexture);

		m_hHpTexture = R_NONE;
	}

	if(m_hBarTexture != R_NONE)
	{
		global.engine->DeleteTexture(m_hBarTexture);

		m_hBarTexture = R_NONE;
	}
}

void XMiniHpBar::Create()
{
	if (m_hHpTexture == R_NONE)
	{
		m_hHpTexture = global.engine->CreateTexture(FILENAME_MINIBAR_HP);
	}

	if (m_hBarTexture == R_NONE)
	{
		m_hBarTexture = global.engine->CreateTexture(FILENAME_MINIBAR_BAR);
	}

	InitVertexBuffer();
}

bool XMiniHpBar::Update( vec3& vCamPos, RMatrix& viewInverse )
{
	if(m_bActiveBar == false)
		return true;

	if(XCharacterCaption::Update(vCamPos, viewInverse) == false)
		return false;

	// hp바 쓰일 빌보드 행렬...
	// 좀 독특하게 계산을 하는데... 이것을 쓰도록 합니다.
	m_matForBillBoard = viewInverse;
	m_matForBillBoard._41 = m_vPosition.x;
	m_matForBillBoard._42 = m_vPosition.y;
	m_matForBillBoard._43 = m_vPosition.z;

	RMatrix matTest;
	matTest.SetRotationX(MMath::DegreeToRadian(-90.0f));
	m_matForBillBoard = matTest * m_matForBillBoard;

	// 거리로 정렬을 하다가 보니 insert가 안돼서 포지션을 수정하자.
	// map으로 해서 그렇게 나옴
	m_vPosition.z -= MINIBAR_DOWN_Z;
	return true;
}

void XMiniHpBar::InitVertexBuffer()
{
	RLVertex  vInput;

	vInput.p		= vec3(-MINIBAR_WIDTH, 0, MINIBAR_HEIGHT);
	vInput.color	= 0xffffffff;
	vInput.tu		= 0.0f;	
	vInput.tv		= 0.0f;
	m_BarVertices[0] = vInput;
	m_HpVertices[0] = vInput;

	vInput.p		= vec3(MINIBAR_WIDTH, 0, MINIBAR_HEIGHT);
	vInput.tu		= 1.0f;	
	vInput.tv		= 0.0f;
	m_BarVertices[1] = vInput;
	m_HpVertices[1] = vInput;

	vInput.p		= vec3(-MINIBAR_WIDTH, 0, -MINIBAR_HEIGHT);
	vInput.tu		= 0.0f;	
	vInput.tv		= 1.0f;
	m_BarVertices[2] = vInput;
	m_HpVertices[2] = vInput;

	vInput.p		= vec3(MINIBAR_WIDTH, 0, -MINIBAR_HEIGHT);
	vInput.tu		= 1.0f;	
	vInput.tv		= 1.0f;
	m_BarVertices[3] = vInput;
	m_HpVertices[3] = vInput;
}

void XMiniHpBar::Render( RMatrix& matWorld, DWORD dwColor )
{
	if(m_bActiveBar == false)
		return;

	for(int i = 0; i < 4; i++)
	{
		m_BarVertices[i].color = dwColor;
		m_HpVertices[i].color = dwColor;
	}

	RMatrix matLocal;
	matLocal.MakeIdentity();
	matLocal.SetTranslation(vec3(0, 0, -MINIBAR_DOWN_Z));

	global.device->SetDepthEnable(true);

	global.device->SetTransform(RST_WORLD, matLocal * matWorld);
	global.device->SetFvF(RLVertexType);
	global.device->SetCullMode(RCULL_NONE);

	// HP Bar
	global.device->SetTexture(0, m_hBarTexture);
	global.device->DrawPrimitiveUP(RPT_TRIANGLESTRIP, 2, (LPVOID)m_BarVertices, sizeof(RLVertex));

	// HP
	global.device->SetTexture(0, m_hHpTexture);
	global.device->DrawPrimitiveUP(RPT_TRIANGLESTRIP, 2, (LPVOID)m_HpVertices, sizeof(RLVertex));

	global.device->SetTexture(0, NULL);
}

void XMiniHpBar::UpdateHpData( int nHPRatio )
{
	if(m_bActiveBar == false)
		return;

	int nCur = (MINIBAR_WIDTH * 2.0f * nHPRatio) / 100.0f;
	int nNewX = -MINIBAR_WIDTH + nCur;

	m_HpVertices[1].p = vec3(nNewX, 0, MINIBAR_HEIGHT);
	m_HpVertices[3].p = vec3(nNewX, 0, -MINIBAR_HEIGHT);
}

void XMiniHpBar::postRender()
{
	if(m_bActiveBar && XGETCFG_GAME_MINIHPBAR)
		Render(m_matForBillBoard, RenderInfo.m_dwColor);
}