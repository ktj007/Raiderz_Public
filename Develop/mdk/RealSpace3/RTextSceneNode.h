#pragma once

#include <tchar.h>
//#include <map>
//#include <list>
//#include "MemPool.h"
//#include "RSceneManager.h"
#include "RSceneNode.h"
#include "RTypes.h"
//#include "RFont.h"

using namespace std;

namespace rs3 {

class RS_API RTextSceneNode : public RSceneNode
{
//	DECLARE_USING_PASS(RRP_2D);

	int		m_nX,m_nY;		// position
	DWORD	m_dwColor;
	int		m_nBufferSize;
	TCHAR	*m_szText;
	RFont	*m_pFont;
	float	m_fScale;

public:
	RTextSceneNode();
	virtual ~RTextSceneNode(void);

	void ClearText();

	void SetFont(RFont *);
	void SetBufferSize(int nSize);
	void SetText(const char *szText);
	void SetPosition(int x,int y);
	void SetColor(DWORD dwColor);

	virtual void OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod = RENDERING_NULL) override;
};

}
