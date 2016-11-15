#pragma once

#include "d3exutil.h"

#include "RDevice.h"
#include "RSceneManager.h"
#include "RToolApplication.h"
#include "REffect.h"
#include "RActor.h"
#include "RAnimationMgr.h"
#include "RSky.h"
#include "RFont.h"
#include "RTextSceneNode.h"

// CRView 뷰입니다.
using namespace rs3;

class CRView : public CView, public rs3::RToolApplication
{
	DECLARE_DYNCREATE(CRView)

protected:
	CRView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CRView();

	void		CreateEmitter();

public:
	void		InitState(void);
	void		InitEffect();

	void		Destroy();
	void		ClearEffect();

	void		RenderState();
	void		RenderScene();
	void		ModifyParameter();
	void		ModifyParticle(WPARAM wParam, LPARAM lParam);

	void		OnNew();
	void		OnEffectFileOpen();
	void		EffectFileOpen(const char* szName);

	void		OnEffectFileSave();
	void		OnEffectFileSaveAs();

	void		OnEmitterFileOpen();
	void		EmitterFileOpen(const char* filename);
	void		OnEmitterFileSave();
	void		LoadEffectFile(const char* szFileName);
	void		OnMeshEffectFileOpen();
	RActor*		MeshEffectFileOpen(const char* pathname, const char* filename);
	RActor*		MeshFileOpen(const char* FileName);
	void		OnCharFileOpen();
	void		ActorFileOpen(const char* filename);
	void		LoadFile(const char* FileName);
//	void		LoadXmlFile(const char* FileName);
	void		LoadAniFile(const char* FileName);

 	void		SaveEffectXML(const char* szName);
// 	void		SaveXML(RSceneNode *pSceneNode, MXmlNode *pXMLNode);
	bool		SaveEffectFile(const char* szFileName);

	void		DelMeshEffect(LPARAM lParam);

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual void OnInitialUpdate();

protected:
	DECLARE_MESSAGE_MAP()

public:
	RSky*			m_pSky;
	RFont*			m_pFont;
	RTextSceneNode*	m_pTextNode;

	REffect*		m_pEffect;
	REmitter*		m_pEmitter;

	RActor*			m_pEffectMesh;

	RActor			m_pActor;
	RAnimationMgr	m_AniMgr;

	L3DCamera		m_kCamera;
	float			m_fRotStep;
	float			m_fZoomStep;

	DWORD			m_dwBGColor;

	bool			m_bWired;
	bool			m_bAxis;
	bool			m_bGrid;
	bool			m_bEmitter;

	CString			m_strPathname;

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void OnViewStart();
	void OnViewStop();
	void OnViewPause();
	void OnViewBgcolor();
	void OnWireFrame();
	void OnWireFrameUpdateUI(CCmdUI *pCmdUI);
	void OnViewAxis();
	void OnViewAxisUpdateUI(CCmdUI *pCmdUI);
	void OnViewGrid();
	void OnViewGridUpdateUI(CCmdUI *pCmdUI);
	void OnViewEmitter();
	void OnViewEmitterUpdateUI(CCmdUI *pCmdUI);
	void OnViewPath();
	bool ConfigFromXML();
	void	ResetCamera();
public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};



