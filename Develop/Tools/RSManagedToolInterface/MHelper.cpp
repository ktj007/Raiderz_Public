#include "stdafx.h"
#include "MHelper.h"
#include "NativeConverter.h"

using namespace RSManaged;

void MHelper::DrawLine(Vector3 vec, int size, unsigned int color)
{
	rs3::RVector v = NativeConverter::getNativeVector(vec);
	RToolHelper::DrawLine(&v, size, color);
}

void MHelper::DrawGrid(Vector3 pos, float w, float h, int cnt_x, int cnt_y, unsigned int color)
{
	rs3::RVector v = NativeConverter::getNativeVector(pos);
	RToolHelper::DrawGrid(v, w, h, cnt_x, cnt_y, color);
}
