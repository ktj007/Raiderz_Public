#pragma once


namespace rs3 {

// Tool에서 사용하는 Application framework
class RToolApplication
{
protected:
	bool			m_bActive;
	RDevice			*m_pDevice;
	RSceneManager	*m_pSceneManager;
public:
	RToolApplication(void);
	virtual ~RToolApplication(void);

	virtual bool Create();
	virtual void Destroy();
	//virtual void Update();
	//virtual void Render();

	virtual void Activate(bool bActive);
	virtual char *GetName() { return "ToolApplication"; }

	virtual RDevice*		GetDevice()				{ return m_pDevice; }
	virtual RSceneManager*	GetSceneManager()		{ return m_pSceneManager; }

};

}
