@echo off

call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall"
call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall"

SET target_path=.

devenv /rebuild release MCPlug2008.sln
devenv /rebuild release2008 MCPlug2008.sln
devenv /rebuild release2009 MCPlug2008.sln
devenv /rebuild release2011 MCPlug2008.sln