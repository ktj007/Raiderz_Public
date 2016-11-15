#pragma once

#include "../../sdk/bandi_video_library/v2.1.1/source/BandiVideoTexture.h"

class XBandiVideoTexture : public CBandiVideoTexture
{
	rs3::RDeviceD3D*		m_pDevice;
	rs3::RTexture*			m_pTexture;

	RFORMAT					m_fmt_d3d;
	BVL_PIXEL_FORMAT		m_fmt_bvl;

	int	m_vid_width;
	int m_vid_height;
	int	m_tex_width;
	int m_tex_height;
	int m_pixel_size;

protected:
	int		CalcTexSize(int vid_size, BOOL pow2);
	HRESULT SetVertices(float scr_x, float scr_y, float scr_width, float scr_height);

public:
	XBandiVideoTexture(rs3::RDeviceD3D* pDevice);
	virtual ~XBandiVideoTexture(void);

	virtual HRESULT	Open(INT vid_width, INT vid_height);
	virtual void	Close();

	virtual BYTE*	Lock(INT &pitch);
	virtual void	Unlock();

	virtual HRESULT	Draw(INT x, INT y, INT width, INT height);

	virtual void*	GetObject() { return m_pTexture; };
	virtual BVL_PIXEL_FORMAT GetFormat() { return m_fmt_bvl; };
};
