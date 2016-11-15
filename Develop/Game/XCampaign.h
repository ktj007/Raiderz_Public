
#pragma once


using namespace rs3;




class XWorld;



/// class : XCampaign
class XCampaign
{
/// Member variables
protected:
	bool					m_bCreated;
	bool					m_bLoadingCompleted;
	XWorld*					m_pWorld;
	unsigned int			m_SkipRenderCount;
	bool					m_bCallOnCreate;
	bool					m_bCallOnLoadingComplete;
	int						m_nSceneIndex;




/// Member functions
public:
	XCampaign();
	virtual ~XCampaign();

	bool Create();
	bool Destroy();

	bool IsLoadingCompleted();

	void Update( float fDelta);
	void Render();

	void ChangeScene( int nScene);

protected:
	virtual bool CheckLoadingCompleted();


/// Event handler
protected:
	virtual void OnCreate();
	virtual void OnDestroy();
	virtual void OnLoadingComplete();
	virtual void OnUpdate( float fDelta);
	virtual void OnRender();
	virtual void OnChangedScene( int nPrev, int nScene);
};






class XNonPlayer;


/// class : XLoginCampaign
class XLoginCampaign : public XCampaign
{
/// Member variables
protected:
	int					m_nRestoreFOV;
	bool				m_bMoveCamera;
	DWORD				m_dwCamMoveTimer;


/// Member functions
public:
	XLoginCampaign();

	static XLoginCampaign* GetInstance();

protected:
	virtual bool CheckLoadingCompleted() override;


/// Event handler
protected:
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void OnLoadingComplete() override;
	virtual void OnUpdate( float fDelta) override;
	virtual void OnChangedScene( int nPrev, int nScene) override;
};






// inline : XGetLoginCampaign
inline XLoginCampaign* XGetLoginCampaign()
{
	return XLoginCampaign::GetInstance();
}
