@echo off
del ..\*.* /q
copy TestRS3_T.* ..\TestRS3_ref.*
svn info > ..\svnInfo_Runtime.txt
svn info mdk > ..\svnInfo_Develop.txt

TestRS3_T.exe /r