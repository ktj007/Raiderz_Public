#include "StdAfx.h"
#include "XHelpFrm.h"

XHelpFrm::XHelpFrm( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: mint3::MTextFrame(szName, pParent, pListener)
{
	Mint* pMint = global.ui->GetMint();

	ShowTitleBar( false);
	SetRect(pMint->GetWorkspaceWidth()-480, 100, 480, 567);
	SetText("µµ¿ò¸»");
	SetResizable(false);
}

void XHelpFrm::OnDraw( MDrawContext* pDC )
{
	MTextFrame::OnDraw(pDC);

	static bool bOpenedFile = false;
	static vector<string>	m_stHelpTextVec;
	const char* SCREEN_HELP_FILE = "help.txt";

	if (!bOpenedFile)
	{
		FILE* fp;
		if( fopen_s( &fp, SCREEN_HELP_FILE, "r" ) == 0 )
		{
			char szBuf[1024] = "";
			string strLine;

			while ( fgets( szBuf, 1024, fp ) != NULL)
			{
				strLine = szBuf;
				m_stHelpTextVec.push_back(strLine);
			}
			fclose( fp );
		}

		bOpenedFile = true;
	}

	MRECT r = GetInitialClientRect();
	r.x += 10;
	r.y += 10;
	pDC->SetColor( 255, 255, 255);
	for (size_t i = 0; i < m_stHelpTextVec.size(); i++)
	{
		pDC->Text(r.x, r.y+20*i, m_stHelpTextVec[i].c_str());
	}
}
