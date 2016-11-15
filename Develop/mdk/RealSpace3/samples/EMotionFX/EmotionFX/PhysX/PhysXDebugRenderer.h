/*
 * EMotion FX 3 - Animation System
 * Copyright (c) 2001-2006 Mystic Game Development - http://www.mysticgd.com
 * All Rights Reserved.
 */

#ifndef DEBUGRENDER_H
#define DEBUGRENDER_H

class NxDebugRenderable;


class DebugRenderer
{
	public:
		void renderData(const NxDebugRenderable& data) const;

	private:
		static void renderBuffer(float* pVertList, float* pColorList, int type, int num);
};

#endif
