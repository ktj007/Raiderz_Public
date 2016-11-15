#ifndef _DDS_GEN_H__
#define _DDS_GEN_H__

#include <direct.h>
#include <d3dx9.h>

class DDSGen
{
public:
	DDSGen()
	: m_pD3D(NULL)
	, m_pd3dDevice(NULL)
	, m_bGenMip(true)
	, m_bUndds(false)
	, m_nTexLevel(0)
	, m_nRecursionLevel(0)
	{}
	~DDSGen() {}

	bool					InitD3D();
	void					CloseD3D();
	LPDIRECT3DDEVICE9		GetDevice(){ return m_pd3dDevice; }

	void					SetUnddsOption( bool b)		{ m_bUndds = b; }
	void					SetGenMipOption( bool b)	{ m_bGenMip = b;}
	void					SetTexLevelOption( int n)	{ m_nTexLevel = n;}

	void					GenDDS(const char *szPath);


protected:

	bool					CheckAndGen(char *name);

	LPDIRECT3D9				m_pD3D;
	LPDIRECT3DDEVICE9		m_pd3dDevice;
	D3DPRESENT_PARAMETERS	m_d3dpp;

	bool					m_bGenMip;
	bool					m_bUndds;
	int						m_nTexLevel;

	int						m_nRecursionLevel;
};

#endif