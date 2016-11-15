#pragma once

class XDamageCaptionDraw
{
private:
	rs3::RTexture*				m_hTexture;

	vector<RLVertex>			m_vecVertexBuffer;

	int							m_nTexWidth;
	int							m_nTexHeight;
	float						m_fNumTexWidthGap;
	float						m_fNumTexHeightGap;

	int							m_nCaptionSize;

	bool						m_bLink;

private:
	void						SetDrawInfo(float fx, float fy, float fWidth, float fHeight, int nIndex, RMatrix& matRender, DWORD& dwColor);
	bool						SetLinkVertex();

	void						GetTextureIndex(int nTextureIndex, int nVertexIndex, float& fu, float& fv);

public:
	XDamageCaptionDraw();
	virtual ~XDamageCaptionDraw();

	bool						Create(wstring& strFileName, int nCaptionSize);
	void						Destroy();
	void						Init();
	void						Render();

	void						SetDamageCaption(RMatrix& matRender, wstring& strText, DWORD dwColor = D3DCOLOR_XRGB(255,255,255));
};