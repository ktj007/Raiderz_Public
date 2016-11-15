#pragma once

#include "MWidget.h"
#include "MButton.h"
#include "MFileBox.h"
#include "MEdit.h"
#include "MFrame.h"
#include "MMsgBox.h"

namespace mint3 {

/// 파일 다이얼로그 타입
enum MFileDialogType{
	MFDT_OPEN = 0,	///< 오픈 다이얼로그
	MFDT_SAVE,		///< 세이브 다이얼로그
};

/// 파일 다이얼로그 박스
class MFileDialog : public MFrame{
	MButton*		m_pOK;
	MButton*		m_pCancel;
	MFileBox*		m_pFileBox;
	MEdit*			m_pFileName;
	MFileDialogType	m_nType;
	MMsgBox*		m_pMsgBox;
	char			m_szTempPathName[256];
protected:
	//virtual void OnDraw(MDrawContext* pDC);
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
	bool IsExistFile(const char* szFileName);

public:
	MFileDialog(const char* szFilter, MWidget* pParent, MListener* pListener=NULL);
	~MFileDialog(void);

	const char* GetFileName(void);
	const char* GetPathName(void);
	void Refresh(const char* szFilter=NULL);

	const char* GetBaseDir(void);

	void SetType(MFileDialogType fdt);

#define MINT_FILEDIALOG	"FileDialog"
	virtual const char* GetClassName(void){ return MINT_FILEDIALOG; }
};

} // namespace mint3