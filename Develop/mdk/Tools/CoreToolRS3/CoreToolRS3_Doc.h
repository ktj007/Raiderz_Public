// SDIdocDoc.h : CCoreToolRS3Doc 클래스의 인터페이스
//
#pragma once

class MVector3;

class CControlMediator;
class CMainFrame;
class CDlgMapNew;
class CDlgMapInfo;
class CDlgProfileInfo;
class CCoreToolRS3Doc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CCoreToolRS3Doc();
	DECLARE_DYNCREATE(CCoreToolRS3Doc)

public:
	virtual ~CCoreToolRS3Doc();

	#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	#endif

	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);
	void InitDocument(CControlMediator* _pControlMediator, CMainFrame* _pMainFrame);
	MAP_EDIT_TYPE OpenMapFile(const CString& sFile);
	void SaveMap(const char* _pSzSaveAsFileName = NULL);
	void ExportObj(int nLevel, int nW, int nH);
	void ImportObj(const char *szFilename);
	void EditUndo();

	// Enter / Exit 관련
	CString		EnterFileName;
	int*		ExitCode;

private:
	CControlMediator* m_pControlMediator_;
	CMainFrame* m_pMainFrame_;
	enum LOAD_TYPE
	{
		SCENE_LOAD = 0,
		FROM_WORKSPACE
	};
	int ConfirmSaveAndClose(MAP_EDIT_TYPE& _maptype, LOAD_TYPE _type, const char* filename, const std::string& _rWorkSpaceKeyword);
	void SetToolAfterMapLoaded_(MAP_EDIT_TYPE _type, const char* _fileName, const std::string& _rWorkSpaceKeyword);

	// 생성된 메시지 맵 함수
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnCameraReset();
	afx_msg void OnGoToSelectCenter();
	afx_msg void OnSetReferenceMesh();
	afx_msg void OnRelaxAll();
	afx_msg void OnWaterPlaneEdit();
	afx_msg void OnResourceRefresh();
};
