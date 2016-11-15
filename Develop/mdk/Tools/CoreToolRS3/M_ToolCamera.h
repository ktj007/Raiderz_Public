/// @file M_ToolCamera.h
#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <RCameraSceneNode.h>

class CControlMediator;
class CControlMediatorHelperAction;

namespace rs3 {
class RToolTerrain;
class CORETOOL_API RToolCamera : public rs3::RCameraSceneNode
{
public:

	RToolCamera();
	virtual ~RToolCamera();

	void SetPosAndTarget(const rs3::RVector& _refCamPos, const rs3::RVector& _refCamTarget, bool _bSetPivot);
	void SetPosAndDir(const rs3::RVector& _refCamPos, const rs3::RVector& _refCamDir);

	void MoveFront(float n, CControlMediator* pHeightControl = NULL);
	void MoveSide(float n, CControlMediator* pHeightControl = NULL);
	void MoveXY(float _x, float _y);

	void Rotate_at_Pivot(const rs3::RVector& _refPivotVector, float _h, float _v);
	void SetPositionToViewCenter(const rs3::RVector& _refVecToGoCenter, int _nZoomFactor);
	void SetPositionToSelectCenter(CControlMediatorHelperAction* _pControlMediatorHelper);

	bool DoCameraCallBackAction(CControlMediator* _pControlMediator);
	bool DoCameraOnTimeActionAndBreak(CControlMediator* _pControlMediator, bool _bIsNavi);

	// fog
	void GetFogInfo(float& _near, float& _far, DWORD& _color) const;

	//for Camera Info and Save Camera
	static const int LOCATION_NUM = 10; ///< 8개는 단축키, 2개는 특수용도
	static const int DEFAULT_CAMERA = 8;
	static const int TMP_CAMERA = 9;
	/** @brief 카메라 위치 관련 정보 */
	struct LOCATION_INFO {
		LOCATION_INFO() : m_bUsed(false) {}
		rs3::RVector	m_vPos;
		rs3::RVector	m_vDir;
		bool			m_bUsed;
	} m_LocationInfo[LOCATION_NUM];
	void RememberLocation(int nLocationIndex);
	void RememberLocation(int nLocationIndex, RVector& _refPos, RVector& _refDir);
	void ResetRememberLocation(int _nResetNum = -1);

	const LOCATION_INFO* GetLocation(int nLocationIndex);
	void SetLocation(const LOCATION_INFO* _pLocationInfo);

	const RVector& GetPivotPoint(){ return m_vCamPivot_; }

	void SetEnableLogToStatusBar(bool bEnable){ m_bLogToStatusBar_ = bEnable; }

	void InitCamera1st3rdPerson();

	MVector2 GetScreenPosition( const MVector3& worldPos );

	/// 플러그인의 코드와의 호환을 맞추기 위해 의미 없는 상태로라도 함수가 존재
	void SetDistanceCullEnable( bool b)	{}
	bool GetDistanceCullEnable()	{ return false;}

protected:
	virtual void OnUpdate();

private:
	RVector m_vCamPivot_;
	bool bCheckChangeCamera1st3rdPerson;
	void SetCameraPivotType3rd_(CControlMediator* _pControlMediator, bool _bCheckRenderable);

	//////////////////////////////////////////////////////////////////////////
	// 1st3rd Camera
	RVector m_vCamCenter;
	RVector m_vPrePick;
	RPlane m_vCenterPlane;
	float m_fCamCenterLen;
	bool m_b3rdCamera;
	bool m_bInitCameraMBUttonMove;
	void SetCamera1st3rdPerson(CControlMediator* _pControlMediator);
	void SetCamera1st3rdRotation(CControlMediator* _pControlMediator);
	void SetCamera1st3rdKeyDownMove(CControlMediator* _pControlMediator, float fMoveFactor);
	void SetCamera1st3rdMButtonMove(CControlMediator* _pControlMediator);
	RVector CalCameraMove(const rs3::RVector& _refPivotVector, float x, float y);
	RVector CalCameraRotation(const rs3::RVector& _refPivotVector, float _h, float _v);
	float CalCameraWheel(const rs3::RVector& _refPivotVector, float _w);

/*------ for hold version -------------*/
private:
	RVector m_vHoldPosition;
	RVector m_vHoldDirection;

	void Rotate_at_Pivot_hold(const rs3::RVector& _refPivotVector, float _h, float _v);
	void Hold() { m_vHoldPosition = GetPosition(); m_vHoldDirection = GetDirection(); }
	const RVector& GetHoldPosition() { return m_vHoldPosition; }
	const RVector& GetHoldDirection() { return m_vHoldDirection; }
/*------ hold version end -------------*/

	bool m_bLogToStatusBar_;
};
}
