#ifndef _RGI_PROCESSOR_H
#define _RGI_PROCESSOR_H



namespace rs3 {

class RTexture;
class RShaderFX;
class RCameraSceneNode;	

class RS_API RGIProcessor
{
protected:
	int m_nWidth;
	int m_nHeight;

	float m_Radius;
	float m_AngleBias;
	float m_Attenuation;
	float m_Contrast;
	float m_BrightBias;

	int	m_QualityMode;

	string	m_strConfigPath;

	RTexture*	m_pTexShadow;	/// 그림자 영역에만 AO를 처리 할 것이다.
	bool		m_bNotShadowOnly;	/// 그림자 영역에만 AO를 처리 하는 것이 레알임?
	bool		m_bSubtract;	/// AO와 라이팅 셈을 어케할꺼임? 뺄셈임? 아님 기본으로 곱셈임.
	bool		m_bAllLight;	/// 모든 라이트에 적용인지? 모든 라이트에 적용이면 라이트 패스 맨 마지막에 처리가 되고 아니면 디렉셔널 라이트에만 처리가 됨.

public:
	RGIProcessor( int nQualityMode);
	virtual ~RGIProcessor(void);

	virtual void Render( RCameraSceneNode* pCamera, int nWidth, int nHeight, bool bRenderDirect) = 0;
	virtual void Init( int nW, int nH );
	virtual void Destroy();
	virtual void Resize( int nW, int nH );
	virtual void Invalidate();
	virtual void Restore();

	bool CreateBuffer();
	void DestroyBuffer();

	virtual void SaveToXML(MXmlElement &element);
	virtual void LoadToXML(MXmlElement &element);

	virtual void LoadConfig( string strPath);
	virtual void SaveConfig( string strPath);

	virtual int GetQuality() { return m_QualityMode;}
	virtual RTexture* GetRTShadow() { return m_pTexShadow;}
	virtual RTexture* GetRTGI() { return NULL;}

	bool IsShadowOnly()	{ return !m_bNotShadowOnly;	}
	bool IsSubtract()	{ return m_bSubtract;	}
	bool IsAllLight()	{ return m_bAllLight;	}
};



}

#endif