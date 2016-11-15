#if !defined(AFX_AUTOEDIT_H__056ECEAA_9283_11D3_8599_00105A744766__INCLUDED_)
#define AFX_AUTOEDIT_H__056ECEAA_9283_11D3_8599_00105A744766__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoEdit.h : Header-Datei
//

#include "infownd.h"
#include "../WM_USER.h"

class CEditValidate
{
public:
    CEditValidate() {};
    ~CEditValidate() {};
    virtual CString validate(CString& str) { return(_T(""));};
};


/////////////////////////////////////////////////////////////////////////////
// Fenster CAutoEdit 

class CAutoEdit : public CEdit
{
// Konstruktion
public:
    CAutoEdit();

// Attribute
public:

// Operationen
public:

// ?erschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions?erschreibungen
	//{{AFX_VIRTUAL(CAutoEdit)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementierung
public:
	void SetText(const char* str = NULL);
	void SetText(const float value);
    //void SetExitChar(int nChar = '.') { m_nExitChar = nChar;}; // Sets the Character used to exit this field
    void SetValidChar(char* pChar)                             // Set the array with valid characters
    { 
        if (m_nValidChar) delete [] m_nValidChar;
        m_nValidChar = new char[_tcsclen(pChar) + 1];

		#pragma warning( push )
		#pragma warning( disable : 4996 )
        _tcscpy(m_nValidChar,pChar);
		#pragma warning( pop )
    }; 
    void SetMaxChars(int nMax = -1)  {m_nMaxChar = nMax;LimitText(nMax);};      // Set the max characters, if set the field is skipped when the last character is typed
    void SetSignHandler(bool bOnce = true ,bool bInFront = true) {m_bSignOnce = bOnce; m_bSignInFront = bInFront;};
    void SetDefaultValue(LPCSTR p) {m_sDefault = p;};
    void SetNullPadding(bool b = true) {m_bNullPad = b;};
    void SetValidationHandler(CEditValidate* p) {m_validateProc = p;};
    int CountNoOfChars();
    int CountSigns();

    virtual ~CAutoEdit();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CAutoEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    void ArrangeSign();
protected:
	//UINT m_nExitChar;   // This character acts like a TAB, e.g a '.' in a numeric field
    int m_nMaxChar;     // -1 unused, > 0 leaves the field automatically
	char* m_nValidChar; // NULL or a array of valid keys, eg. "0123456789" fo numeric only !
    bool m_bSignOnce;
    bool m_bSignInFront;
    bool m_bNullPad;
    CString m_sDefault;
    CEditValidate* m_validateProc;
    CInfoWnd* m_pInfoWnd;
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f?t unmittelbar vor der vorhergehenden Zeile zus?zliche Deklarationen ein.

#endif // AFX_AUTOEDIT_H__056ECEAA_9283_11D3_8599_00105A744766__INCLUDED_
