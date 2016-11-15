#pragma once

#include "MMath.h"
#include "RDevice.h"

namespace rs3 {

// 월드 좌표를 스크린 좌표로 변환해 주는 것. 친절한 DX 함수를 사용하였다.
RS_API void GetScreenFromWorld(MVector2 *pScreen, const MVector3 *position, RDevice *pDevice);
RS_API void GetScreenFromWorld(MVector2 *pScreen, const MVector3 *position, const RViewport *pViewport, const MMatrix *pWorldMatrix, const MMatrix *pViewMatrix, const MMatrix *pProjectionMatrix);
RS_API void GetScreenFromWorld(MVector3 *pScreen, const MVector3 *position, const RViewport *pViewport, const MMatrix *pWorldMatrix, const MMatrix *pViewMatrix, const MMatrix *pProjectionMatrix);

}