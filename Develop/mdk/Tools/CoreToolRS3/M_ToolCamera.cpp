#include "stdafx.h"
#include "M_ToolCamera.h"

#include "M_ToolTerrain.h"
#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperResource.h"
#include "RDebugPassRenderer.h"

#include "M_ToolCmdMgr.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_StatusBarHandler.h"
#include "C_XMLUserConfig.h"

#include "VR_EtcMapView.h"

#include "RUtil.h"
#include "RSUtil.h"

using namespace rs3;

bool IntersectionPlane(rs3::RVector *pOut, rs3::RVector *pOrg, rs3::RVector *pDir, const RPlane *pPlane)
{
	float dist = pPlane->GetDistanceTo(*pOrg);
	rs3::RVector d = *pDir;
	d.Normalize();

	float n = -d.DotProduct(pPlane->Normal());
	if ( n == 0 )
		return false;

	if ( pOut )
		*pOut = *pOrg + d*(dist*n);

	return true;
}

//RToolCamera
namespace rs3 {

RToolCamera::RToolCamera() : m_vCamPivot_(0,0,0), m_bLogToStatusBar_(true), bCheckChangeCamera1st3rdPerson(false), m_vCamCenter(0,0,0), m_fCamCenterLen(0.0f), m_b3rdCamera(false), m_bInitCameraMBUttonMove(false)
{
	m_viewMatrix.MakeIdentity();
}

RToolCamera::~RToolCamera(){}

void RToolCamera::MoveFront(float n, CControlMediator* pHeightControl /* = NULL */)
{
	RVector newPosition(GetPosition());
	RVector vecDirectionOffset(GetDirection() * n);

	if(pHeightControl )
	{
		
		RVector vecToMove( m_vCamPivot_ + vecDirectionOffset );
		//vecToMove.z += Tool3Const::CONST_WB_NAVI_HEIGHT; // 캐릭터 키 정도만 오를수 있음

		const RVector* pVecToContactVertically = pHeightControl->GetToolSceneManager()->GetVerticalContactPos( vecToMove, NULL );
		if( pVecToContactVertically )
		{
			float fHightOffset = pVecToContactVertically->z + Tool3Const::CONST_WB_NAVI_HEIGHT;

			m_vCamPivot_ = vecToMove;
			m_vCamPivot_.z = fHightOffset;

			newPosition += vecDirectionOffset;
			newPosition.z = fHightOffset;
		}
	}
	else
		newPosition += vecDirectionOffset;

	SetPosition(newPosition);
}

void RToolCamera::MoveSide(float n, CControlMediator* pHeightControl /* = NULL */)
{
	RVector newPosition = GetPosition();

	D3DXVECTOR3 vAxisX(m_viewMatrix._11, m_viewMatrix._21, m_viewMatrix._31);
	D3DXVec3Normalize(&vAxisX,&vAxisX);
	rs3::RVector vR(vAxisX.x * n, vAxisX.y * n, vAxisX.z * n );

	if(pHeightControl )
	{
		RVector vecToMove( m_vCamPivot_ + vR);
		RVector vecToMoveDirZ( vecToMove + RVector( 0, 0, -10000 ));

		RPICKINFO pickInfo(vecToMove, vecToMoveDirZ, RPM_DETAIL);
		if(pHeightControl->GetToolSceneManager()->PickSceneNode(pickInfo))	// RSceneNode:: 이거 왜 써줘야 하나요 ?
		{
			float fHightOffset = pickInfo.outPosition.z + Tool3Const::CONST_WB_NAVI_HEIGHT;
			m_vCamPivot_ = vecToMove;
			m_vCamPivot_.z = fHightOffset;

			newPosition += vR;
			newPosition.z = fHightOffset;
		}
	}
	else
		newPosition += vR;

	SetPosition(newPosition);
}

void RToolCamera::MoveXY(float _x_factor, float _y_factor)
{
	RVector vAxisX( m_viewMatrix._11, m_viewMatrix._21, m_viewMatrix._31);
	RVector vAxisY( m_viewMatrix._12, m_viewMatrix._22, m_viewMatrix._32);
	vAxisX.Normalize();
	vAxisY.Normalize();

	rs3::RVector vR(vAxisX * _x_factor + vAxisY * _y_factor );

//	m_position += vR;
	SetPosition(GetPosition() + vR);
//	m_bNeedUpdateLocalTransform = true;
}

void RToolCamera::SetPosAndDir(const rs3::RVector& _refCamPos, const rs3::RVector& _refCamDir)
{
//	m_position = _refCamPos;
	//SetPosition(_refCamPos);
	//SetDirection(_refCamDir);
	
	Set( _refCamPos, _refCamDir );

//	m_bNeedUpdateLocalTransform = true;
}

void RToolCamera::SetPosAndTarget(const rs3::RVector& _refCamPos, const rs3::RVector& _refCamTarget, bool _bSetPivot)
{
	if(_bSetPivot)
		m_vCamPivot_ = _refCamTarget; //center를 pivot 점으로 설정

	//set position, position과 direction 만으로 카메라 기술, UpVector는 0,0,1로 고정
//	m_position = _refCamPos;
	SetPosition(_refCamPos);

	//dir 은 at - pos /// pos + dir = at
	RVector camDir(_refCamTarget - _refCamPos);
	camDir.Normalize();

	//set direction
	SetDirection(camDir);

//	m_bNeedUpdateLocalTransform = true;
}

void RToolCamera::Rotate_at_Pivot(const rs3::RVector& _refPivotVector, float _h, float _v)
{
	RMatrix matPivot;
	matPivot.SetLocalMatrix(_refPivotVector,RVector(0,-1,0),RVector(0,0,1));
	RMatrix matInvPivot;
	matPivot.GetInverse(&matInvPivot);

	RMatrix matRotationZ;
	matRotationZ.SetRotationMatrix(0,0,-_h);

	
	RVector right = Normalize(CrossProduct(GetDirection(),RVector(0,0,1)));
	matPivot.TransformNormal(right);
	RMatrix matRotationX;
	matRotationX.SetRotationMatrixAxis(right,_v);

	RMatrix matRotation = matRotationX * matRotationZ;

	RMatrix matTM = matInvPivot *matRotation *matPivot;

	RVector test_direction(GetDirection());
	matTM.TransformNormal(test_direction);

	using namespace Tool3Const;
	if(test_direction.z > CONST_MOUSE_RATATE_SAFE_Z || test_direction.z < - CONST_MOUSE_RATATE_SAFE_Z)
		return;

	RVector direction = GetDirection();
	matTM.TransformNormal(direction);
	SetDirection(direction);
//	m_position = m_position * matTM;
	SetPosition( GetPosition() * matTM );

//	m_bNeedUpdateLocalTransform = true;
}


bool RToolCamera::DoCameraCallBackAction(CControlMediator* _pControlMediator)
{
	using namespace Tool3Const;
	CControlMediator::MOUSE_INFO& mouseInfo = _pControlMediator->m_mouseInfo;
	CControlMediator::CAMERA_INFO& cameraInfo = _pControlMediator->m_cameraInfo;

	//navigation mode 일때
	if(_pControlMediator->IsWBNavigation())
	{
		//휠처리
		if ( mouseInfo.m_nWheelDelta != 0 )
			MoveFront(mouseInfo.m_nWheelDelta * cameraInfo.WHEEL_ZOOM_STEP);

		return true;
	}

	// 1st/3rd Person (new) 카메라
	if(CXMLUserConfig::GetReference().Is1st3rdPerson())
	{
		SetCamera1st3rdPerson(_pControlMediator);
		return true;
	}

	bCheckChangeCamera1st3rdPerson = false;

	bool isCamAction = false;
	CToolCmdMgr* pCmdMgr = _pControlMediator->GetToolCmdMgr();
	/*LONG wbWidth, wbHeight;
	_pControlMediator->GetWBSize(wbWidth, wbHeight);*/

	// 줌인 : 마우스 휠만
	if ( mouseInfo.m_nWheelDelta != 0 )
	{
		MoveFront(mouseInfo.m_nWheelDelta * cameraInfo.WHEEL_ZOOM_STEP);
		isCamAction = true;
	}
	//회전
	else if ( GetAsyncKeyState(VK_MENU) && !GetAsyncKeyState(VK_LCONTROL)
		&&( mouseInfo.mouseState.bDrag || mouseInfo.mouseState.bLButton ||
			mouseInfo.mouseState.bMDrag || mouseInfo.mouseState.bMButton ))
	{
		if(pCmdMgr->IsMouseCmdEntryL() || pCmdMgr->IsMouseCmdEntryM())
		{
			SetCameraPivotType3rd_(_pControlMediator, true);
			//by pok, hold test
			//Hold();
			pCmdMgr->DoneMouseCmdL();
			pCmdMgr->DoneMouseCmdM();
		}

		if (GetDirection().IsEqual(-RVector::AXISZ))
		{
			SetDirection( RVector(0.f, 0.2f, -1.f) );
		}
		float h = mouseInfo.m_movedMouseX *	cameraInfo.ROTATE_STEP * CONST_MOUSE_ROTATE;
		float v = - mouseInfo.m_movedMouseY * cameraInfo.ROTATE_STEP * CONST_MOUSE_ROTATE;
		Rotate_at_Pivot(m_vCamPivot_, h, v);
		if (_pControlMediator->GetViewPtr<CRollupEtcMapView>()->m_ctlVeiwCameraPivot.GetCheck())
		{
			rs3::RMatrix mat;
			mat.SetLocalMatrix( m_vCamPivot_ - GetDirection() * (50.f), rs3::RVector3::AXISY, rs3::RVector3::AXISZ);
			rs3::REngine::GetSceneManager().GetPassRenderer<rs3::RDebugPassRenderer>()->AddDebugBox(mat, 50.f, 0.0001f);
		}

		isCamAction = true;
		if( mouseInfo.m_bCaptured == false )
		{
			SetCapture( _pControlMediator->GetWBWindowHandle() );
			mouseInfo.m_bCaptured = true;
		}
	}
	// 패닝 : 마우스 가운데 버튼만
	else if ( (GetAsyncKeyState(VK_SPACE) && (mouseInfo.mouseState.bDrag || mouseInfo.mouseState.bLButton))
		|| mouseInfo.mouseState.bMDrag ) 
	{
		// TODO : work plane
		//const rs3::RVector* pVecPicked = _pControlMediator->GetHelperResource()->GetTerrainContactPos();
		const rs3::RVector* pVecPicked = _pControlMediator->GetHelperAction()->GetCurrentPickingPosition();

		rs3::RVector VecPickedPoint;

		if( pVecPicked )
		{
			VecPickedPoint = (*pVecPicked);
			float minx = _pControlMediator->m_mapInfo.m_orgX;
			float miny = _pControlMediator->m_mapInfo.m_orgY;
			float maxx = minx + _pControlMediator->m_mapInfo.m_width;
			float maxy = miny + _pControlMediator->m_mapInfo.m_height;
			
			if( (VecPickedPoint.x >= maxx ) || (VecPickedPoint.x < minx) || (VecPickedPoint.y >= maxy) || VecPickedPoint.y < miny )
				VecPickedPoint = rs3::RVector( 0.0f, 0.0f, 0.0f );
		}
		// 픽 안됬을 경우 원점을 기준으로함 by eset
		else
		{
			VecPickedPoint = rs3::RVector( 0.0f, 0.0f, 0.0f );
		}

		float pos_length_factor = 1;
		pos_length_factor = (GetPosition() - VecPickedPoint).Length() * cameraInfo.MOVE_STEP * CONST_MOUSE_PANNING;
		if(pos_length_factor < 0.5f )
			pos_length_factor = 0.5f;

		MoveXY( -mouseInfo.m_movedMouseX * pos_length_factor, mouseInfo.m_movedMouseY * pos_length_factor);

		isCamAction = true;
		if( mouseInfo.m_bCaptured == false )
		{
			SetCapture( _pControlMediator->GetWBWindowHandle() );
			mouseInfo.m_bCaptured = true;
		}
	}

	return isCamAction;
}

bool RToolCamera::DoCameraOnTimeActionAndBreak(CControlMediator* _pControlMediator, bool _bIsNavi)
{
	using namespace Tool3Const;

	float fMoveFactor = _pControlMediator->m_cameraInfo.KEY_STEP * _pControlMediator->GetWBTimerTic() * CONST_KEY_MOVE;
	if(GetAsyncKeyState(VK_SHIFT)) fMoveFactor *= 3; //turbo 3

	// 1st/3rd Camera 사용시에는 작동 안돼게...
	if(CXMLUserConfig::GetReference().Is1st3rdPerson())
	{
		SetCamera1st3rdKeyDownMove(_pControlMediator, fMoveFactor);
		return false;
	}

	//카메라 제어용 Controller
	//navi 모드가 아니면 카메라의 높이를 제어할 필요가 없다
	CControlMediator* pControllerForHegihtControl(NULL);
	if( _bIsNavi)
		pControllerForHegihtControl = _pControlMediator;

	if ( _pControlMediator->m_cameraInfo.m_bKeyW) MoveFront(fMoveFactor, pControllerForHegihtControl);
	if ( _pControlMediator->m_cameraInfo.m_bKeyS) MoveFront(-fMoveFactor, pControllerForHegihtControl);
	if ( _pControlMediator->m_cameraInfo.m_bKeyA) MoveSide(-fMoveFactor, pControllerForHegihtControl);
	if ( _pControlMediator->m_cameraInfo.m_bKeyD) MoveSide(fMoveFactor, pControllerForHegihtControl);

	if (_pControlMediator->m_cameraInfo.m_bKeyW && _pControlMediator->m_cameraInfo.m_bKeyS)
	{
		_pControlMediator->m_cameraInfo.m_bKeyW = false;
		_pControlMediator->m_cameraInfo.m_bKeyS = false;
	}

	if (_pControlMediator->m_cameraInfo.m_bKeyA && _pControlMediator->m_cameraInfo.m_bKeyD)
	{
		_pControlMediator->m_cameraInfo.m_bKeyA = false;
		_pControlMediator->m_cameraInfo.m_bKeyD = false;
	}

	if(_bIsNavi)
	{
		int h, v;
		_pControlMediator->GetHelperAction()->GetMovedCursorPosFromCenter(h, v);
		Rotate_at_Pivot(m_vCamPivot_, h * CONST_KEY_MOVE, v * CONST_KEY_MOVE);
		_pControlMediator->GetHelperAction()->SetCursorToScreenCenter();
		return true;
	}

	return false;
}

void RToolCamera::SetCameraPivotType3rd_(CControlMediator* _pControlMediator, bool _bCheckRenderable)
{
	rs3::RVector* pVecSelectCenter = _pControlMediator->GetHelperAction()->GetSelectNodeCenter();
// 	if (_bCheckRenderable)
// 	{
// 		SCENEOBJECTMAP*	pSelectedSceneNodes = _pControlMediator->GetHelperAction()->GetSelectedSceneObjectPtr();
// 		rs3::RVector3* pSelectedRenderableCheck = NULL;
// 		for (SCENEOBJECTMAP::iterator itr = pSelectedSceneNodes->begin(); itr != pSelectedSceneNodes->end(); ++itr)
// 		{
// 			if (itr->first->IsPrimaryCamRenerable())
// 			{
// 				pSelectedRenderableCheck = pVecSelectCenter;
// 				break;
// 			}
// 		}
// 		pVecSelectCenter = pSelectedRenderableCheck;
// 	}

	if( pVecSelectCenter == NULL)
	{
		_pControlMediator->GetHelperAction()->SetActionPlaneAndContactPos();
		if( CXMLUserConfig::GetReference().IsCenterPivot())
		{
			RVector3 vTo = GetPosition() + GetDirection()* m_fFarZ;
			if(!_pControlMediator->GetHelperAction()->GetActionPlane().Intersect(m_vCamPivot_, GetPosition(), vTo) )
			{
				rs3::RToolTerrain *pTerrain = (rs3::RToolTerrain*) _pControlMediator->GetToolSceneManager()->GetCurrentTerrain();
				if(pTerrain != NULL)
					pTerrain->GetPickLine(m_vCamPivot_, NULL, GetPosition(), vTo);
			}
		}
		else
		{
			const rs3::RVector* pContactMouseRayPos = _pControlMediator->GetHelperAction()->GetCurrentPickingPosition();
			if(pContactMouseRayPos != NULL)
			{
				m_vCamPivot_ = *pContactMouseRayPos;
			}
		}
	}
	else
		m_vCamPivot_ = *pVecSelectCenter;
}

//hold 버전
void RToolCamera::Rotate_at_Pivot_hold(const rs3::RVector& _refPivotVector, float _h, float _v)
{
	_ASSERT(!_isnan(_h));
	_ASSERT(!_isnan(_v));
	if(_h==0 || _v==0) return;

	const float fStep = 0.01f;
	_h*=fStep;
	_v*=-fStep;

	RMatrix matPivot;
	matPivot.SetLocalMatrix(_refPivotVector,RVector(0,-1,0),RVector(0,0,1));
	RMatrix matInvPivot;
	matPivot.GetInverse(&matInvPivot);

	RMatrix matRotationZ;
	matRotationZ.SetRotationMatrix(0,0,-_h);

	
	RVector right = Normalize(CrossProduct(GetHoldDirection(),RVector(0,0,1)));
	matPivot.TransformNormal(right);
	RMatrix matRotationX;
	matRotationX.SetRotationMatrixAxis(right,_v);

	RMatrix matRotation = matRotationX * matRotationZ;

	RMatrix matTM = matInvPivot *matRotation *matPivot;

	SetPosition( GetHoldPosition() * matTM );
	RVector direction;
	matTM.TransformNormal(GetHoldDirection(),direction);
	SetDirection(direction);
}

void RToolCamera::SetPositionToViewCenter(const rs3::RVector& _refVecToGoCenter, int _nZoomFactor)
{
//	m_position = _refVecToGoCenter + ( -m_direction ) * _nZoomFactor;
	SetPosition(_refVecToGoCenter + ( -GetDirection() ) * _nZoomFactor);
}

void RToolCamera::SetPositionToSelectCenter(CControlMediatorHelperAction* _pControlMediatorHelper)
{
	//rs3::RVector* pVecPosCenter = _pControlMediatorHelper->GetSelectSceneNodeCenter();
	//if (pVecPosCenter == NULL)
	//	return;

	//rs3::RVector vPos;
	//SCENEOBJECTMAP*	pSceneNodeMap = _pControlMediatorHelper->GetSelectedSceneObjectPtr();
	//if (pSceneNodeMap->size() == 1)
	//{
	//	vPos = pSceneNodeMap->begin()->first->GetWorldPosition();
	//}
	//else
	//{
	//	vPos = *pVecPosCenter;
	//}

	//SetPositionToViewCenter( vPos, Tool3Const::CONST_WB_STATIC_OBJ_TO_CENTER );

	rs3::RVector* pVecPosCenter = _pControlMediatorHelper->GetSelectNodeCenter();
	if (pVecPosCenter == NULL)
		return;

	SetPositionToViewCenter( *pVecPosCenter, Tool3Const::CONST_WB_STATIC_OBJ_TO_CENTER );
}

void RToolCamera::SetLocation(const LOCATION_INFO* _pLocationInfo)
{
	//default는 초기위치
	if(_pLocationInfo == NULL) return;
	SetPosAndDir( _pLocationInfo->m_vPos, _pLocationInfo->m_vDir );
}

void RToolCamera::OnUpdate()
{
	RCameraSceneNode::OnUpdate();

	if (m_bLogToStatusBar_)
	{
		RVector pos = GetPosition();
		RVector dir = GetDirection();
		CStatusBarHandler::GetInstance()->ShowCurrentCameraPositionAndDirection( pos.x, pos.y, pos.z, dir.x, dir.y, dir.z );
	}

	if(CXMLUserConfig::GetReference().Is1st3rdPerson() &&
		m_fCamCenterLen > 0)
	{
		//////////////////////////////////////////////////////////////////////////
		// 피봇 그리기
		rs3::RMatrix mat;
		mat.SetLocalMatrix( m_vCamCenter, rs3::RVector3::AXISY, rs3::RVector3::AXISZ);
		rs3::REngine::GetSceneManager().GetPassRenderer<rs3::RDebugPassRenderer>()->AddDebugBox(mat, 10.f, 0.0001f);
	}
}

void RToolCamera::RememberLocation(int nLocationIndex)
{
	if ((nLocationIndex < 0) || (nLocationIndex >= LOCATION_NUM)) return;

	m_LocationInfo[nLocationIndex].m_vPos = GetPosition();
	m_LocationInfo[nLocationIndex].m_vDir = GetDirection();
	m_LocationInfo[nLocationIndex].m_bUsed = true;
}

void RToolCamera::RememberLocation(int nLocationIndex, RVector& _refPos, RVector& _refDir)
{
	if ((nLocationIndex < 0) || (nLocationIndex >= LOCATION_NUM)) return;

	m_LocationInfo[nLocationIndex].m_vPos = _refPos;
	m_LocationInfo[nLocationIndex].m_vDir = _refDir;
	m_LocationInfo[nLocationIndex].m_bUsed = true;
}

void RToolCamera::ResetRememberLocation(int _nResetNum)
{
	if( _nResetNum == -1 )
	{
		for(int i = 0; i < DEFAULT_CAMERA; ++i)
			m_LocationInfo[i].m_bUsed = false;
	}
	else if(_nResetNum >= 0 && _nResetNum < DEFAULT_CAMERA)
	{
		m_LocationInfo[_nResetNum].m_bUsed = false;
	}
}

const RToolCamera::LOCATION_INFO* RToolCamera::GetLocation(int nLocationIndex)
{
	if ((nLocationIndex < 0) || (nLocationIndex >= LOCATION_NUM)) return NULL;
	if( m_LocationInfo[nLocationIndex].m_bUsed == false ) return NULL;
	return &m_LocationInfo[nLocationIndex];
}

void RToolCamera::GetFogInfo(float& _near, float& _far, DWORD& _color) const
{
	_near = m_fFogNearZ;
	_far = m_fFogFarZ;
	_color =  m_dwFogColor;
}

void RToolCamera::SetCamera1st3rdPerson( CControlMediator* _pControlMediator )
{
	if(bCheckChangeCamera1st3rdPerson == false)
	{
		InitCamera1st3rdPerson();
	}

	bCheckChangeCamera1st3rdPerson = true;

	// 이동
	SetCamera1st3rdMButtonMove(_pControlMediator);

	// 회전, 거리
	SetCamera1st3rdRotation(_pControlMediator);

	m_fCamCenterLen = (m_vCamCenter - GetPosition()).Length();
}

void RToolCamera::InitCamera1st3rdPerson()
{
	m_vCamCenter = GetPosition() + GetDirection() * 300.0f;
	m_fCamCenterLen = (m_vCamCenter - GetPosition()).Length();
}

void RToolCamera::SetCamera1st3rdRotation( CControlMediator* _pControlMediator )
{
	using namespace Tool3Const;
	CControlMediator::MOUSE_INFO& mouseInfo = _pControlMediator->m_mouseInfo;
	CControlMediator::CAMERA_INFO& cameraInfo = _pControlMediator->m_cameraInfo;

	float h = - mouseInfo.m_movedMouseX * cameraInfo.ROTATE_STEP * CONST_MOUSE_ROTATE;
	float v = - mouseInfo.m_movedMouseY * cameraInfo.ROTATE_STEP * CONST_MOUSE_ROTATE;
	float w = - mouseInfo.m_nWheelDelta * cameraInfo.WHEEL_ZOOM_STEP;

	RVector vNewDst = -GetDirection();

	// 회전
	if ( GetAsyncKeyState(VK_MENU) && !GetAsyncKeyState(VK_LCONTROL)
		&&( mouseInfo.mouseState.bDrag || mouseInfo.mouseState.bLButton))
	{
		vNewDst = CalCameraRotation(m_vCamCenter, h, v);
	}


	// 휠(거리)
	float fNewLen = CalCameraWheel(m_vCamCenter, w);


	// 최종
	RVector vNewPos = m_vCamCenter + (vNewDst * fNewLen);
	RVector vNewDir = -vNewDst;

	SetPosition(vNewPos);
	SetDirection(vNewDir);
}

void RToolCamera::SetCamera1st3rdKeyDownMove( CControlMediator* _pControlMediator, float fMoveFactor )
{
	float fx = 0;
	float fy = 0;
	if ( _pControlMediator->m_cameraInfo.m_bKeyW) fy = fMoveFactor;
	if ( _pControlMediator->m_cameraInfo.m_bKeyS) fy = -fMoveFactor;
	if ( _pControlMediator->m_cameraInfo.m_bKeyA) fx = -fMoveFactor;
	if ( _pControlMediator->m_cameraInfo.m_bKeyD) fx = fMoveFactor;

	m_vCamCenter = CalCameraMove(m_vCamCenter, fx, fy);

	RVector vNewPos = m_vCamCenter + (GetDirection() * -m_fCamCenterLen);
	RVector vNewDir = GetDirection();

	SetPosition(vNewPos);
	SetDirection(vNewDir);

	m_fCamCenterLen = (m_vCamCenter - GetPosition()).Length();
}

void RToolCamera::SetCamera1st3rdMButtonMove( CControlMediator* _pControlMediator )
{
	using namespace Tool3Const;
	CControlMediator::MOUSE_INFO& mouseInfo = _pControlMediator->m_mouseInfo;
	CControlMediator::CAMERA_INFO& cameraInfo = _pControlMediator->m_cameraInfo;
	
	if(mouseInfo.mouseState.bMDrag || mouseInfo.mouseState.bMButton)
	{
		if(m_bInitCameraMBUttonMove == false)
		{
			const rs3::RVector* pVecPicked = _pControlMediator->GetHelperAction()->GetCurrentPickingPosition();
			if(pVecPicked)
				m_vPrePick = *pVecPicked;
			else
				m_vPrePick = RVector(0, 0, 0);
			m_vCenterPlane = MPlane(-GetDirection(), m_vPrePick);
			m_bInitCameraMBUttonMove = true;
		}

		RVector vecMouseLineOrg; 
		RVector vecMouseLineTo;
		RVector vecMouseLineOrg2; 
		RVector vecMouseLineTo2;
		_pControlMediator->GetHelperAction()->GetMouseLine(mouseInfo.m_currentMouseX, mouseInfo.m_currentMouseY, vecMouseLineOrg, vecMouseLineTo);
		_pControlMediator->GetHelperAction()->GetMouseLine(mouseInfo.m_currentMouseX - mouseInfo.m_movedMouseX, mouseInfo.m_currentMouseY - mouseInfo.m_movedMouseY, vecMouseLineOrg2, vecMouseLineTo2);

		RVector vPlanePick;
		if(m_vCenterPlane.Intersect(vPlanePick, vecMouseLineOrg, vecMouseLineTo) &&
			m_vCenterPlane.Intersect(m_vPrePick, vecMouseLineOrg2, vecMouseLineTo2))
		{
			m_vCamCenter += m_vPrePick - vPlanePick;
		}
		else
		{
			float h = - mouseInfo.m_movedMouseX * 10;
			float v = mouseInfo.m_movedMouseY * 10;

			m_vCamCenter += GetRight() * h;
			m_vCamCenter += GetUp() * v;
		}

		RVector vNewPos = m_vCamCenter + (GetDirection() * -m_fCamCenterLen);
		RVector vNewDir = GetDirection();

		SetPosition(vNewPos);
		SetDirection(vNewDir);
	}
	else
	{
		m_bInitCameraMBUttonMove = false;
	}
}

RVector RToolCamera::CalCameraRotation( const rs3::RVector& _refPivotVector, float _h, float _v )
{
	RVector vCross = GetRight();

	RMatrix matRotX;
	matRotX.SetRotationMatrixAxis(vCross, _v);

	RMatrix matRotY;
	matRotY.SetRotationMatrixAxis(RVector(0,0,1), _h);

	RMatrix matRot = matRotX * matRotY;

	RVector vNewDst;
	matRot.TransformVect(-GetDirection(), vNewDst);

	using namespace Tool3Const;
	if(vNewDst.z > CONST_MOUSE_RATATE_SAFE_Z || vNewDst.z < - CONST_MOUSE_RATATE_SAFE_Z)
		return -GetDirection();

	return vNewDst;
}

float RToolCamera::CalCameraWheel( const rs3::RVector& _refPivotVector, float _w )
{
	float fCurrLen = m_fCamCenterLen;

	fCurrLen += _w;
	
	if(fCurrLen < 0.0f)
		fCurrLen = 0;

	return fCurrLen;
}

rs3::RVector RToolCamera::CalCameraMove( const rs3::RVector& _refPivotVector, float x, float y )
{
	RVector vNewDst = _refPivotVector;

	RVector vDir = GetDirection();
	RVector vCross = GetRight();

	vNewDst += (vDir * y) + (vCross * x);

	return vNewDst;
}


MVector2 RToolCamera::GetScreenPosition( const MVector3& worldPos )
{
	MVector2 vecScreenPos;
	GetScreenFromWorld( &vecScreenPos, &worldPos, 
		&REngine::GetDevice().GetViewport(),
		&RMatrix::IDENTITY, 
		&GetViewMatrix(),
		&GetProjectionMatrix());
	return vecScreenPos;
}

}