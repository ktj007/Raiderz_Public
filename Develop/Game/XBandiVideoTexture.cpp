#include "StdAfx.h"
#include "XBandiVideoTexture.h"

XBandiVideoTexture::XBandiVideoTexture( rs3::RDeviceD3D* pDevice )
: m_pDevice(pDevice)
, m_pTexture(NULL)
{
	m_fmt_d3d = REngine::GetConfig().pixelFormat;
	m_pixel_size = 4;
	m_fmt_bvl = BVLPF_X8R8G8B8;
}

XBandiVideoTexture::~XBandiVideoTexture(void)
{
	Close();
}

HRESULT XBandiVideoTexture::Open( INT vid_width, INT vid_height )
{
	RTexture* pTexture = NULL;

	LPDIRECT3DDEVICE9 d3d9_device = m_pDevice->GetD3DDevice();
	BOOL pow2 = TRUE;

	D3DCAPS9 Caps;
	d3d9_device->GetDeviceCaps(&Caps);
	if(!!(Caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL))
	{
		pow2 = FALSE;
	}

	int tex_width = CalcTexSize(vid_width, pow2);
	int tex_height = CalcTexSize(vid_height, pow2);


	// try to create a dynamic texture first
	pTexture = m_pDevice->CreateTexture(tex_width,
										tex_height,
										m_fmt_d3d,
										RTF_LINEAR, 
										RCT_DYNAMIC);

	if (pTexture == NULL)
	{
		pTexture = m_pDevice->CreateTexture(tex_width,
			tex_height,
			m_fmt_d3d,
			RTF_LINEAR, 
			RCT_SYSTEMMEM);

		if (pTexture == NULL)
		{
			Beep(1000, 1000);
			ASSERT(0);
			return S_FALSE;
		}
	}

	m_pixel_size = 4;//pTexture->GetInfo().GetSize();


	RLOCKED_RECT locked_rect;

	// Clear the texture to black.
	if(m_pDevice->LockRect(pTexture, 0, &locked_rect, NULL, 0))
	{
		// Clear the pixels.
		BYTE* dest = (BYTE*) locked_rect.pBits;
		int bytes = tex_width*m_pixel_size;

		for(int y = 0 ; y < tex_height ; y++)
		{
			memset(dest, 0, bytes);
			dest += locked_rect.Pitch;
		}

		// Unlock the DirectX texture.
		m_pDevice->UnlockRect(pTexture, 0);
	}

	m_pTexture = pTexture;

	m_vid_width = vid_width;
	m_vid_height = vid_height;
	m_tex_width = tex_width;
	m_tex_height = tex_height;

	return S_OK;
}

void XBandiVideoTexture::Close()
{
	//m_pTexture->Release();
	m_pTexture = NULL;
}

BYTE* XBandiVideoTexture::Lock( INT &pitch )
{
	RLOCKED_RECT locked_rect;
	BYTE* buf = NULL;

	if (m_pDevice->LockRect( m_pTexture, 0, &locked_rect, NULL, 0))
	{
		buf = (BYTE *) locked_rect.pBits;
		pitch = locked_rect.Pitch;
	}

	return buf;
}

void XBandiVideoTexture::Unlock()
{
	m_pDevice->UnlockRect(m_pTexture, 0);
}

HRESULT XBandiVideoTexture::Draw( INT x, INT y, INT width, INT height )
{
	return SetVertices((float)x, (float)y, (float)width, (float)height);
}

typedef struct _D3DVERTEX {						// D3D Vertex
	vec3		p;
	float		rhw;
	float		tu, tv;
} D3DVERTEX;
#define D3DFVF_VERTEX (D3DFVF_XYZRHW | D3DFVF_TEX1)

HRESULT XBandiVideoTexture::SetVertices(float scr_x, float scr_y, float scr_width, float scr_height)
{
	_D3DVERTEX	vertices[4];

	// Set vertices
	vertices[0].p	= vec3(scr_x, scr_y, 0);
	vertices[0].tu  = 0.0f;
	vertices[0].tv  = 0.0f;

	vertices[1].p	= vec3(scr_x + scr_width, scr_y, 0);
	vertices[1].tu  = 1.0f * ((float)m_vid_width / m_tex_width);
	vertices[1].tv  = 0.0f;

	vertices[2].p	= vec3(scr_x, scr_y + scr_height, 0);
	vertices[2].tu  = 0.0f;
	vertices[2].tv  = 1.0f * ((float)m_vid_height / m_tex_height);

	vertices[3].p	= vec3(scr_x + scr_width, scr_y + scr_height, 0);
	vertices[3].tu  = 1.0f * ((float)m_vid_width / m_tex_width);
	vertices[3].tv  = 1.0f * ((float)m_vid_height / m_tex_height);

	vertices[0].rhw   = vertices[1].rhw   = vertices[2].rhw   = vertices[3].rhw   = 1.0f;

	m_pDevice->ShaderOff();
	m_pDevice->SetCullMode(RCULL_NONE);
	m_pDevice->SetFvF(D3DFVF_VERTEX);
	m_pDevice->SetLighting(false);
	m_pDevice->SetTransform(RST_WORLD, RMatrix::IDENTITY);

	m_pDevice->SetTexture(0, m_pTexture);
	m_pDevice->DrawPrimitiveUP( RPT_TRIANGLESTRIP, 2, (LPVOID)vertices, sizeof(_D3DVERTEX));

	m_pDevice->SetTexture(0, NULL);

	return true;
}

int XBandiVideoTexture::CalcTexSize( int vid_size, BOOL pow2 )
{
	// Power-of-2 texture dimensions are required.
	D3DCAPS9 caps;
	m_pDevice->GetD3DDevice()->GetDeviceCaps(&caps);
	if(pow2 && caps.TextureCaps & D3DPTEXTURECAPS_POW2)
	{
		int tex_size = 16;
		while (tex_size < vid_size)
			tex_size = tex_size<<1;

		return tex_size;
	}

	return vid_size;
}
