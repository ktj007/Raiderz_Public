#include "StdAfx.h"
#include "M_ToolTerrain.h"
#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"

#include "M_CommandBuffer.h"
#include "M_ToolCmdMgr.h"

#include "C_ControlDoAction.h"
#include "C_XMLConfig.h"
#include "C_ControlCursor.h"

#include "VR_TerrainHeight.h"
#include "VR_TerrainAverage.h"
#include "VR_TerrainFlat.h"
#include "VR_TerrainSplat.h"
#include "VR_TerrainRelax.h"
#include "VR_TerrainGrass.h"
#include "VR_TerrainUserMaterial.h"

#include "RGrassSystem.h"
#include "M_ToolTerrain.h"
#include "RDeviceD3D.h"
#include "RTexture.h"

#include "MDebug.h"
#include "RUtil.h"

//#include "RToolUtil.h"

using namespace rs3;

//DoAction Stuff Namespace
namespace WB_TerrainAction_NS {

//일반적인 높이 맵 변화 함수
float fEditHeight(float fHeight, float fRange, float fScale, float fFactor)
{
	return fHeight + ( fRange * fScale );
}

//평균화 함수
float fEditFlat(float fHeight, float fRange, float fScale, float fFactor)
{
	if ( fRange > 0.0f )
		return fHeight*(1.0f-fScale) + fFactor*fScale;
	else
		return fHeight;
}

//평균값을 위한 변수
float ns_fSum;
int ns_nCount;

void InitAver()
{
	ns_fSum = 0.0f;
	ns_nCount = 0;
}

//평균을 구하는 함수
float fGetAver(float fHeight, float fRange, float fScale, float fFactor)
{
	if ( fRange > 0.0f )
	{
		ns_fSum += fHeight;
		++ns_nCount;
	}

	return fHeight;
}

inline void RenderTerrainActionCursor(
	const rs3::RVector* _pSetVec, const rs3::RVector& _refScaleVec, const DWORD _dwColor)
{
	L3DCursor::GetInstance()->SetType(L3DCursor::CYLINDER, true);
	L3DCursor::GetInstance()->SetTransform(*_pSetVec, _refScaleVec);
	L3DCursor::GetInstance()->SetColor(_dwColor);
	//와우, 칼라가 되는구나.
	//L3DCursor::GetInstance()->SetColor(D3DCOLOR_XRGB(255, 255, 255));
	//*_pGetVec = *L3DCursor::GetInstance()->GetPos();
}

} //namespace 끝

using namespace WB_TerrainAction_NS;

void CControlDoAction::TerrainHeight(CControlMediator* _pController)
{
	_pController->m_mouseInfo.setIgnoreMouseMovedPickingRActor( true );

	//확인 - 지형이 있고, 지형위에 액션을 할 마우스가 있는지
	RToolTerrain* pTerrain = (RToolTerrain*) _pController->GetToolSceneManager()->GetCurrentTerrain();
	if ( pTerrain == NULL)
		return;

	const RVector* pMousePosVec = _pController->GetHelperAction()->GetTerrainContactPos();
	if( pMousePosVec == NULL )
		return;

	float fSize = _pController->GetViewPtr<CRollupTerrainHeight>()->m_fEditSize;
	float fStrength = _pController->GetViewPtr<CRollupTerrainHeight>()->m_fStr;

	//Set Scale value
	float fScale = 1.0f;
	if ( GetAsyncKeyState(VK_LSHIFT) ) fScale = 2.0f;
	if ( GetAsyncKeyState(VK_LCONTROL) ) fScale = -fScale;
	fStrength *= fScale;

	//Apply Cursor
	RenderTerrainActionCursor(pMousePosVec,
		RVector(fSize * pTerrain->GetXScale(), fSize * pTerrain->GetYScale(), pTerrain->GetHSize() * 2.01f),
		D3DCOLOR_XRGB(60, 0, 250));

	//Doing Action
	if ( _pController->m_mouseInfo.mouseState.bLButton == true )
	{
		//Undo Entry Point : DoAction의 시작점이면 편집전의 값을 복사한다.
		CToolCmdMgr* pCommandManager = _pController->GetToolCmdMgr();
		if(pCommandManager->IsMouseCmdEntryL())
		{
			CTerrainHeightCmdBuffer* pCmdBuffer = new CTerrainHeightCmdBuffer(pTerrain);
			pCommandManager->RecordCommand(pCmdBuffer);
			pCommandManager->DoneMouseCmdL();
		}
		pTerrain->ApplyCircleActionWithFuncPtr(pMousePosVec, fSize, fStrength, 0.0f, fEditHeight);
		// 지형 편집시 노말 갱신
		pTerrain->UpdateNormal( _pController->m_mapInfo.m_colFace + 1, _pController->m_mapInfo.m_rowFace + 1, _pController->m_mapInfo.m_width, _pController->m_mapInfo.m_height );
	}
}

void CControlDoAction::TerrainAver(CControlMediator* _pController)
{
	_pController->m_mouseInfo.setIgnoreMouseMovedPickingRActor( true );

	//확인 - 지형이 있고, 지형위에 액션을 할 마우스가 있는지
	RToolTerrain* pTerrain = (RToolTerrain*) _pController->GetToolSceneManager()->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;

	const RVector* pMousePosVec = _pController->GetHelperAction()->GetTerrainContactPos();
	if( pMousePosVec == NULL )
		return;

	float fSize = _pController->GetViewPtr<CRollupTerrainHeight>()->m_fEditSize;
	float fAverRatePer = _pController->GetViewPtr<CRollupTerrainAverage>()->m_fStr;

	//가속기능
	float fScale = 1.0f;
	if ( GetAsyncKeyState(VK_LSHIFT) ) fScale = 2.0f;
	fAverRatePer = fAverRatePer * fScale;

	//Apply Cursor
	RenderTerrainActionCursor(pMousePosVec,
		RVector(fSize * pTerrain->GetXScale(), fSize * pTerrain->GetYScale(), pTerrain->GetHSize() * 2.01f),
		D3DCOLOR_XRGB(60, 0, 250));

	//Doing Action
	if ( _pController->m_mouseInfo.mouseState.bLButton == true )
	{
		//Undo Entry Point : DoAction의 시작점이면 편집전의 값을 복사한다.
		CToolCmdMgr* pCommandManager = _pController->GetToolCmdMgr();
		if(pCommandManager->IsMouseCmdEntryL())
		{
			CTerrainHeightCmdBuffer* pCmdBuffer = new CTerrainHeightCmdBuffer(pTerrain);
			pCommandManager->RecordCommand(pCmdBuffer);
			pCommandManager->DoneMouseCmdL();
		}

		// 평균 값을 얻고 ...
		InitAver();
		pTerrain->ApplyCircleActionWithFuncPtr(pMousePosVec, fSize, 0.0f, 0.0f, fGetAver, false);
		float averHeight = ns_fSum / ns_nCount;

		// 퍼센트를 0.0~1.0 으로 바꿔서
		float f = fAverRatePer / 100.0f;
		if ( f > 1.0f )		f = 1.0f;
		if ( f < 0.0f )		f = 0.0f;

		//fEditFlat 적용
		pTerrain->ApplyCircleActionWithFuncPtr( pMousePosVec, fSize, f, averHeight, fEditFlat);
		pTerrain->UpdateNormal( _pController->m_mapInfo.m_colFace + 1, _pController->m_mapInfo.m_rowFace + 1, _pController->m_mapInfo.m_width, _pController->m_mapInfo.m_height );
	}
}

void CControlDoAction::TerrainFlat(CControlMediator* _pController)
{
	_pController->m_mouseInfo.setIgnoreMouseMovedPickingRActor( true );

	RToolTerrain* pTerrain = (RToolTerrain*) _pController->GetToolSceneManager()->GetCurrentTerrain();
	if ( pTerrain == NULL)
		return;

	const RVector* pMousePosVec = _pController->GetHelperAction()->GetTerrainContactPos();
	if( pMousePosVec == NULL )
		return;

	float fSize = _pController->GetViewPtr<CRollupTerrainHeight>()->m_fEditSize;
	float fAverRatePer = _pController->GetViewPtr<CRollupTerrainFlat>()->m_fStr;

	//가속기능
	float fScale = 1.0f;
	if ( GetAsyncKeyState(VK_LSHIFT) ) fScale = 2.0f;
	fAverRatePer = fAverRatePer * fScale;

	//Apply Cursor
	RenderTerrainActionCursor(
		pMousePosVec,
		RVector(fSize * pTerrain->GetXScale(), fSize * pTerrain->GetYScale(), pTerrain->GetHSize() * 2.01f),
		D3DCOLOR_XRGB(60, 0, 250));

	static float fActionEntryHeight;
	//Doing Action
	if( _pController->m_mouseInfo.mouseState.bLButton )
	{
		//Undo Entry Point : DoAction의 시작점이면 편집전의 값을 복사한다.
		CToolCmdMgr* pCommandManager = _pController->GetToolCmdMgr();
		if(pCommandManager->IsMouseCmdEntryL())
		{
			//처음 편집시작할때 값을 base로 가져간다.
			fActionEntryHeight = pMousePosVec->z;

			CTerrainHeightCmdBuffer* pCmdBuffer = new CTerrainHeightCmdBuffer(pTerrain);
			pCommandManager->RecordCommand(pCmdBuffer);
			pCommandManager->DoneMouseCmdL();
		}

		float f = fAverRatePer / 100.0f;
		if ( f > 1.0f )		f = 1.0f;
		if ( f < 0.0f )		f = 0.0f;

		//apply action
		pTerrain->ApplyCircleActionWithFuncPtr( pMousePosVec, fSize, f, fActionEntryHeight, fEditFlat);
		pTerrain->UpdateNormal( _pController->m_mapInfo.m_colFace + 1, _pController->m_mapInfo.m_rowFace + 1, _pController->m_mapInfo.m_width, _pController->m_mapInfo.m_height );
	}
}

void CControlDoAction::TerrainSplat(CControlMediator* _pController)
{
	_pController->m_mouseInfo.setIgnoreMouseMovedPickingRActor( true );

	RToolTerrain* pTerrain = (RToolTerrain*) _pController->GetToolSceneManager()->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;

	const RVector* pMousePosVec = _pController->GetHelperAction()->GetTerrainContactPos();
	if( pMousePosVec == NULL )
		return;

	CRollupTerrainSplat* pEditSplat = _pController->GetViewPtr<CRollupTerrainSplat>();
	int nLayerIndex = pEditSplat->m_nCurrentLayer;
	float fSize     = pEditSplat->m_fEditSize;
	float fStrength = pEditSplat->m_fStr;

	//Apply Cursor
	RenderTerrainActionCursor(
		pMousePosVec,
		RVector(fSize * pTerrain->GetXScale(), fSize * pTerrain->GetYScale(), pTerrain->GetHSize() * 2.01f),
		D3DCOLOR_XRGB(60, 0, 250));

	if ( _pController->m_mouseInfo.mouseState.bLButton )
	{
		float fScale = 1.0f;
		if ( GetAsyncKeyState(VK_LSHIFT) )
			fScale = 2.0f;

		if ( GetAsyncKeyState(VK_LCONTROL) )
			fScale = -fScale;

		if ( nLayerIndex < 0 )
			return;
		if ( nLayerIndex >= pTerrain->GetLayerNum())
			return;

		// 해당되는 u, v 좌표를 얻는다.
		float u = (pMousePosVec->x / pTerrain->GetWidthSize());
		float v = (pMousePosVec->y / pTerrain->GetHeightSize());

		BYTE *pByte = pTerrain->GetAlphaTexturePtr(nLayerIndex);
		if (pByte == NULL)
		{
			pTerrain->CreateAlphaTexture(nLayerIndex);
			pByte = pTerrain->GetAlphaTexturePtr(nLayerIndex);
			_ASSERT(pByte);
		}

		DWORD dwWidth = pTerrain->GetAlphaTextureWidth(nLayerIndex);
		DWORD dwHeight = pTerrain->GetAlphaTextureHeight(nLayerIndex);

		//Undo Entry Point : DoAction의 시작점이면 편집전의 값을 복사한다.
		CToolCmdMgr* pCommandManager = _pController->GetToolCmdMgr();
		if(pCommandManager->IsMouseCmdEntryL())
		{
			CTerrainSplatCmdBuffer* pCmdBuffer = new CTerrainSplatCmdBuffer(nLayerIndex, dwWidth, dwHeight, pByte);
			pCommandManager->RecordCommand(pCmdBuffer);
			pCommandManager->DoneMouseCmdL();
		}

		int nX = int(u*dwWidth+0.5f);
		int nY = int(v*dwHeight+0.5f);

		float fUOffset = 1.0f/(float)dwWidth;
		float fVOffset = 1.0f/(float)dwHeight;

		float fUnitU = pTerrain->GetXScale()/pTerrain->GetWidthSize();
		float fUnitV = pTerrain->GetYScale()/pTerrain->GetHeightSize();

		RECT rt;

		int sizeppx = (int)((fSize + fSize/2)*fUnitU*dwWidth + 0.5f);
		int sizeppy = (int)((fSize + fSize/2)*fUnitV*dwHeight + 0.5f);

		rt.left = nX - sizeppx;
		rt.right = nX + sizeppx;
		rt.top = nY - sizeppy;
		rt.bottom = nY + sizeppy;

		float lx = (fUnitU*fSize);
		float ly = (fUnitV*fSize);
		float minlen = min(lx,ly);

		if ( rt.left < 0 )	rt.left = 0;
		if ( rt.left  >= (int)dwWidth )		rt.left = dwWidth-1;
		if ( rt.right >= (int)dwWidth )		rt.right = dwWidth-1;

		if ( rt.top < 0 )	rt.top = 0;
		if ( rt.top >= (int)dwHeight )		rt.top = dwHeight-1;
		if ( rt.bottom >= (int)dwHeight )	rt.bottom = dwHeight-1;

		for ( int y = rt.top; y<=rt.bottom; ++y )
		{
			for ( int x = rt.left; x<=rt.right; ++x )
			{
				int nIndex = (dwWidth*y + x)*4;
				BYTE *pPixel = &pByte[nIndex+3];

				float fx = (float)x/(float)(dwWidth) + fUOffset*0.5f;
				float fy = (float)y/(float)(dwHeight) + fUOffset*0.5f;

				float dx = fx - u;
				float dy = fy - v;

				float f = sqrtf(dx*dx+dy*dy) / minlen;
				if ( f < 0.0f )
					f = 0.0f;
				if ( f > 1.0f )		
					f = 1.0f;

				f = cosf(f*MMath::PI/2);
				int val = int(f*fStrength*fScale + 0.5f);
				int n = *pPixel + val;

				if ( n < 0 )
					n = 0;
				if ( n > 255 )
					n = 255;

				*pPixel = n;
			}
		}
		pTerrain->UpdateAlphaTexture( nLayerIndex);

		// AlphaMask map의 정보에 따라 변하는 DescMap을 갱신한다.
		RECT dRt;
		dRt.left = ((float)rt.left/(float)dwWidth)* pTerrain->GetWidthMap();
		dRt.right = ((float)rt.right/(float)dwWidth)*pTerrain->GetWidthMap() + 1;
		dRt.top = ((float)rt.top/(float)dwHeight)*pTerrain->GetHeightMap();
		dRt.bottom = ((float)rt.bottom/(float)dwHeight)*pTerrain->GetHeightMap() + 1;
		pTerrain->UpdateDescriptMapAndMaterial(dRt);
	}
}

void CControlDoAction::TerrainRelax(CControlMediator* _pController)
{
	_pController->m_mouseInfo.setIgnoreMouseMovedPickingRActor( true );

	RToolTerrain* pTerrain = (RToolTerrain*) _pController->GetToolSceneManager()->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;

	const RVector* pMousePosVec = _pController->GetHelperAction()->GetTerrainContactPos();
	if( pMousePosVec == NULL )
		return;

	float fSize = _pController->GetViewPtr<CRollupTerrainRelax>()->m_fEditSize;
	float fstrength = _pController->GetViewPtr<CRollupTerrainRelax>()->m_fStr;

	//Apply Cursor
	float fTerrainXScale = pTerrain->GetXScale();
	float fTerrainYScale = pTerrain->GetYScale();

	RenderTerrainActionCursor(
		pMousePosVec,
		RVector(fSize * fTerrainXScale, fSize * fTerrainYScale, pTerrain->GetHSize() * 2.01f),
		D3DCOLOR_XRGB(60, 0, 250));

	//Action
	if ( _pController->m_mouseInfo.mouseState.bLButton )
	{
		CToolCmdMgr* pCommandManager = _pController->GetToolCmdMgr();
		if(pCommandManager->IsMouseCmdEntryL())
		{
			CTerrainHeightCmdBuffer* pCmdBuffer = new CTerrainHeightCmdBuffer(pTerrain);
			pCommandManager->RecordCommand(pCmdBuffer);
			pCommandManager->DoneMouseCmdL();
		}

		float fScale = 1.0f;
		if ( GetAsyncKeyState(VK_LSHIFT) )
			fScale = 2.0f;
		fstrength = fstrength * fScale;

		RECT rt;
		int nX = (int)(pMousePosVec->x / fTerrainXScale + 0.5f);
		int nY = (int)(pMousePosVec->y / fTerrainYScale + 0.5f);

		int sizepp = int(fSize+0.5f) + int(fSize/2.0f);
		if ( fSize < 2.5f )
			sizepp += 1;

		rt.left = nX - sizepp;
		rt.right = nX + sizepp;
		rt.top = nY - sizepp;
		rt.bottom = nY + sizepp;

		pTerrain->ApplyRelaxAction(fstrength, sizepp, &rt);
	}
}

void CControlDoAction::TerrainUserMaterial(CControlMediator* _pController)
{
#if 0
	RToolTerrain* pTerrain = (RToolTerrain*) _pController->GetToolSceneManager()->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;

	const RVector* pMousePosVec = _pController->GetHelperAction()->GetTerrainContactPos();
	if( pMousePosVec == NULL )
		return;

	CEditTerrainUserMaterial *pVEditTerrainUserMaterial = _pController->T_GetView<CEditTerrainUserMaterial>(VIEW_ID::PANEL_TERRAIN_USERMATERIAL);
	_ASSERT(pVEditTerrainUserMaterial!=NULL);

	float fSize = pVEditTerrainUserMaterial->m_fEditSize;

	// Apply Cursor
	float fTerrainXScale = pTerrain->GetXScale();
	float fTerrainYScale = pTerrain->GetYScale();

	RenderTerrainActionCursor(
		pMousePosVec,
		RVector(fSize * fTerrainXScale, fSize * fTerrainYScale, pTerrain->GetHSize() * 2.01f),
		D3DCOLOR_XRGB(250, 0, 60));

	// Action
	if ( _pController->m_mouseInfo.mouseState.bLButton )
	{
		// TODO : Undo 기능 구현
		pVEditTerrainUserMaterial->UpdateData(TRUE);
		DWORD dwMaterialID = pVEditTerrainUserMaterial->m_dwEditID;

		pTerrain->ApplyUserMaterial(pMousePosVec, fSize, dwMaterialID);
		if ( pVEditTerrainUserMaterial->m_btnView.GetCheck())
			pTerrain->UpdateUserMaterialMap(dwMaterialID);
	}
#endif
}

void CControlDoAction::TerrainGrass(CControlMediator* _pController)
{
	_pController->m_mouseInfo.setIgnoreMouseMovedPickingRActor( true );

	RToolTerrain* pTerrain = (RToolTerrain*) _pController->GetToolSceneManager()->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;

	const RVector* pMousePosVec = _pController->GetHelperAction()->GetTerrainContactPos();
	if( pMousePosVec == NULL )
		return;

	rs3::RGrassSystem* pGrassSystem = pTerrain->GetGrassSystem();
	if(pGrassSystem == NULL) return;

	CRollupTerrainGrass* pTerrainGrassView =  _pController->GetViewPtr<CRollupTerrainGrass>();

	//Set Argument Value
	float fSize = pTerrainGrassView->m_fEditSize;
	float lx = fSize*pTerrain->GetXScale();
	float ly = fSize*pTerrain->GetYScale();
	float fRealSize = min(lx,ly);

	if(!GetAsyncKeyState(VK_CONTROL) ) //Add
	{
		//커서 설정
		RenderTerrainActionCursor(
			pMousePosVec,
			RVector(lx, ly, pTerrain->GetHSize() * 2.01f),
			D3DCOLOR_XRGB(60, 0, 250) );

		//리턴 포인트 1 evented 수신 - ontime에서 콜백을 알기위함
		if(_pController->m_mouseInfo.mouseState.bEvented == false) return;

		//evented 무효화(확인했으므로 다음에 event 받을때까지 event 아님으로 판별)
		_pController->m_mouseInfo.mouseState.bEvented = false;

		//리턴 포인트 2
		if( _pController->m_mouseInfo.mouseState.bDrag == false &&
			_pController->m_mouseInfo.mouseState.bLButton == false)
			return;

		if( _pController->m_mouseInfo.mouseState.bDrag )
		{
			//SENSITIVE 가 커질수록 덜 민감한것
			int SENSITIVE = (int)pTerrainGrassView->m_nSpace;
			static int moved_x = 0;
			static int moved_y = 0;
			int abs_x = abs(_pController->m_mouseInfo.m_movedMouseX );
			int abs_y = abs(_pController->m_mouseInfo.m_movedMouseY );
			moved_x += abs_x;
			moved_y += abs_y;
			if(moved_x < SENSITIVE && moved_y < SENSITIVE)
				return;
			else
			{
				moved_x = 0;
				moved_y = 0;
			}
		}
		else if (_pController->m_mouseInfo.mouseState.bLButton == false)
			return;

		std::vector<SPieceGrassInfo*> grassCategory;
		pTerrainGrassView->GetCurrentPieceGrassVector(grassCategory);
		if(grassCategory.empty()) return;

		int nCategorySize = (int)grassCategory.size();
		for(int i_category = 0; i_category < nCategorySize; ++i_category)
		{
			SPieceGrassInfo* pGrassInfo = grassCategory[i_category];
			if(pGrassInfo == NULL) return;

			rs3::RVector vTerrainZ;

			// 1 fSize당 심기는 량
			int grassNum = pGrassInfo->m_nNumber * fSize * fSize * pTerrainGrassView->m_fStr * 0.2;
			//최소값은 1개
			if(grassNum < 1 ) grassNum = 1;
			
			int pieceNum = pGrassSystem->GetGrassID(pGrassInfo->m_strPieceGrassName.c_str());
			int grassSize = pGrassInfo->m_nSize;
			_ASSERT(pieceNum>=0);
	
			// by rOseria
			// Grass Size를 256px를 기준으로 다시 계산 ( Move to GetGrassVertex Funtion)
			//float fGrassWidth = grassSize * pGrassSystem->GetGrassWidth(pieceNum)/256.f;
			//float fGrassHeight = grassSize * pGrassSystem->GetGrassHeight(pieceNum)/256.f;

			float fX = pMousePosVec->x;
			float fY = pMousePosVec->y;
			float fRate = 1.0f;

			int nFailCnt = 0;
			rs3::RGrassBlade b;
			for ( int i = 0; i < grassNum; ++i)
			{
				if(grassNum > 1)
				{
					//랜덤하게 뿌려찍기
					//b.m_afPos[0] = RandomNumber(fX-fRealSize, fX+fRealSize); //x 좌표
					//b.m_afPos[1] = RandomNumber(fY-fRealSize, fY+fRealSize); //y 좌표
					// 원형 영역으로 랜덤하게 한다. - 090610, OZ
					float fRandCir = RandomNumber( 0.0f, 31.4f);
					float fRandFar = RandomNumber( 0.0f, fRealSize);
					b.m_afPos[0] = fX + sin( fRandCir) * fRandFar; //x 좌표
					b.m_afPos[1] = fY + cos( fRandCir) * fRandFar; //y 좌표
				}
				else
				{
					b.m_afPos[0] = fX;
					b.m_afPos[1] = fY;
				}

				//맵 영역 안에서만 풀심기
				RVector vNorm;
				if(pTerrain->GetPickVertical(vTerrainZ, &vNorm, b.m_afPos[0], b.m_afPos[1]))
				{
					RVector4 vColor;
					//pTerrain->GetPickLightMapColor(vColor, vTerrainZ);
					vColor.x = vColor.y = vColor.z = vColor.a = 1.0f;
					
					
					DWORD dwColor = D3DCOLOR_COLORVALUE(vColor.x, vColor.y, vColor.z, vColor.a);
					b.m_dwBottomColor = dwColor;
					b.m_dwTopColor = dwColor;

					b.m_afPos[2] = vTerrainZ.z; //z좌표
					b.m_fNoise = CSpeedGrassRT::NOISE;

					//사이즈는 80 ~ 120 사이 랜덤
					fRate = RandomNumber(0.8f, 1.2f);

					// 노말 : 노말라이즈는 GetPickVertical내에서 수행 됨.
					b.m_afNorm[0] = vNorm[0];
					b.m_afNorm[1] = vNorm[1];
					b.m_afNorm[2] = vNorm[2];

					//좌우는 30%  랜덤
					if ( ::rand()%10 < 3 ) fRate = -fRate;

					if(grassNum > 1)
					{
						b.m_fSize = grassSize * fRate;
						b.m_fThrow = CSpeedGrassRT::THROW;
						b.m_nWhichTexture = pieceNum;
						if(!pGrassSystem->AddBlade(b))
							nFailCnt++;
					}
					else //한개이면 한개만 랜덤하게 찍어주고 나온다.
					{
						pGrassInfo = grassCategory[RandomNumber(0, nCategorySize-1)];
						pieceNum = pGrassSystem->GetGrassID(pGrassInfo->m_strPieceGrassName.c_str());
						grassSize = pGrassInfo->m_nSize;
						_ASSERT(pieceNum>=0);

						b.m_fSize = grassSize * fRate;
						b.m_fThrow = CSpeedGrassRT::THROW;
						b.m_nWhichTexture = pieceNum;
						if(!pGrassSystem->AddBlade(b))
							nFailCnt++;

						//더티 플래그 체크
						_pController->GetToolCmdMgr()->RecordCommand(NULL);
						return;
					}
				}
				else
				{
					nFailCnt++;
				}
			}

			//더티 플래그 체크
			_pController->GetToolCmdMgr()->RecordCommand(NULL);

			if( nFailCnt)	// 풀 심은 결과에 대한 로그 - 090609, OZ
			{
				if( nFailCnt != grassNum)
					mlog( " 풀 %d개 중 %d개를 심는 데 실패하였습니다. 밀도가 높거나 영역 밖일 수 있습니다. \n", grassNum, nFailCnt);
				else
					mlog( " 풀 %d 개 전부 심지 못합니다. 밀도가 높거나 영역 밖일 수 있습니다. \n", grassNum);
			}
		}
	}
	else //지우기
	{
		RenderTerrainActionCursor(
			pMousePosVec,
			RVector(lx, ly, pTerrain->GetHSize() * 2.01f),
			D3DCOLOR_XRGB(255, 255, 255) );

		//evented 수신 - ontime에서 콜백을 알기위함
		if(_pController->m_mouseInfo.mouseState.bEvented == false) return;

		//evented 무효화(확인했으므로 다음에 event 받을때까지 event 아님으로 판별)
		_pController->m_mouseInfo.mouseState.bEvented = false;

		if( _pController->m_mouseInfo.mouseState.bLButton)
		{
			_pController->GetToolCmdMgr()->RecordCommand(NULL);
			pGrassSystem->DeleteBlade(pMousePosVec->x, pMousePosVec->y, fRealSize);
		}
	}
}
