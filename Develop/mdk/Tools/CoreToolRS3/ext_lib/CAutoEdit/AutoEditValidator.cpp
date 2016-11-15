#include "stdafx.h"
#include "AutoEditValidator.h"

CString CGeneralValidate::validate(CString& str)
{
    return(_T(""));
}

CString CColorPickerValidate::validate(CString& str)
{
    if (atoi(str) > 255) {
        return("RGB의 범위는 255 까지입니다");
    }

    return(_T(""));
}
