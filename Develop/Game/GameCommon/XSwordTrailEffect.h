#pragma once
#ifndef _XSWORDTRAIL_EFFECT_H
#define _XSWORDTRAIL_EFFECT_H

#include "RActor.h"
#include "XCharacter.h"
#include "XSwordTrailSamplingData.h"

using namespace rs3;

class XSwordTrailEffect
{
private:
	vector<RLVertex>		m_vecVertexBuffer;
	RTexture*				m_hTexture;

	vector<vec3>			m_vecSwordTrailStartSampling;
	vector<vec3>			m_vecSwordTrailEndSampling;

	int						m_nVertexCount;

private:
	bool					CalSwordTrailPosition(XCharacter* pCharacter, SWORDTRAILSAMPLING_DATA* pSamplingData, vector<vec3>& vecStart, vector<vec3>& vecEnd);

public:
	XSwordTrailEffect();
	virtual ~XSwordTrailEffect();

	bool					CreateSwordTrailEffect(XCharacter* pCharacter, SWORDTRAILSAMPLING_DATA* pSamplingData, tstring strSwordTrailTexture);
	void					RenderSwordTrailEffect(DWORD dwFadeValue);
	void					DestroySwordTrailEffect();

	void					CalSwordTrailEffectVertexCount(int nCurrentFrame, int nStartFrame, int nEndFrame, int nFrameGap, int& nOutVertexCount);

	void					UpdateVertex(int nVectexCount, MMatrix& matWorldObject);

	int						GetSamplingCount() { return m_vecSwordTrailStartSampling.size(); }
	vec3					GetVertexSwordTrailSampling(int nIndex);
};


#endif