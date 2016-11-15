#include "stdafx.h"

using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;

//
// 어셈블리에 대한 일반 정보는 다음 특성 집합을 통해 제어됩니다. 
// 어셈블리와 관련된 정보를 수정하려면
// 이 특성 값을 변경하십시오.
//
[assembly:AssemblyTitleAttribute("")];
[assembly:AssemblyDescriptionAttribute("")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("")];
[assembly:AssemblyProductAttribute("")];
[assembly:AssemblyCopyrightAttribute("")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];		

//
// 어셈블리의 버전 정보는 다음 네 가지 값으로 구성됩니다.
//
//      주 버전
//      부 버전 
//      빌드 번호
//      수정 번호
//
// 모든 값을 지정하거나
// 아래와 같이 '*'를 사용하여 수정 번호 및 빌드 번호가 자동으로 지정되도록 할 수 있습니다.

[assembly:AssemblyVersionAttribute("1.0.*")];

//
// 어셈블리에 서명하려면 사용할 키를 지정해야 합니다. 
// 어셈블리 서명에 관한 자세한 내용은 Microsoft .NET Framework 설명서를 참조하십시오.
//
// 다음 특성을 사용하면 어떤 키를 사용하여 서명할지 제어할 수 있습니다. 
//
// 참고: 
//   (*) 키를 지정하지 않으면 어셈블리에 서명할 수 없습니다.
//   (*) KeyName은
//       사용자 컴퓨터에서 CSP(암호화 서비스 공급자)에 설치되어 있는 키를 참조하고
//       KeyFile은 키가 포함된 파일을 참조합니다.
//   (*) KeyFile과 KeyName 값을 모두 지정하면
//       다음과 같은 프로세스가 생성됩니다.
//       (1) CSP 안에 KeyName이 있으면 해당 키가 사용됩니다.
//       (2) KeyName은 없고, KeyFile이 있으면 
//           KeyFile 안에 있는 키가 CSP에 설치된 후 사용됩니다.
//   (*) sn.exe(강력한 이름 유틸리티)를 사용하면 KeyFile을 만들 수 있습니다.
//        KeyFile을 지정하는 경우
//        KeyFile의 위치는 프로젝트 디렉터리를 기준으로 하는 상대 위치이어야 합니다.
//   (*) 서명 연기는 고급 옵션입니다.
//       이 옵션에 대한 자세한 내용은 Microsoft .NET Framework 설명서를 참조하십시오.
//
[assembly:AssemblyDelaySignAttribute(false)];
[assembly:AssemblyKeyFileAttribute("")];
[assembly:AssemblyKeyNameAttribute("")];

