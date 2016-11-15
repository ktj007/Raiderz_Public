#include "stdafx.h"
#include "RShaderComposer.h"
#include <sstream>
#include <fstream>
#include "MCRC32.h"

namespace rs3 {

//////////////////////////////////////////////////////////////////////////
/** RShaderComponent
@brief rsc 파일등의 쉐이더 파일 조각
RShaderComposer 를 사용해 조합합니다
*/

RShaderComponent::RShaderComponent() :
m_bInputStruct( false ),
m_bOutputStruct( false ),
m_bMainVS( false ),
m_bMainPS( false ),
m_nCurrentLine( 0 ),
m_dwCRC( 0 )
{
	m_strName.clear();
	m_strShaderString.clear();
	m_strErrorString.clear();
	m_strCurrentLine.clear();
	m_listDefines.clear();
	m_listDependencies.clear();
	m_mapExports.clear();
}


RShaderComponent::~RShaderComponent()
{

}

bool RShaderComponent::ParseInterface(istringstream& iss, int& nLine)
{
	if(!getline(iss, m_strCurrentLine) || m_strCurrentLine!="{")
	{
		++nLine;
		m_strErrorString = "{ expected";
		return false;
	}
	++nLine;

	while( getline(iss, m_strCurrentLine) )
	{
		++nLine;

		m_strCurrentLine = TrimRight(TrimLeft(m_strCurrentLine));

		if(m_strCurrentLine.empty()) continue;
		if(m_strCurrentLine=="}") return true;

		size_t keyPos = m_strCurrentLine.find_first_not_of(" \t");
		size_t valuePos = m_strCurrentLine.find_first_of("=");
		if(valuePos==string::npos)
		{
			m_strErrorString = " $key = value required in interface block";
			return false;
		}
		string strKey = TrimRight(m_strCurrentLine.substr( keyPos, valuePos - keyPos));
		string strValue = TrimRight(TrimLeft(m_strCurrentLine.substr( valuePos+1)));

		if(strKey=="$name")
			m_strName = strValue;
		else
		if(strKey=="$define")
			m_listDefines.push_back(strValue);
		else
		if(strKey=="$dependency")
			m_listDependencies.push_back(strValue);
		else
		{
			// unknown ??
		}
	}

	m_strErrorString = "} expected";
	return false;
}

bool RShaderComponent::Create(const string& strStringBuffer)
{
	istringstream iss(strStringBuffer);

	// 쉐이더 조각 토큰 상수 
	const char* RSC_TOKEN_EXPORT	= "EXPORT";
	const char* RSC_TOKEN_VSINPUT	= "VSINPUT";
	const char* RSC_TOKEN_VSOUTPUT	= "VSOUTPUT";
	const char* RSC_TOKEN_VSMAIN	= "VSMAIN";
	const char* RSC_TOKEN_PSMAIN	= "PSMAIN";

	m_bInputStruct = false;
	m_bOutputStruct = false;
	m_bMainVS = false;
	m_bMainPS = false;
	m_strName = "";
	m_strShaderString = "";

	m_nCurrentLine = 0;

	while( getline(iss, m_strCurrentLine) )
	{
		++m_nCurrentLine;
		m_strCurrentLine = TrimLeft(m_strCurrentLine);

		//  주석 태그 "//" 가 발견되면 이후 삭제
		size_t posComment = m_strCurrentLine.find("//");
		if( posComment!=string::npos )
			m_strCurrentLine = m_strCurrentLine.substr(0, posComment);

		if(m_strCurrentLine.empty()) continue;

		if(m_strCurrentLine=="interface()")
		{
			if(!ParseInterface(iss,m_nCurrentLine))
				return false;
		}else
		{
			// EXPORT 문 처리
			size_t posExport = m_strCurrentLine.find(RSC_TOKEN_EXPORT);
			if(posExport!=string::npos)
			{
				string strRemain = m_strCurrentLine.substr( posExport + strlen(RSC_TOKEN_EXPORT));
				size_t posOpen = strRemain.find_first_of("(");
				size_t posClose = strRemain.find_last_of(")");
				size_t posComma1 = strRemain.find_first_of(",");
				size_t posComma2 = strRemain.find_first_of(",",posComma1+1);

				if(posOpen==string::npos || posClose==string::npos || 
					posComma1==string::npos || posComma2==string::npos )
				{
					m_strErrorString = "EXPORT ( type, global name, value ) expected";
					return false;
				}

				string strParam1 = strRemain.substr( posOpen+1, posComma1 - posOpen -1);
				string strParam2 = strRemain.substr( posComma1+1, posComma2 - posComma1 -1 );
				string strParam3 = strRemain.substr( posComma2+1, posClose - posComma2 -1 );

				strParam1 = TrimRight(TrimLeft(strParam1));
				strParam2 = TrimRight(TrimLeft(strParam2));
				strParam3 = TrimRight(TrimLeft(strParam3));

				//				m_strExportString +=  strParam1 + " " + strParam2 + ";\n";
				m_mapExports.insert( map<string,string>::value_type( strParam2, strParam1 ) );
				m_strShaderString += "\t" + GetExportName(strParam2) + " = " + strParam3 + ";\n";

			}else
			{
				// VSINPUT -> VSINPUT_name 등으로 대체
				struct { 
					const char* szFind;
					bool* pbReplaced;
				} replaceTable[] =
				{	{ RSC_TOKEN_VSINPUT, &m_bInputStruct },
				{ RSC_TOKEN_VSOUTPUT, &m_bOutputStruct },
				{ RSC_TOKEN_VSMAIN, &m_bMainVS },
				{ RSC_TOKEN_PSMAIN, &m_bMainPS }
				};

				for(int i=0;i<_countof(replaceTable);++i)
					if(StringReplace(m_strCurrentLine, replaceTable[i].szFind, string(replaceTable[i].szFind)+"_"+m_strName))
						(*replaceTable[i].pbReplaced) = true;
				m_strShaderString += m_strCurrentLine;
				m_strShaderString += "\n";
			}
		}
	}

	return true;
}

bool RShaderComponent::CreateFromFile(const char *szFileName)
{
	_ASSERT( szFileName );

	MFile file;

	if( file.Open(szFileName) == false )
	{
		m_strErrorString = "file open error.";
		return false;
	}

	unsigned int fileSize = file.GetLength();
	char* pBuffer = new char[fileSize+1];
	pBuffer[fileSize] = 0;
	file.ReadAll( pBuffer, fileSize );
	file.Close();

	m_dwCRC = MCRC32::BuildCRC32( (BYTE*)pBuffer, fileSize );

	stringstream ss( pBuffer );
	delete [] pBuffer;
	string strCurrentLine;
	string strShaderString;
	while( getline(ss, strCurrentLine) )
	{
		strCurrentLine = TrimRight(strCurrentLine);
		strShaderString += strCurrentLine + "\n";
	}

	return Create( strShaderString.c_str() );
}

bool RShaderComponent::HasExport(const string& strExport)
{
	map<string,string>::iterator itr = m_mapExports.find( strExport );
	if(itr!=m_mapExports.end())
		return true;
	return false;
}


//////////////////////////////////////////////////////////////////////////
/**
@brief 조각 쉐이더 텍스트를 조합해주는 클래스
RShaderComponent 들을 모아서 쉐이더(텍스트)로 만들어줍니다
*/

bool RShaderComposer::Make()
{
	// 조합중 참조하는 스트링 토큰
	const char* RSC_TOKEN_TEXCOORD		= "TEXCOORD%";
	const char* RSC_TOKEN_IMPORT		= "IMPORT";

	m_nTexCoord = 0;
	m_nTexCoordIn = 0;

	// 포워드든 인퍼드든 랜더링 전략에 상관 없이 같은 포맷의 아웃풋을 사용 하고,
	// 랜더링 시 MRT 타겟 지정을 통해 필요 한 것만 사용 되게 한다.
	m_strShader = "";
	Add("\nstruct PSOUTPUT {\n");
	Add("\tfloat4 color  : COLOR0;\n");
	Add("\tfloat4 color1 : COLOR1;\n");
	Add("\tfloat4 color2 : COLOR2;\n");
	Add("};\n\n");

	Add("////////////////////////////////////////////////////////////////////////// EXPORTS\n\n");

	//////////////////////////////////////////////////////////////////////////
	// EXPORT 들을 모아서 미리 선언해 둔다
	for( ComponentList::iterator itr = m_components.begin(); itr!=m_components.end(); ++itr)
	{
		RShaderComponent* pComponent = *itr;
		if(!pComponent->m_mapExports.empty())
		{
			map<string,string>::iterator itr = pComponent->m_mapExports.begin();
			for( ; itr!= pComponent->m_mapExports.end(); ++itr)
			{
				string strExport = itr->second + " " + pComponent->GetExportName(itr->first) + ";\n";
				Add (strExport);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// COMPONENT 별로 구조체/펑션을 출력
	for( ComponentList::iterator itr = m_components.begin(); itr!=m_components.end(); ++itr)
	{
		RShaderComponent* pComponent = *itr;

		Add("\n////////////////////////////////////////////////////////////////////////// component : ");
		Add( pComponent->GetName());
		Add("\n");

		istringstream streamShaderString( pComponent->GetShaderString() );

		string aLine;
		while( getline(streamShaderString, aLine) )
		{
			// IMPORT 문 처리
			size_t posImport = aLine.find(RSC_TOKEN_IMPORT);
			if(posImport!=string::npos)
			{
				string strRemain = aLine.substr( posImport + strlen(RSC_TOKEN_IMPORT));
				size_t posOpen = strRemain.find_first_of("(");
				size_t posClose = strRemain.find_last_of(")");
				size_t posComma = strRemain.find_first_of(",");

				if(posOpen==string::npos || posClose==string::npos || posComma==string::npos )
				{
					m_strErrorString = "IMPORT ( global name, expression) expected";
					return false;
				}

				string strMetaFunctionComment = " // metafunction: " + aLine + "\n";
				Add(strMetaFunctionComment);

				string strParam1 = strRemain.substr( posOpen+1, posComma - posOpen -1);
				string strParam2 = strRemain.substr( posComma+1, posClose - posComma -1 );

				strParam1 = TrimRight(TrimLeft(strParam1));
				strParam2 = TrimRight(TrimLeft(strParam2));

				for( ComponentList::iterator j = m_components.begin(); j!=m_components.end(); ++j)
				{
					/// 현재 컴포넌트 이전 순서까지만 Export구문이 있는 지를 체크한다. 현재 컴포넌트를 지나서 있는 Export구문은 반영이 되면 안된다.
					if(itr==j) break;
					RShaderComponent* pComponent = *j;
					if(pComponent->HasExport(strParam1))
					{
						string strImportLine = strParam2;

						size_t posExport = 0;
						while( (posExport = strImportLine.find(strParam1,posExport))!=string::npos )
						{
							string strTo = pComponent->GetExportName(strParam1);
							strImportLine.replace( posExport, strParam1.length(), strTo );
							posExport+= strTo.length();
						}

						strImportLine = "\t" + strImportLine + ";\n";

						Add(strImportLine);
					}
				}
			}
			// 일반 문장은 복사
			else
			{
				// TEXCOORD% 문을 적절한 숫자로 대체
		 		size_t posTexCoordOutput = aLine.find(RSC_TOKEN_TEXCOORD);
		 		if(posTexCoordOutput!=string::npos)
		 		{
					if( m_nTexCoord>=8)
					{
						// TEXCOORD8 이 넘어갔습니다. 대책있나요 ?
//						_ASSERT(FALSE);
						m_strErrorString += "TEXCOORD8 이 넘어갔습니다.";
//						return false;
					}
		 			// % 를 숫자로 대체
		 			char buffer[32];
		 			sprintf(buffer,"%d",m_nTexCoord++);
		 			aLine.replace( posTexCoordOutput + strlen(RSC_TOKEN_TEXCOORD)-1, 1, buffer);
		 		}
			 
				Add(aLine);
				Add("\n");
			}
		}

	}

	Add("\n\n////////////////////////////////////////////////////////////////////////// MAIN\n");
	// 합쳐진 VS_INPUT 만들기
	Add( "struct VSINPUT {\n");
	for( ComponentList::iterator itr = m_components.begin(); itr!=m_components.end(); ++itr)
	{
		RShaderComponent* pComponent = *itr;
		if(pComponent->HasInputStruct())
		{
			Add("\tVSINPUT_");
			Add( pComponent->GetName() );
			Add( " ");
			Add( pComponent->GetName() );
			Add( ";\n");
		}
	}
	Add("};\n");

	// 합쳐진 VS_OUTPUT 만들기
	Add( "struct VSOUTPUT {\n");
	for( ComponentList::iterator itr = m_components.begin(); itr!=m_components.end(); ++itr)
	{
		RShaderComponent* pComponent = *itr;
		if(pComponent->HasOutputStruct())
		{
			Add("\tVSOUTPUT_");
			Add( pComponent->GetName() );
			Add( " ");
			Add( pComponent->GetName() );
			Add( ";\n");
		}
	}
	Add("};\n\n");

	// main vs
	Add("\nVSOUTPUT MainVS(VSINPUT In) {\n");
	Add("\tVSOUTPUT output = (VSOUTPUT)0;\n");

	// 각 콤포넌트의 vs function 들을 차례로 부름
	for( ComponentList::iterator itr = m_components.begin(); itr!=m_components.end(); ++itr)
	{
		RShaderComponent* pComponent = *itr;
		if(pComponent->HasVS())
		{
			Add( "\tVSMAIN_");
			Add( pComponent->GetName() );
			Add( "( ");
			if(pComponent->HasInputStruct())
			{
				Add("In.");
				Add( pComponent->GetName() );
			}
			if(pComponent->HasInputStruct() && pComponent->HasOutputStruct())
				Add(", ");
			if(pComponent->HasOutputStruct())
			{
				Add("output.");
				Add( pComponent->GetName() );
			}

			Add(" );\n");
		}
	}
	Add("\treturn output;\n}\n");

	// main ps
	Add("\nPSOUTPUT MainPS(VSOUTPUT In) {\n");
	Add("\tPSOUTPUT output = (PSOUTPUT)0;\n");

	// 각 콤포넌트의 ps function 들을 차례로 부름
	for( ComponentList::iterator itr = m_components.begin(); itr!=m_components.end(); ++itr)
	{
		RShaderComponent* pComponent = *itr;
		if(pComponent->HasPS())
		{
			Add( "\tPSMAIN_");
			Add( pComponent->GetName() );
			Add( "( ");
			if(pComponent->HasOutputStruct())
			{
				Add("In."); 
				Add( pComponent->GetName() );
				Add(", ");
			}
			Add("output );\n");
		}
	}
	Add("\treturn output;\n}\n\n");

	// technique
	Add("Technique MainTechnique\n");
	Add("{\n");
	Add("\tpass P0\n");
	Add("\t{\n");
	Add("\t\tVertexShader = compile vs_3_0 MainVS();\n");
	Add("\t\tPixelShader = compile ps_3_0 MainPS();\n");
	Add("\t}\n");
	Add("}\n");

	return true;
}

bool RShaderComposer::AddComponent( RShaderComponent* pComponet )
{
	const list<string>& listDependency = pComponet->GetDependencies();
	for(list<string>::const_iterator itr = listDependency.begin(); itr!= listDependency.end(); ++itr)
	{
		set<string>::iterator itrFind = m_setDefines.find( *itr );
		if(itrFind == m_setDefines.end())
		{
			m_strErrorString = pComponet->GetName() + " requires " + *itr + " define.";
			return false;
		}
	}

	const list<string>& listDefines = pComponet->GetDefines();
	for(list<string>::const_iterator itr = listDefines.begin(); itr!= listDefines.end(); ++itr)
	{
		set<string>::iterator itrFind = m_setDefines.find( *itr );
		if(itrFind != m_setDefines.end())
		{
			m_strErrorString = pComponet->GetName() + " " + *itr + " already defined.";
			return false;
		}
	}

	// 에러가 없으면 디파인 추가후 콤포넌트 추가
	for(list<string>::const_iterator itr = listDefines.begin(); itr!= listDefines.end(); ++itr)
	{
		m_setDefines.insert( *itr );
	}

	m_components.push_back(pComponet);
	return true;
}

}