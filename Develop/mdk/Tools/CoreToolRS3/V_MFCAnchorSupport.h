#pragma once

const unsigned char ANCHOR_LEFT		= 1;
const unsigned char ANCHOR_TOP		= 1<<1;
const unsigned char ANCHOR_RIGHT	= 1<<2;
const unsigned char ANCHOR_BOTTOM	= 1<<3;

typedef unsigned char CAnchorStyle;

// mfc anchor Áö¿ø
class CORETOOL_API CMFCAnchorSupport
{
public:
	CMFCAnchorSupport();

	void	Initialize(CWnd* pParent, int cx, int cy);
	void	RegisterAnchor( int nResourceID, CAnchorStyle anchorStyle );
	void	OnSize( int cx, int cy );

protected:
	struct AnchorData
	{
		CAnchorStyle	style;
		RECT			initialRect;
	};

	std::map< int, AnchorData >	m_mapAnchorWindows;
	CWnd*				m_pParent;
	int					m_nInitialCX;
	int					m_nInitialCY;
};
