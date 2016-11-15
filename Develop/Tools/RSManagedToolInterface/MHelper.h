#pragma once

#include "MFramework.h"

using namespace System;

namespace RSManaged
{
	public __gc class MHelper
	{
	public:
		static void DrawLine(Vector3 vec, int size, unsigned int color);
		static void DrawGrid(Vector3 pos, float w, float h, int cnt_x, int cnt_y, unsigned int color);
	};

}