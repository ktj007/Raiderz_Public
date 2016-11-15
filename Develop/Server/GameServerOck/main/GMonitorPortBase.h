#pragma once

class GMonitorPortBase
{
public:
	class Sender
	{
	public:
		virtual ~Sender() {}
		virtual bool Send(const wchar_t* szName, string strMessage) = 0;
	};

	class ToViewerSender : public Sender, public MTestMemPool<ToViewerSender>
	{
	public:
		virtual bool Send( const wchar_t* szName, string strMessage ) override;
	};
public:
	GMonitorPortBase(void);
	virtual ~GMonitorPortBase(void);

	virtual void Send( string strMessage );
	void LaunchMonitor();
	void SetSender(Sender* pSender);

	virtual bool IsEnable() = 0;
	virtual const wchar_t* GetName() = 0;
	virtual const char* GetFilePath() = 0;

	virtual void OnSendFailed() {}

private:
	Sender* m_pSender;
};

template<typename T>
void AddXMLElement(string strName, const T& var, MXmlElement* pParent)
{
	MXmlElement* pNew = new MXmlElement(strName.c_str());
	_SetContents(pNew, var);
	pParent->LinkEndChild(pNew);
}