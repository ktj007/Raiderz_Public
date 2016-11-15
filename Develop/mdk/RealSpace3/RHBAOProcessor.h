#pragma once


#include "RGIProcessor.h"
#include "RShaderConstant.h"


namespace rs3 {


class RS_API RHBAOProcessor : public RGIProcessor
{

protected:

	// The effects and rendering techniques
	RShaderFX*		m_Effect;
	RTechnique		m_TechniqueAO;
	RTechnique		m_TechniqueDenoise;
	RTechnique		m_TechniqueCopy;
	RTechnique		m_TechniqueSubtract;

	RTexture*		m_ptexNoise;
	RTexture*		m_pTarget;
	RTexture*		m_pTargetDenoise;
	RTexture*		m_pCurrentTarget;	// target인지 targetDenoise인지 지시 포인터

	RShaderConstant	m_scNumSteps;
	RShaderConstant	m_scNumDirs;
	RShaderConstant	m_scRadius;
	RShaderConstant	m_scRadiusInv;
	RShaderConstant	m_scRadiusSqr;
	RShaderConstant	m_scAngleBias;
	RShaderConstant	m_scTanAngleBias;
	RShaderConstant	m_scInvRadius;
	RShaderConstant	m_scSqrRadius;
	RShaderConstant	m_scAttenuation;
	RShaderConstant	m_scContrast;
	RShaderConstant	m_scBrightBias;
	RShaderConstant	m_scAspectRatio;
	RShaderConstant	m_scInvAspectRatio;
	RShaderConstant	m_scInvFocalLen;
	RShaderConstant	m_scFocalLen;
	RShaderConstant	m_scInvResolution;
	RShaderConstant	m_scResolution;
	RShaderConstant	m_scProjectionInverseMatrix;
	RShaderConstant	m_scFarZ;
	RShaderConstant	m_scFarClipCornerInView;

	float			m_InvFocalLen[2];
	float			m_FocalLen[2];
	float			m_InvResolution[2];
	float			m_Resolution[2];

	bool			m_bDenoise;
	bool			m_bTargetImmediately;

	RTexture*		m_pRndTexture;

	DWORD			m_nWidthHalf;
	DWORD			m_nHeightHalf;
	

public:
	RHBAOProcessor( int n);
	virtual ~RHBAOProcessor(void);

	virtual void Render( RCameraSceneNode* pCamera, int nWidth, int nHeight, bool bRenderDirect) override;
	virtual void Init( int nW, int nH) override;
	virtual void Destroy() override;
	virtual void Resize( int nW, int nH );
	virtual void Invalidate();
	virtual void Restore();

	void CalcHalfResolution();

	void DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV, int bSmallSize);


	float GetRadius()		{ return m_Radius;			}
	float GetAngleBias()	{ return m_AngleBias;		}
	float GetAttenuation()	{ return m_Attenuation;		}
	float GetContrast()		{ return m_Contrast;		}
	float GetBrightBias()	{ return m_BrightBias;		}

	void SetRadius( float f)			{ m_Radius = f;			}
	void SetAngleBias( float f)			{ m_AngleBias = f;		}
	void SetAttenuation( float f)		{ m_Attenuation = f;	}
	void SetContrast( float f)			{ m_Contrast = f;		}
	void SetBrightBias( float f)		{ m_BrightBias = f;		}

	virtual RTexture* GetRTGI() override { return m_pCurrentTarget;}

};




}

