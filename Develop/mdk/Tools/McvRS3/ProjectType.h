/*
	프로젝트에 따른 설정을 미리 정해주는 클래스.
	프로젝트에 맞게 수정해서 사용하세요.
*/


#pragma once



enum PROJECT_TYPE
{
	PT_NONE			= 0,

	PT_RAIDERZ,						// Raiderz
	PT_GUNZ,						// Gunz

	PT_END
};


const char FIND_FILE_NAME[ PT_END][ 256] = { "", "Raiderz.exe", "Gunz2.exe" };



class ProjectTypeInfo
{
public:
	PROJECT_TYPE	m_ProjectType;
	string			m_strPrefix;
	string			m_strPathRoot;				// 파일을 연 폴더
	string			m_strPathRuntime;			// Runtime 패쓰
	string			m_strPathSound;				// Sound 패쓰
	string			m_strFnameSndXml;			// sound.xml 패쓰

	ProjectTypeInfo() : m_ProjectType( PT_NONE)		{ }
};




class ProjectType
{
protected:
	ProjectTypeInfo*	m_pProjectTypeInfo;


public:
	ProjectType()							{ m_pProjectTypeInfo = new ProjectTypeInfo;					}
	virtual ~ProjectType()					{ if ( m_pProjectTypeInfo)	delete m_pProjectTypeInfo;		}

	void			SetProjectType( const char* szPath);
	PROJECT_TYPE	GetProjectType()		{ return m_pProjectTypeInfo->m_ProjectType;					}
	const char*		GetPrefix()				{ return m_pProjectTypeInfo->m_strPrefix.c_str();			}
	const char*		GetPathRoot()			{ return m_pProjectTypeInfo->m_strPathRoot.c_str();			}
	const char*		GetPathRuntime()		{ return m_pProjectTypeInfo->m_strPathRuntime.c_str();		}
	const char*		GetPathSound()			{ return m_pProjectTypeInfo->m_strPathSound.c_str();		}
	const char*		GetFnameSoundXml()		{ return m_pProjectTypeInfo->m_strFnameSndXml.c_str();		}
};




class ProjectTypeInfoSoulHunt : public ProjectTypeInfo
{
public:
	ProjectTypeInfoSoulHunt( const char* szPathRuntime);
};


class ProjectTypeInfoGunz : public ProjectTypeInfo
{
public:
	ProjectTypeInfoGunz( const char* szPathRuntime);
};