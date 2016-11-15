@echo off
svn cleanup . RealSpace3\Dependencies\dx9 RealSpace3\Dependencies\speedtree RealSpace3\Dependencies\speedgrass RealSpace3\Dependencies\gtest

svn up .
IF NOT %ERRORLEVEL%==0 EXIT 1