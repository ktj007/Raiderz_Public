#pragma once

namespace rs3
{
	class RToolCamera;
};

class CMainFrame;
class CORETOOL_API CXMLUserConfig
{
public:
	inline static CXMLUserConfig& GetReference()
	{
		static CXMLUserConfig userConfig;
		return userConfig;
	}

	//파일이 열리거나 저장될때(파일과 같이 움직임), ControlMediator 내에서 작동
	//.xxx.xml 에 대응하여 .config.xml 파일로 저장
	bool LoadConfigRelatedMapFile(const char *filename, rs3::RToolCamera* _pToolCamera);
	void SaveConfigRelatedMapFile(const char *filename, rs3::RToolCamera* _pToolCamera);
	void SetLastCameraInfo(const rs3::RVector &_vPos, const rs3::RVector &_vDir);
	bool GetLastCameraInfo(rs3::RVector &_vPos, rs3::RVector &_vDir) const;
	void SetLastWork(int _lastWork);
	bool GetLastWork(int& _lastWork);

	//Environment Setter / Getter
	void SetEnvironmentInfo(bool _bVisible, int _time, int _weather, std::string& _strEnvSet);
	void GetEnvironmentInfo(bool& _bVisible, int& _time, int& _weather, std::string& _strEnvSet);

	//프로그램이 시작되거나 종료될때(프로그램과 같이 움직임)
	//user.config.xml 로 저장
	void LoadConfigRelatedUserTool(CControlMediator* pControlMediator);
	void SaveConfigRelatedUserTool(CControlMediator* pControlMediator);

	//ref-mesh
	void SetRefMeshName(const char* _pMeshName);
	const char* GetRefMeshName(); //이건 control mediator 에서

	//for Script Edition
	const string& GetTextEditor(){ return m_strTextEditor_; }
	void SetTextEditor(const char* _pSzTextEditor);

	//for Server
	const string& GetTextLoginID(){ return m_strTextLoginID_; }
	void SetTextLoginID(const char* _pSzTextLoginID);
	const string& GetTextLoginIP(){ return m_strTextLoginIP_; }
	void SetTextLoginIP(const char* _pSzTextLoginIP);

	//Options
	bool IsCenterPivot()
	{
		if(m_nCenterPivot_ == 1) return true;
		else return false;
	}
	bool IsRememberWork()
	{
		if(m_nRememberWork_ == 1) return true;
		else return false;
	}
	bool Is1st3rdPerson()
	{
		if(m_b1st3rdPerson) 
			return true;
		
		return false;
	}
	void SetCenterPivot(int _nCenter){ m_nCenterPivot_ = _nCenter; }
	void SetRememberWork(int _nRemember){ m_nRememberWork_ = _nRemember; }
	void Set1st3rdPerson(bool b1st3rd) { m_b1st3rdPerson = b1st3rd; }

	~CXMLUserConfig(void);	
private:
	CXMLUserConfig(void);

	//related with map file
	//inner structure for camera infomation
	struct CAMERA_INFO{
		CAMERA_INFO() : vPos(0){}
		rs3::RVector vPos;
		rs3::RVector vDir;
		float fWASDSpeed;
		float fWheelSpeed;
	} m_lastCameraInfo_;

	//last work
	int m_nWbState_;

	//User Option
	int m_nRememberWork_;
	int m_nCenterPivot_;
	bool m_b1st3rdPerson;

	struct ENVIRONMENT_INFO{
		ENVIRONMENT_INFO() : m_nTime(0), m_nWeather(0), m_strEnvSetName("default"), m_bVisible(true) {}
		int m_nTime;
		int m_nWeather;
		bool m_bVisible;
		std::string m_strEnvSetName;
	} m_lastEnvInfo_;

	//reference mesh string
	std::string m_strRefMesh_;

	//Text Editor
	std::string m_strTextEditor_;

	// Server
	std::string m_strTextLoginID_;
	std::string m_strTextLoginIP_;

	template<typename T>
	inline void T_SetMapStrengthControl_(T* _type, double _dValue, float _fRate)
	{
		_type->m_fStr = float(_dValue);
		int str = (int)(_type->m_fStr / _fRate);
		_type->m_sliderCtrlStr.SetPos(str);
	}

	template<typename T>
	inline void T_GetMapStrengthControl_(T* _type, float& _refValue)
	{
		_refValue = _type->m_fStr;
	}
};
