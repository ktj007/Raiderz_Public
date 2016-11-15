#include "stdafx.h"
#include "WS_CutSceneObjectEntityCamera.h"

CCutSceneObjectEntityCamera::CCutSceneObjectEntityCamera(CControlMediator* pControlMediator)
: CCutSceneObjectEntity(pControlMediator)
{
}

CCutSceneObjectEntityCamera::~CCutSceneObjectEntityCamera()
{
}

void CCutSceneObjectEntityCamera::SetTransform(const MMatrix& mat, bool bUsingCollision)
{
	m_pControlMediator_->GetToolCamera()->Set( vec3(mat._41, mat._42, mat._43), vec3(mat._21, mat._22, mat._23), vec3(mat._31, mat._32, mat._33) );
}

rs3::RCameraSceneNode* CCutSceneObjectEntityCamera::GetRelatedCamera()
{
	return m_pControlMediator_->GetToolCamera();
}

void CCutSceneObjectEntityCamera::SetAnimation(const std::string& _rAnimationName)
{

}
