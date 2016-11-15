using System.Reflection;
using System.Runtime.CompilerServices;

//
// 어셈블리에 대한 일반 정보는 다음 특성 집합을 통해 제어됩니다. 
// 어셈블리와 관련된 정보를 수정하려면
// 이 특성 값을 변경하십시오.
//
[assembly: AssemblyTitle("")]
[assembly: AssemblyDescription("")]
[assembly: AssemblyConfiguration("")]
[assembly: AssemblyCompany("")]
[assembly: AssemblyProduct("")]
[assembly: AssemblyCopyright("")]
[assembly: AssemblyTrademark("")]
[assembly: AssemblyCulture("")]		

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

[assembly: AssemblyVersion("1.0.*")]

//
// 어셈블리에 서명하려면 사용할 키를 지정해야 합니다. 어셈블리 서명에 대한 자세한 내용은 
// Microsoft .NET Framework 설명서를 참조하십시오.
//
// 서명하는 데 사용할 키를 제어하려면 아래 특성을 사용합니다. 
//
// 참고: 
//   (*) 키를 지정하지 않으면 어셈블리에 서명할 수 없습니다.
//   (*) KeyName은
//       사용자 컴퓨터의 CSP(암호화 서비스 공급자)에
//        설치되어 있는 키를 참조하고 KeyFile은 키가 포함된 파일을
//        참조합니다.
//   (*) KeyFile과 KeyName 값을 모두 지정하면 
//       다음과 같은 프로세스가 발생합니다.
//       (1) CSP에 KeyName이 있으면 해당 키가 사용됩니다.
//       (2) KeyName은 없고, KeyFile이 있으면 
//           KeyFile의 키가 CSP에 설치되어 사용됩니다.
//   (*) sn.exe(강력한 이름 유틸리티)를 사용하면 KeyFile을 만들 수 있습니다.
//        KeyFile을 지정하는 경우
//       KeyFile의 위치는 %Project Directory%\obj\<configuration>의 프로젝트 출력 디렉터리 위치를 기준으로 하는 상대 위치이어야 합니다.
//       예를 들어, KeyFile이 프로젝트 디렉터리에 있는 경우
//       AssemblyKeyFile 특성을 
//       [assembly: AssemblyKeyFile("..\\..\\mykey.snk")]로 지정합니다.
//   (*) 서명 연기는 고급 옵션입니다.
//       이 옵션에 대한 자세한 내용은 Microsoft .NET Framework 설명서를 참조하십시오.
//
[assembly: AssemblyDelaySign(false)]
[assembly: AssemblyKeyFile("")]
[assembly: AssemblyKeyName("")]
