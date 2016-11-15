#pragma once
#include "MSingleton.h"
#include "MMemPool.h"

namespace rs3
{
	class RFont;
	class RSceneNode;
};

class XCaption
{
	friend class XCaptionMgr;	// 생성 / 소멸은 매니저를 통해
private:
	MID		m_nID;
protected:
	rs3::RSceneNode*	m_pSceneNode;
	std::wstring			m_strText;
	DWORD				m_dwColor;
	rs3::RMatrix		m_matTextReflectAndScale;
	bool				m_bVisible;
	bool				m_bPassRender;							// 특정 상황에서 그리지 않을 경우...
	vec3				m_vPosition;
	bool				m_bSelfDelete;

	void SetupRenderInfo(RMatrix& viewInverse, DWORD dwColor)
	{
		// billboard
		rs3::RMatrix matT( viewInverse );
		matT._41 = m_vPosition.x;
		matT._42 = m_vPosition.y;
		matT._43 = m_vPosition.z;

		RenderInfo.m_matForRender = m_matTextReflectAndScale * matT;
		RenderInfo.m_dwColor = dwColor;
	}

	// false를 리턴하면 Mgr이 자동으로 지우고 삭제해 버린다.
	virtual bool Update(vec3& vCamPos, RMatrix& viewInverse) { return true; }
	virtual void preRender()	{}
	virtual void postRender()	{}
public:
	XCaption(MID nID, rs3::RSceneNode* pSceneNode);
	virtual ~XCaption() {}

	MID GetID() { return m_nID; }
	rs3::RSceneNode*	GetSceneNode() { return m_pSceneNode; }

	//////////////////////////////////////////////////////////////////////////
	// caption text
	//
	void				SetText(const wchar_t* _pSzText)			{ m_strText = _pSzText; }
	void				SetText(const std::wstring& _refText)	{ m_strText = _refText; }

	void				AppendText(const wchar_t* _pSzText)		{ m_strText += _pSzText; }
	void				AppendText(const std::wstring& _refText)	{ m_strText += _refText; }

	const std::wstring&	GetText() const { return m_strText; }
	bool				IsTextEmpty()		{ return m_strText.empty(); }

	//////////////////////////////////////////////////////////////////////////
	// color
	//
	void				SetColor(DWORD _dwColor)	{ m_dwColor = _dwColor; }
	DWORD				GetColor()					{ return m_dwColor; }

	//////////////////////////////////////////////////////////////////////////
	// scale
	//
	void				SetScale(float _fScale);
	float				GetScale(){ return m_matTextReflectAndScale._11; }

	//////////////////////////////////////////////////////////////////////////
	// visible
	//
	void				SetVisible(bool _b)	{ m_bVisible = _b; }
	bool				GetVisible()		{ return m_bVisible; }

	//////////////////////////////////////////////////////////////////////////
	// pass render
	//
	bool				IsPassRender()		{ return m_bPassRender;		}
	virtual	bool		IsFontRender()		{ return true; }

	virtual bool		CheckRenderByCamera(RCameraSceneNode* pCam)	{ return false; }

	//////////////////////////////////////////////////////////////////////////
	// pos
	//
	const vec3&			GetPosition() { return m_vPosition; }

	struct CAP_RENDER_INFO
	{
		DWORD				m_dwColor;
		RMatrix				m_matForRender;
	};
	CAP_RENDER_INFO RenderInfo;
};

// XCharacterCaption은 이름뿐만 아니라 HP게이지, 등급, 기타 정보를 이름과 같은 위치에 보여주기 위해서 쓰인다. 
// 아니라면 나중에 리펙토링 합니다.(투명처리와 3D 처럼 보여주기 위해서 같이 쓰입니다.)
// MiniHpBar 때문에 public MMemPool<XCharacterCaption> 이부분을 삭제합니다. 나중에 넣을 겁니다.
class XCharacterCaption : public XCaption
{
	friend class XCaptionMgr;	// 생성 / 소멸은 매니저를 통해
protected:
	virtual bool Update(vec3& vCamPos, RMatrix& viewInverse);
	virtual void postRender()	{}
public:
	XCharacterCaption(MID nID, rs3::RSceneNode* pSceneNode);
	virtual ~XCharacterCaption();

	void SetBoneNode( const rs3::RSceneNode* _pActorNdoe );
	void SetOffSet(float _fOffset)	{ m_fOffSet = _fOffset; }
	float GetOffSet() { return m_fOffSet; }

private:
	rs3::RSceneNode*	m_pActorNode;	// 더미, 본 이런것을 붙이는 겁니다.
	float				m_fOffSet;

	float				m_fBackupActorNodeHeight;
};


class XCombatCaption : public XCaption
{
protected:
	float	m_fElapsedTime;
	float	m_fLastTime;
	float	m_fStartTime;

	virtual bool OnUpdate(float fDelta, RMatrix& viewInverse) = 0;
	virtual bool Update(vec3& vCamPos, RMatrix& viewInverse);
	virtual void preRender();
	virtual void postRender();
public:
	XCombatCaption(MID nID, rs3::RSceneNode* pSceneNode);
	virtual ~XCombatCaption() {}
};

class XDamageCaption : public XCombatCaption
{
	friend class XCaptionMgr;	// 생성 / 소멸은 매니저를 통해
protected:
	bool	m_bCritical;

protected:
	virtual bool Update(vec3& vCamPos, RMatrix& viewInverse);
	virtual bool OnUpdate(float fDelta, RMatrix& viewInverse);

	void UpdatePosition( const float fElapsedTime, const float fDelta );
	virtual void UpdateScale( const float fElapsedTime );
	DWORD UpdateColor( const float fElapsedTime );
public:
	XDamageCaption(MID nID, rs3::RSceneNode* pSceneNode);
	virtual ~XDamageCaption() {}

	void SetupEnemy(const wchar_t* szDamage, vec3& pos, bool bCritical=false, uint32 nColor=0xFFFFFFFF);
	void SetupMine(const wchar_t* szDamage, vec3& pos, bool bCritical=false, uint32 nColor=0xFFFF0000);
};

class XDamageTextureCaption : public XDamageCaption, public MMemPool<XDamageTextureCaption>
{
protected:
	virtual bool Update(vec3& vCamPos, RMatrix& viewInverse);
	virtual void UpdateScale( const float fElapsedTime );

public:
	XDamageTextureCaption(MID nID, rs3::RSceneNode* pSceneNode);
	virtual ~XDamageTextureCaption() {}

	virtual	bool		IsFontRender()		{ return false; }
	virtual bool		CheckRenderByCamera(RCameraSceneNode* pCam);
};