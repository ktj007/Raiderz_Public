#ifndef _XTERRAIN_H
#define _XTERRAIN_H

#include "RTerrain.h"


class XTerrain : public RTerrain
{
protected:
	unsigned int	m_nRenderFlag;

	int RenderGrid();
public:
	XTerrain();
	XTerrain(RDevice *pDevice);
	virtual ~XTerrain();
	virtual void OnRender();
	static XTerrain* CreateFromXML(RDevice *pDevice, const char *szFilename);

	// Debug ¿ëµµ
	enum _RENDER_FLAG
	{
		FLAG_GRID		= 0x01,
	};
	void SetRenderFlag(unsigned int nFlag)	{ m_nRenderFlag = nFlag; }
	unsigned int GetRenderFlag()			{ return m_nRenderFlag; }
};






#endif