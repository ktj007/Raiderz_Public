#pragma once

#include "MTextView.h"


using namespace std;
using namespace mint3;



#define MDEFAULT_BALLOONTEXTBOXLOOK		"__defaultballoontextboxlook"



/// class : XUIBalloonTextBox
class XUIBalloonTextBox : public MTextView
{
protected:
	MUID		m_UID;
	MTimer		m_tFade;					// 말풍선의 fade in/out을 체크함
	MTimer		m_tLife;					// 말풍선이 사라지기까지 시간을 체크함


public:
	XUIBalloonTextBox( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	void SetOwner( const MUID& uID)							{ m_UID = uID;							}

	void PopupBalloon( const wstring& strMessage, float fLifeTime);

private:
	RVector2 GetBalloonPosition();

protected:
	virtual int GetDrawState() override;


protected:
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnPreDraw( MDrawContext* pDC) override;
	virtual void OnShow( bool bShow) override;

	void		RePosition();
	bool		CheckDistance();
	bool		CheckPickLine();


#define MINT_BALLOONTEXTBOX		"BalloonTextBox"
	virtual const char* GetClassName() override				{ return MINT_BALLOONTEXTBOX;			}
};

