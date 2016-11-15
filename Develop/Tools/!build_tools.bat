@echo off

@rem 경로 설정 배치파일 실행
call "%VS80COMNTOOLS%vsvars32.bat"

@rem 각 툴을 빌드
devenv /build Release "./Character_NET/Character_NET.sln"
devenv /build Release "./CoreToolRS3/CoreToolRS3.sln"
devenv /build Release "./EffectTool_NET/EffectTool_NET.sln"
