#pragma once

class VErrorMsg;
class VEffectInfoMgr;
class VSoundInfoMgr;
class VCameraInfoMgr;

//////////////////////////////////////////////////////////////////////////
bool ExistFile(const char* str);

void CheckExistXmlFile(string strFileName, VErrorMsg* pErrorMsg);
void CheckExistFile(string strFileName, VErrorMsg* pErrorMsg);

string TransToStringFromNumber(int nNum);