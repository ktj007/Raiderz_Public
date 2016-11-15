/// @file C_XMLConfig.h
#pragma once

#include <string>

/**
	@brief 툴 환경설정 XML 관리 클래스

	XML 형식의 파일을 읽어 환경설정을 하는 싱글턴 클래스 입니다.
*/
class CORETOOL_API CXMLConfig
{
protected:
	CXMLConfig();
	~CXMLConfig();
public:
	// 기본설정은 DEFAULT_PATH_DATA 값으로 설정된다.
	// 각각의 패쓰들은 pcDataPath로부터 자동 설정.
	char pcDataPath[MAX_PATH];
	std::string strDataFullPath;
	
	//트리
	char pcTreepath[MAX_PATH];

	//elu
	char pcEluPath[MAX_PATH];			///< model path
	char pcMapObjectPath[MAX_PATH];		///< 배치될 map object path
	char pcSkyPath[MAX_PATH];
	
	char pcEftpath[MAX_PATH];
	char pcShaderpath[MAX_PATH];
	char pcEffectpath[MAX_PATH];

	// 사운드
	char pcSoundPath[MAX_PATH];
	char pcBGMPath[MAX_PATH];
	char pcEffectSoundPath[MAX_PATH];

	//텍스쳐
	char pcTilepath[MAX_PATH];
	std::string	strTexturePath;

	//animation
	std::string strCameraAniPath;

	//field data
	std::string strServerPath;
	std::string strServerScriptPath;
	std::string strServerScriptFullPath;
	std::string strServerFieldScriptFullPath;
	std::string strServerNPCScriptFullPath;
	std::string strServerFieldPath;
	std::string strServerFieldFullPath;
	std::string strServerZonePath;
	std::string strServerZoneFullPath;

	float fMaxFarZ;
	float fMinFarZ;
	float fWheelZoomFactor;
	float fKeyZoomFactor;
	float fVelocity;

	float fRotateStep;
	float fPanStep;

	bool LoadXML(const char *filename);
	void SaveXML(const char *filename);

	static CXMLConfig* GetInstance()
	{
		static CXMLConfig kConfig;
		return &kConfig;
	};

private:
	void SetPathesFromDataPath_();
};
