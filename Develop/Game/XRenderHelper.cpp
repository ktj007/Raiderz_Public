#include "stdafx.h"
#include "XRenderHelper.h"
#include "RCameraSceneNode.h"
#include "RSUtil.h"

void XRenderHelper::DrawText(vec3& vPos, MCOLOR color, const wchar_t* text)
{
	if (REngine::GetDevicePtr() == NULL) return;
	if ((global.smgr == NULL) || (global.ui==NULL)) return;

	RCameraSceneNode* pCamera = global.smgr->GetPrimaryCamera();
	if (pCamera == NULL) return;

	RViewFrustum view_frustum = pCamera->GetViewFrustum();
	RSphere sphere(vPos, 1.0f);
	if (!view_frustum.TestSphere(sphere)) return;

	vec2 screen_pos;

	GetScreenFromWorld(&screen_pos,&vPos,&REngine::GetDevicePtr()->GetViewport(), &RMatrix::IDENTITY, &pCamera->GetViewMatrix(), &pCamera->GetProjectionMatrix());

	global.ui->GetDC()->SetColor(color);

	global.ui->GetDC()->BeginDraw();
	{
		global.ui->GetDC()->Text(screen_pos.x, screen_pos.y, MLocale::ConvUTF16ToAnsi(text).c_str());
	}
	global.ui->GetDC()->EndDraw();
	
}
